#include <glis/ipc/server_core.hpp>
#include <glis/internal/log.hpp>
#include <fstream>
#include <zconf.h>
#include <sys/time.h> // gettimeofday

SOCKET_SERVER& SOCKET_SERVER::operator=(const SOCKET_SERVER&) {
    abort();
}

bool SERVER_LOG_TRANSFER_INFO = true;
const char * LOG_TAG_SERVER = "server";
Kernel SERVER_KERNEL;
class SERVER_MESSAGES SERVER_MESSAGES;

void SERVER_SHUTDOWN(char *server_name, SOCKET_SERVER_DATA *&internaldata) {
    if (internaldata->server_closed) {
        LOG_ERROR(
                "SERVER: SERVER_SHUTDOWN attempting to close server %s but server has already been closed",
                server_name);
        delete internaldata;
        internaldata = nullptr;
        return;
    }
    internaldata->server_should_close = true;
    while (!internaldata->server_closed);
    delete internaldata;
    internaldata = nullptr;
}

char *SOCKET_SERVER_name_to_server_name(SOCKET_SERVER_DATA *internaldata) {
    char * server_name = new char[107];
    memset(server_name, 0, 107);
    mempcpy(server_name, internaldata->socket_name+1, 107);
    return server_name;
}

long now_ms(void) {
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
}

long now_nanoseconds(void) {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux
    return ts.tv_nsec;
}

char *str_humanise_bytes(off_t bytes) {
    char *data = new char[1024];
    if (bytes > 1 << 30) {
        sprintf(data, "%u.%2.2u Gigabytes",
                (int) (bytes >> 30),
                (int) (bytes & ((1 << 30) - 1)) / 10737419);
    } else if (bytes > 1 << 20) {
        int x = bytes + 5243;  /* for rounding */
        sprintf(data, "%u.%2.2u Megabytes",
                x >> 20, ((x & ((1 << 20) - 1)) * 100) >> 20);
    } else if (bytes > 1 << 10) {
        int x = bytes + 5;  /* for rounding */
        sprintf(data, "%u.%2.2u Kilobytes",
                x >> 10, ((x & ((1 << 10) - 1)) * 100) >> 10);
    } else {
        sprintf(data, "%u bytes", (int) bytes);
    }
    return data;
}

bool SOCKET_READ(const char *TAG, ssize_t *ret, int &socket_data_fd, void *__buf, size_t __count,
                 int flags, ssize_t total) {
    LOG_INFO("%sreading message from fd %d", TAG, socket_data_fd);
    for (;;) { // implement blocking
        *ret = recv(socket_data_fd, static_cast<void *>(static_cast<uint8_t *>(__buf) + total),
                    __count - total, flags);
        if (*ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            else break;
        }
        break;
    }
    if (*ret == 0) {
        // if recv returns zero, that means the connection has been closed
        LOG_INFO("%sClient has closed the connection", TAG);
        return false;
    } else if (*ret < 0) {
        LOG_ERROR("%srecv: (errno: %2d) %s", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool
SOCKET_WRITE(const char *TAG, ssize_t *ret, int &socket_data_fd, const void *__buf, size_t __count,
             int flags, ssize_t total) {
    LOG_INFO("%swriting message to fd %d", TAG, socket_data_fd);
    for (;;) { // implement blocking
        *ret = send(socket_data_fd,
                    static_cast<const void *>(static_cast<const uint8_t *>(__buf) + total),
                    __count - total, flags);
        if (*ret < 0) if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
        break;
    }
    if (*ret < 0) {
        LOG_ERROR("%ssend: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_GET(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, void *__buf,
                size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while (total != __count) {
        ssize_t ret = 0;
        if (SOCKET_READ(TAG, &ret, socket_data_fd, __buf, __count, MSG_PEEK | MSG_DONTWAIT,
                        total)) {
            if (SOCKET_READ(TAG, &ret, socket_data_fd, __buf, __count, 0, total)) {
                total += ret;
                if (SERVER_LOG_TRANSFER_INFO)
                    LOG_INFO("%srecv %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO("%sObtained %s of data in %G milliseconds (Total obtained: %s of data)", TAG, n,
             end - start, t);
    delete[] n;
    delete[] t;
    return true;
}

bool
SOCKET_SEND(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, const void *__buf,
            size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while (total != __count) {
        ssize_t ret = 0;
        if (SOCKET_WRITE(TAG, &ret, socket_data_fd, __buf, __count, 0, total)) {
            total += ret;
            if (SERVER_LOG_TRANSFER_INFO)
                LOG_INFO("%ssend %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO("%sWrote %s of data in %G milliseconds (Total sent: %s of data)", TAG, n,
             end - start, t);
    delete[] n;
    delete[] t;
    return true;
}

// in recvmsg, between __pthread_enable_asynccancel() and __pthread_disable_asynccancel() a new memfd is created, which is racking up loads of memfd's, in which by the time i recieve an 8th fd i get too many sockets error, and i checked in /proc/PID/fd and its full of memfd fd links
// BUG: recvmsg creates a new memfd with each call
// this quickly racks up a ton of fd's and quickly produces out of sockets error
bool
SOCKET_READ_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, msghdr *__msg, int flags,
                    ssize_t total) {
    LOG_INFO("%sreading message from fd %d", TAG, socket_data_fd);
    for (;;) { // implement blocking
        if (total > 0) {
            struct msghdr msg = {0};
            memcpy(&msg, __msg, sizeof(*__msg));
            *ret = recvmsg(socket_data_fd, &msg, flags);
        } else *ret = recvmsg(socket_data_fd, __msg, flags);
        if (*ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            else break;
        }
        break;
    }
    if (*ret == 0) {
        // if recv returns zero, that means the connection has been closed
        LOG_INFO("%sClient has closed the connection", TAG);
        return false;
    } else if (*ret < 0) {
        LOG_ERROR("%srecvmsg: (errno: %2d) %s", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_WRITE_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, const msghdr *__msg,
                          int flags, ssize_t total) {
    LOG_INFO("%swriting message to fd %d", TAG, socket_data_fd);
    for (;;) { // implement blocking
        if (total > 0) {
            struct msghdr msg = {0};
            memcpy(&msg, __msg, sizeof(*__msg));
//            msg.msg_control = static_cast<void *>(static_cast<uint8_t *>(__msg->msg_control) +
//                                                  total);
//            msg.msg_controllen -= total;
            *ret = sendmsg(socket_data_fd, &msg, flags);
        } else *ret = sendmsg(socket_data_fd, __msg, flags);
        if (*ret < 0) if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
        break;
    }
    if (*ret < 0) {
        LOG_ERROR("%ssendmsg: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_GET_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                        msghdr *__msg, size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while (total != __count) {
        ssize_t ret = 0;
        if (SOCKET_READ_MESSAGE(TAG, &ret, socket_data_fd, __msg, MSG_PEEK | MSG_DONTWAIT, total)) {
            if (SOCKET_READ_MESSAGE(TAG, &ret, socket_data_fd, __msg, 0, total)) {
                total += ret;
                if (SERVER_LOG_TRANSFER_INFO)
                    LOG_INFO("%srecvmsg %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO("%sObtained %s of data in %G milliseconds (Total obtained: %s of data)", TAG, n,
             end - start, t);
    delete[] n;
    delete[] t;
    return true;
}

bool SOCKET_SEND_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                         const msghdr *__msg, size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_WRITE_MESSAGE(TAG, &ret, socket_data_fd, __msg, 0, total)) {
            total += ret;
            if (SERVER_LOG_TRANSFER_INFO)
                LOG_INFO("%ssendmsg %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO("%sWrote %s of data in %G milliseconds (Total sent: %s of data)", TAG, n,
             end - start, t);
    delete[] n;
    delete[] t;
    return true;
}

bool SOCKET_SEND_SERIAL(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                        serializer &S, char *server_name) {
    S.construct();
    SOCKET_SEND(s, TAG, socket_data_fd, &S.stream.data_len, sizeof(size_t), server_name);
    SOCKET_SEND(s, TAG, socket_data_fd, S.stream.data, S.stream.data_len, server_name);
    S.stream.deallocate();
    return true;
}

bool
SOCKET_GET_SERIAL(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, serializer &S,
                  char *server_name) {
    if (SOCKET_GET(s, TAG, socket_data_fd, &S.stream.data_len, sizeof(size_t), server_name)) {
        S.stream.allocate(S.stream.data_len);
        if (SOCKET_GET(s, TAG, socket_data_fd, S.stream.data, S.stream.data_len, server_name)) {
            S.deconstruct();
            return true;
        } else S.stream.deallocate();
    }
    return false;
}

void SOCKET_SEND_FD(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, int &fd,
                    char *server_name) {
    if (fd <= 0) return;
    struct iovec	iov[1];
    struct msghdr	msg;
    char			buf[2];	/* send_fd()/recv_fd() 2-byte protocol */

    iov[0].iov_base = buf;
    iov[0].iov_len  = 2;
    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;
    msg.msg_name    = NULL;
    msg.msg_namelen = 0;

    struct cmsghdr	*cmptr = static_cast<cmsghdr *>(malloc(CMSG_LEN(sizeof(int))));
    cmptr->cmsg_level  = SOL_SOCKET;
    cmptr->cmsg_type   = SCM_RIGHTS;
    cmptr->cmsg_len    = CMSG_LEN(sizeof(int));
    msg.msg_control    = cmptr;
    msg.msg_controllen = CMSG_LEN(sizeof(int));
    *(int *)CMSG_DATA(cmptr) = fd;		/* the fd to pass */
    buf[1] = 0;		/* zero status means OK */
    buf[0] = 0;			/* null byte flag to recv_fd() */
    sendmsg(socket_data_fd, &msg, 0);
    free(cmptr);
}

// TODO: error checking
// taken from apue.3e/lib/sendfd.c and apue.3e/lib/recvfd.c
// adapted to socket server implementation

void SOCKET_GET_FD(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, int &fd,
                   char *server_name)  // receive fd from socket
{
    int				nr, status;
    char			*ptr;
    char			buf[4096];
    struct iovec	iov[1];
    struct msghdr	msg;

    struct cmsghdr	*cmptr = static_cast<cmsghdr *>(malloc(CMSG_LEN(sizeof(int))));

    status = -1;
    for ( ; ; ) {
        iov[0].iov_base = buf;
        iov[0].iov_len  = sizeof(buf);
        msg.msg_iov     = iov;
        msg.msg_iovlen  = 1;
        msg.msg_name    = NULL;
        msg.msg_namelen = 0;
        msg.msg_control    = cmptr;
        msg.msg_controllen = CMSG_LEN(sizeof(int));
        if ((nr = recvmsg(socket_data_fd, &msg, 0)) < 0) {
            LOG_ALWAYS_FATAL("recvmsg error: %s", strerror(errno));
            return;
        } else if (nr == 0) {
            LOG_ALWAYS_FATAL("connection closed by server");
            return;
        }

        /*
         * See if this is the final data with null & status.  Null
         * is next to last byte of buffer; status byte is last byte.
         * Zero status means there is a file descriptor to receive.
         */
        for (ptr = buf; ptr < &buf[nr]; ) {
            if (*ptr++ == 0) {
                if (ptr != &buf[nr-1]) {
                    LOG_ERROR("message format error");
                    return;
                }
                status = *ptr & 0xFF;	/* prevent sign extension */
                if (status == 0) {
                    if (msg.msg_controllen == CMSG_LEN(sizeof(int)))
                    LOG_ERROR("status = 0");
                    fd = *(int *)CMSG_DATA(cmptr);
                    LOG_ERROR("received fd: %d", fd);
                    free(cmptr);
                    return;
                } else {
                    fd = -status;
                    LOG_ERROR("status error: %d", fd);
                    free(cmptr);
                    return;
                }
                nr -= 2;
            }
        }
        if (status >= 0) {    /* final data has arrived */
            free(cmptr);
            return;    /* descriptor, or -status */
        }
    }
}

bool SOCKET_CLOSE(const char *TAG, int &socket_fd) {
    int ret;
    for (;;) { // implement blocking
        ret = close(socket_fd);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            if (errno == EBADF) return true;
        }
        break;
    }
    if (ret < 0) {
        LOG_ERROR("%sclose: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

SOCKET_SERVER *SERVER_get(size_t id) {
    SOCKET_SERVER * S = SERVER_KERNEL.table->table[id]->resource.get<SOCKET_SERVER*>();
    assert(S != nullptr);
    return S;
}

char *SERVER_allocate_new_server(void *(*SERVER_MAIN)(void *), size_t &id) {
    Object *o = SERVER_KERNEL.newObject(0, ObjectFlagAutoDeallocateResource, new SOCKET_SERVER);
    id = SERVER_KERNEL.table->findObject(o);
    o->resource.get<SOCKET_SERVER*>()->set_name(std::to_string(id).c_str());
    if (SERVER_MAIN != nullptr) o->resource.get<SOCKET_SERVER*>()->startServer(SERVER_MAIN);
    return o->resource.get<SOCKET_SERVER*>()->server_name;
}

char *SERVER_allocate_new_server(size_t &id) {
    return SERVER_allocate_new_server(nullptr, id);
}

void SERVER_deallocate_server(size_t &id) {
    Object * o = SERVER_KERNEL.table->table[id];
    SOCKET_SERVER * x = o->resource.get<SOCKET_SERVER*>();
    x->shutdownServer();
    SERVER_KERNEL.table->DELETE(id);
}

void *SERVER_START_REPLY_MANUALLY(void *na) {
    assert(na != nullptr);
    SOCKET_SERVER *server = static_cast<SOCKET_SERVER *>(na);
    server->socket_create(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    server->socket_bind(AF_UNIX);
    server->socket_listen(1);
    while (!server->internaldata->server_should_close);
    server->socket_close();
    return NULL;
}

int SOCKET_SERVER::log_info(const char *fmt, ...) {
    va_list va1, va2;
    va_start(va1, fmt);
    // be safe, allocate plus last char plus null terminator
    int l = vsnprintf(NULL, 0, fmt, va1)+2;
    va_end(va1);
    char * s = static_cast<char*>(malloc(l));
    memset(s, 0, l);
    va_start(va2, fmt);
    vsnprintf(s, l, fmt, va2);
    va_end(va2);
    int len = LOG_INFO("%s: %s%s",
        LOG_TAG_SERVER, (TAG == nullptr || TAG == NULL) ? "TAG NOT PROVIDED: " : TAG, s
    );
    free(s);
    return len;
}

void SOCKET_SERVER::startServer(void *(*SERVER_MAIN)(void *)) {
    if (internaldata != nullptr) {
        LOG_ERROR("%sattempting to start server while running", TAG);
        return;
    }
    internaldata = new SOCKET_SERVER_DATA;
    internaldata->server_CAN_CONNECT = false;
    internaldata->server_should_close = false;
    internaldata->server_closed = false;
    memset(internaldata->socket_name, 0, 108);
    // NDK needs abstract namespace by leading with '\0'
    internaldata->socket_name[0] = '\0';
    memcpy(&internaldata->socket_name[1], server_name, 107);
    pthread_create(&server_thread, NULL, SERVER_MAIN, this);
    while (!internaldata->server_CAN_CONNECT) {}
}

void SOCKET_SERVER::shutdownServer() {
    if (internaldata == nullptr) {
        LOG_ERROR(
                "%sshutdownServer attempting to close server but server has already been closed",
                TAG);
        return;
    }
    if (internaldata->server_closed) {
        LOG_ERROR(
                "%sshutdownServer internaldata->server_closed attempting to close serverbut server has already been closed",
                TAG);
        delete internaldata;
        internaldata = nullptr;
    } else SERVER_SHUTDOWN(server_name, internaldata);
}

void SOCKET_SERVER::set_name(const char *name) {
    if (internaldata != nullptr) {
        LOG_ERROR("%sattempting to change server name while server is running",
                  TAG);
        return;
    }
    memset(&server_name, 0, 107);
    if (name == nullptr || name == NULL) {
        // build default TAG
        if (TAG_DEALLOCATE) free(TAG);
        TAG = strdup(
                std::string(std::string("SERVER: ") + (default_server_name) + " : ").c_str());
        TAG_DEALLOCATE = true;
        LOG_ERROR("%sname was not supplied, conflicts are likely to happen", TAG);
        if (strlen(default_server_name) > 107) {
            LOG_ERROR(
                    "%sdefault name is longer than 107 characters, truncating, conflicts may happen",
                    TAG
            );
            memcpy(server_name, default_server_name, 107);
        } else memcpy(server_name, default_server_name, default_server_name_length);
    } else {
        LOG_INFO(
                "%ssetting name to %s",
                TAG, name
        );
        if (strlen(name) > 107) {
            LOG_ERROR(
                    "%sname is longer than 107 characters, truncating, conflicts may happen",
                    TAG
            );
            memcpy(server_name, name, 107);
        } else memcpy(server_name, name, strlen(name));
        // build new TAG
        if (TAG_DEALLOCATE) free(TAG);
        TAG = strdup(std::string(std::string("SERVER: ") + server_name + " : ").c_str());
        TAG_DEALLOCATE = true;
        LOG_INFO(
                "%sset name to %s",
                TAG, server_name
        );
    }
}

void SOCKET_SERVER::unset_name() {
    set_name(nullptr);
}

SOCKET_SERVER::SOCKET_SERVER() {
    set_name(nullptr);
}

SOCKET_SERVER::SOCKET_SERVER(const char *name) {
    set_name(name);
}

SOCKET_SERVER::~SOCKET_SERVER() {
    if (TAG_DEALLOCATE) free(TAG);
    TAG = nullptr;
}

bool
SOCKET_SERVER::socket_create(int &socket_fd, int __af, int __type, int __protocol) {
    socket_fd = socket(__af, __type, __protocol);
    if (socket_fd < 0) {
        LOG_ERROR("%ssocket: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    LOG_INFO("%sSocket made", TAG);
    return true;
}

bool SOCKET_SERVER::socket_bind(int &socket_fd, int __af) {
    memset(&internaldata->server_addr, 0, sizeof(struct sockaddr_un));
    internaldata->server_addr.sun_family = __af;
    memcpy(internaldata->server_addr.sun_path, internaldata->socket_name, 108);
    LOG_INFO("%sbinding socket fd %d to name %s", TAG, socket_fd,
             &internaldata->server_addr.sun_path[1]);
    if (bind(socket_fd, (const struct sockaddr *) &internaldata->server_addr,
             sizeof(struct sockaddr_un)) < 0) {
        LOG_ERROR("%sbind: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    LOG_INFO("%sBind made", TAG);
    return true;
}

bool SOCKET_SERVER::socket_listen(int &socket_fd, int pending_connection_queue_size) {
    if (listen(socket_fd, pending_connection_queue_size) < 0) {
        LOG_ERROR("%slisten: %s", TAG, strerror(errno));
        exit(EXIT_FAILURE);
    }
    LOG_INFO("%sSocket listening for packages", TAG);
    internaldata->server_CAN_CONNECT = true;
    return true;
}

bool SOCKET_SERVER::socket_accept(int &socket_fd, int &socket_data_fd) {
    for (;;) {
        if (internaldata == nullptr) return false;
        if (internaldata->server_should_close) return false;
        socket_data_fd = accept(socket_fd, NULL, NULL);
        if (socket_data_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            else break;
        }
        break;
    }
    if (socket_data_fd < 0) {
        LOG_ERROR("%saccept: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_SERVER::socket_accept_non_blocking(int &socket_fd, int &socket_data_fd) {
    if (internaldata->server_should_close) return false;
    socket_data_fd = accept(socket_fd, NULL, NULL);
    if (socket_data_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return false;
        LOG_ERROR("%saccept: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_SERVER::socket_unaccept(int &socket_data_fd) {
    return SOCKET_CLOSE(TAG, socket_data_fd);
}

bool SOCKET_SERVER::connection_is_alive(int &socket_data_fd) {
    if (internaldata == nullptr) return false;
    if (internaldata->server_should_close) return false;
    if (internaldata->server_closed) return false;
    int ret;
    LOG_INFO("%schecking if socket fd %d is alive", TAG, socket_data_fd);
    for (;;) { // implement blocking
        // users may use this in sockets that send data
        // do not remove any data that may be queued
        void *nothing = malloc(1);
        ret = recv(socket_data_fd, nothing, 1, MSG_PEEK | MSG_DONTWAIT);
        free(nothing);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            else break;
        }
        break;
    }
    if (ret == 0) {
        // if recv returns zero, that means the connection has been closed
        LOG_INFO("%sClient has closed the connection", TAG);
        return false;
    }

    LOG_INFO("%ssocket fd %d is alive", TAG, socket_data_fd);

    if (ret < 0) {
        LOG_ERROR("%srecv: (errno: %2d) %s", TAG, errno, strerror(errno));
        return true;
    }
    return true;
}

bool SOCKET_SERVER::connection_wait_until_disconnect(int &socket_data_fd) {
    int ret;
    for (;;) { // implement blocking
        // users may use this in sockets that send data
        // do not remove any data that may be queued
        void *nothing = malloc(1);
        ret = recv(socket_data_fd, nothing, 1, MSG_PEEK);
        free(nothing);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
            else break;
        }
        break;
    }
    if (ret == 0) {
        // if recv returns zero, that means the connection has been closed
        LOG_INFO("%sClient has closed the connection", TAG);
        return false;
    } else if (ret < 0) {
        LOG_ERROR("%srecv: (errno: %2d) %s", TAG, errno, strerror(errno));
        return true;
    }
    return true;
}

void SOCKET_SERVER::socket_close(int &socket_fd, int &socket_data_fd) {
    if (SOCKET_CLOSE(TAG, socket_data_fd) && SOCKET_CLOSE(TAG, socket_fd))
        internaldata->server_closed = true;
}

bool SOCKET_SERVER::socket_create(int __af, int __type, int __protocol) {
    return socket_create(socket_fd, __af, __type, __protocol);
}

bool SOCKET_SERVER::socket_bind(int __af) { return socket_bind(socket_fd, __af); }

bool SOCKET_SERVER::socket_listen(int pending_connection_queue_size) {
    return socket_listen(socket_fd, pending_connection_queue_size);
}

bool SOCKET_SERVER::socket_accept() { return socket_accept(socket_fd, socket_data_fd); }

bool SOCKET_SERVER::socket_accept_non_blocking() {
    return socket_accept_non_blocking(socket_fd, socket_data_fd);
}

bool SOCKET_SERVER::socket_unaccept() { return socket_unaccept(socket_data_fd); }

bool SOCKET_SERVER::connection_is_alive() { return connection_is_alive(socket_data_fd); }

bool SOCKET_SERVER::connection_wait_until_disconnect() {
    return connection_wait_until_disconnect(socket_data_fd);
}

void SOCKET_SERVER::socket_close() { socket_close(socket_fd, socket_data_fd); }

bool SOCKET_SERVER::socket_put_serial(serializer &S) {
    return SOCKET_SEND_SERIAL(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                              server_name);
}

bool SOCKET_SERVER::socket_get_serial(serializer &S) {
    return SOCKET_GET_SERIAL(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                             server_name);
}

void SOCKET_SERVER::socket_put_fd(int &fd) {
    SOCKET_SEND_FD(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, server_name);
}

void SOCKET_SERVER::socket_get_fd(int &fd) {
    SOCKET_GET_FD(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, server_name);
}

int SOCKET_CLIENT::log_info(const char *fmt, ...) {
    va_list va1, va2;
    va_start(va1, fmt);
    // be safe, allocate plus last char plus null terminator
    int l = vsnprintf(NULL, 0, fmt, va1)+2;
    va_end(va1);
    char * s = static_cast<char*>(malloc(l));
    memset(s, 0, l);
    va_start(va2, fmt);
    vsnprintf(s, l, fmt, va2);
    va_end(va2);
    int len = LOG_INFO("%s: %s%s",
        LOG_TAG_SERVER, (TAG == nullptr || TAG == NULL) ? "TAG NOT PROVIDED: " : TAG, s
    );
    free(s);
    return len;
}

void SOCKET_CLIENT::set_name(const char *name) {
    char socket_name[108]; // 108 sun_path length max
    memset(&socket_name, 0, 108);
    socket_name[0] = '\0';
    if (name == nullptr || name == NULL) {
        // build default TAG
        if (TAG_DEALLOCATE) free(TAG);
        TAG = strdup(std::string(std::string("CLIENT: ") + (default_client_name) + " : ").c_str());
        TAG_DEALLOCATE = true;
        if (strlen(default_client_name) > 107) {
            LOG_ERROR(
                    "%sdefault name is longer than 107 characters, truncating, conflicts may happen",
                    TAG
            );
            memcpy(&socket_name[1], default_client_name, 107);
        } else memcpy(&socket_name[1], default_client_name, default_client_name_length);
    } else {
        LOG_INFO(
                "%ssetting name to %s",
                TAG, name
        );
        if (strlen(name) > 107) {
            LOG_ERROR(
                    "%sname is longer than 107 characters, truncating, conflicts may happen",
                    TAG
            );
            memcpy(&socket_name[1], name, 107);
        } else memcpy(&socket_name[1], name, strlen(name));
        // build new TAG
        if (TAG_DEALLOCATE) free(TAG);
        TAG = strdup(std::string(std::string("CLIENT: ") + &socket_name[1] + " : ").c_str());
        TAG_DEALLOCATE = true;
        LOG_INFO(
                "%sset name to %s",
                TAG, &socket_name[1]
        );
    }
    // clear for safety
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX; // Unix Domain instead of AF_INET IP domain
    memcpy(server_addr.sun_path, socket_name, 108);
}

void SOCKET_CLIENT::unset_name() {
    set_name(nullptr);
}

SOCKET_CLIENT::SOCKET_CLIENT() {
    set_name(nullptr);
}

SOCKET_CLIENT::SOCKET_CLIENT(const char *name) {
    set_name(name);
}

SOCKET_CLIENT::~SOCKET_CLIENT() {
    free(TAG);
    TAG = nullptr;
}

bool SOCKET_CLIENT::socket_put_serial(serializer &S) {
    return SOCKET_SEND_SERIAL(DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                              &server_addr.sun_path[1]);
}

bool SOCKET_CLIENT::socket_get_serial(serializer &S) {
    return SOCKET_GET_SERIAL(DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                             &server_addr.sun_path[1]);
}

void SOCKET_CLIENT::socket_put_fd(int &fd) {
    SOCKET_SEND_FD(DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, &server_addr.sun_path[1]);
}

void SOCKET_CLIENT::socket_get_fd(int &fd) {
    SOCKET_GET_FD(DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, &server_addr.sun_path[1]);
}

bool SOCKET_CLIENT::connect_to_server() {
    socket_data_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_data_fd < 0) {
        LOG_ERROR("%ssocket: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    LOG_INFO("%sconnecting to server", TAG);
    ssize_t ret = 0;
    for (;;) {
        ret = connect(socket_data_fd, (const struct sockaddr *) &server_addr,
                      sizeof(struct sockaddr_un));
        if (ret >= 0) break;
    }
    if (ret < 0) {
        LOG_ERROR("%sconnect: %d (%s)", TAG, errno, strerror(errno));
        return false;
    }
    LOG_INFO("%sconnected to server", TAG);
    return true;
}

bool SOCKET_CLIENT::disconnect_from_server() {
    LOG_INFO("%sclosing connection to server", TAG);
    assert(SOCKET_CLOSE(TAG, socket_data_fd));
    LOG_INFO("%sclosed connection to server", TAG);
    if (SERVER_LOG_TRANSFER_INFO) LOG_INFO("%sReturning response", TAG);
    return true;
}
