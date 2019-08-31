//
// Created by konek on 8/13/2019.
//
#include "serializer.h"
#include "WINAPI/SDK/include/Windows/Kernel/WindowsAPIKernel.h"

#ifndef GLNE_SERVER_CORE_H
#define GLNE_SERVER_CORE_H

bool SERVER_LOG_TRANSFER_INFO = false;

class SOCKET_DATA_TRANSFER_INFO {
    public:
        size_t total_wrote = 0;
        size_t total_sent = 0;
};

class SOCKET_SERVER_DATA {
    public:
        bool server_CAN_CONNECT = false;
        bool server_should_close = false;
        bool server_closed = false;
        struct sockaddr_un server_addr = {0};
        char socket_name[108] = {0}; // 108 sun_path length max
        SOCKET_DATA_TRANSFER_INFO DATA_TRANSFER_INFO;
};

class SERVER_MESSAGES {
    public:
        struct SERVER_MESSAGE_TYPE {
            int ping = 0;
            int texture = 1;
            int mirror = 2;
        } SERVER_MESSAGE_TYPE;
        struct SERVER_MESSAGE_RESPONSE {
            int OK = 0;
            int FAIL = 1;
        } SERVER_MESSAGE_RESPONSE;
} SERVER_MESSAGES;

void SERVER_SHUTDOWN(char * server_name, SOCKET_SERVER_DATA * & internaldata) {
    if (internaldata->server_closed) {
        LOG_ERROR_SERVER(
            "SERVER: SERVER_SHUTDOWN attempting to close server %s but server has already been closed\n",
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

char *SOCKET_SERVER_name_to_server_name(SOCKET_SERVER_DATA * internaldata) {
    char * server_name = new char[107];
    memset(server_name, 0, 107);
    mempcpy(server_name, internaldata->socket_name+1, 107);
    return server_name;
}

/* Return current time in milliseconds */
double now_ms(void) {
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
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

// returns false if an error has occurred otherwise true
bool SOCKET_READ(const char *TAG, ssize_t *ret, int &socket_data_fd, void *__buf, size_t __count,
                 int flags, ssize_t total) {
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
        LOG_INFO_SERVER("%sClient has closed the connection\n", TAG);
        return false;
    } else if (*ret < 0) {
        LOG_ERROR_SERVER("%srecv: (errno: %2d) %s\n", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

// returns false if an error has occurred otherwise true
bool
SOCKET_WRITE(const char *TAG, ssize_t *ret, int &socket_data_fd, const void *__buf, size_t __count,
             int flags, ssize_t total) {
    for (;;) { // implement blocking
        LOG_INFO_SERVER("sending message to fd %d", socket_data_fd);
        *ret = send(socket_data_fd,
                    static_cast<const void *>(static_cast<const uint8_t *>(__buf) + total),
                    __count - total, flags);
        if (*ret < 0) if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
        break;
    }
    if (*ret < 0) {
        LOG_ERROR_SERVER("%ssend: %d (%s)\n", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool
SOCKET_GET(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, void *__buf,
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
                    LOG_INFO_SERVER("%srecv %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO_SERVER("%sObtained %s of data in %G milliseconds (Total obtained: %s of data)", TAG, n,
                    end - start, t);
    delete n;
    delete t;
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
                LOG_INFO_SERVER("%ssend %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO_SERVER("%sWrote %s of data in %G milliseconds (Total sent: %s of data)", TAG, n,
                    end - start, t);
    delete n;
    delete t;
    return true;
}

// returns false if an error has occurred otherwise true
bool SOCKET_READ_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, msghdr *__msg,
                         int flags, ssize_t total) {
    for (;;) { // implement blocking
        if (total > 0) {
            struct msghdr msg = {0};
            memcpy(&msg, __msg, sizeof(*__msg));
//            msg.msg_control = static_cast<void *>(static_cast<uint8_t *>(__msg->msg_control) +
//                                                  total);
//            msg.msg_controllen -= total;
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
        LOG_INFO_SERVER("%sClient has closed the connection\n", TAG);
        return false;
    } else if (*ret < 0) {
        LOG_ERROR_SERVER("%srecvmsg: (errno: %2d) %s\n", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

// returns false if an error has occurred otherwise true
bool SOCKET_WRITE_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, const msghdr *__msg,
                          int flags, ssize_t total) {
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
        LOG_ERROR_SERVER("%ssendmsg: %d (%s)\n", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

bool
SOCKET_GET_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                   msghdr *__msg,
                   size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while (total != __count) {
        ssize_t ret = 0;
        if (SOCKET_READ_MESSAGE(TAG, &ret, socket_data_fd, __msg, MSG_PEEK | MSG_DONTWAIT, total)) {
            if (SOCKET_READ_MESSAGE(TAG, &ret, socket_data_fd, __msg, 0, total)) {
                total += ret;
                if (SERVER_LOG_TRANSFER_INFO)
                    LOG_INFO_SERVER("%srecvmsg %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO_SERVER("%sObtained %s of data in %G milliseconds (Total obtained: %s of data)", TAG, n,
                    end - start, t);
    delete n;
    delete t;
    return true;
}

bool
SOCKET_SEND_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                    const msghdr *__msg,
                    size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    double start = now_ms();
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_WRITE_MESSAGE(TAG, &ret, socket_data_fd, __msg, 0, total)) {
            total += ret;
            if (SERVER_LOG_TRANSFER_INFO)
                LOG_INFO_SERVER("%ssendmsg %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    double end = now_ms();
    s.total_wrote += __count;
    char *n = str_humanise_bytes(__count);
    char *t = str_humanise_bytes(s.total_wrote);
    LOG_INFO_SERVER("%sWrote %s of data in %G milliseconds (Total sent: %s of data)", TAG, n,
                    end - start, t);
    delete n;
    delete t;
    return true;
}

bool
SOCKET_SEND_SERIAL(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                   serializer &S,
                   char *server_name) {
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
                    char *server_name)  // send fd by socket
{
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(fd))];
    memset(buf, '\0', sizeof(buf));

    /* On Mac OS X, the struct iovec is needed, even if it points to minimal data */
    struct iovec io;
    io.iov_base = (void *) "";
    io.iov_len = 1;

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

    memmove(CMSG_DATA(cmsg), &fd, sizeof(fd));

    msg.msg_controllen = CMSG_SPACE(sizeof(fd));

    serializer m;
    m.add<size_t>(sizeof(msg));
    SOCKET_SEND_SERIAL(s, TAG, socket_data_fd, m, server_name);
    SOCKET_SEND_MESSAGE(s, TAG, socket_data_fd, &msg, sizeof(msg), server_name);
}

void SOCKET_GET_FD(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, int &fd,
                   char *server_name)  // receive fd from socket
{
    struct msghdr msg = {0};

    /* On Mac OS X, the struct iovec is needed, even if it points to minimal data */
    char m_buffer[1];
    struct iovec io = {.iov_base = m_buffer, .iov_len = sizeof(m_buffer)};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    char c_buffer[256];
    msg.msg_control = c_buffer;
    msg.msg_controllen = sizeof(c_buffer);
    serializer m;
    SOCKET_GET_SERIAL(s, TAG, socket_data_fd, m, server_name);
    size_t __count;
    m.get<size_t>(&__count);
    SOCKET_GET_MESSAGE(s, TAG, socket_data_fd, &msg, __count, server_name);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);

    LOG_INFO_SERVER("About to extract fd\n");
    memmove(&fd, CMSG_DATA(cmsg), sizeof(fd));
    LOG_INFO_SERVER("Extracted fd %d\n", fd);
}

bool SOCKET_CLOSE(const char *TAG, int & socket_fd) {
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
        LOG_ERROR_SERVER("%sclose: %d (%s)\n", TAG, errno, strerror(errno));
        return false;
    }
    return true;
}

class SOCKET_SERVER {
    private:
        pthread_t server_thread = 0;
        const char *default_server_name = "SOCKET_SERVER";
        const size_t default_server_name_length = strlen(default_server_name);

    public:
        char *TAG = nullptr;
        SOCKET_SERVER_DATA *internaldata = nullptr;
        int socket_fd = 0;
        int socket_data_fd = 0;

        void startServer(void *(*SERVER_MAIN)(void *)) {
            if (internaldata != nullptr) {
                LOG_ERROR_SERVER("%sattempting to start server while running\n", TAG);
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

        void shutdownServer() {
            if (internaldata == nullptr) {
                LOG_ERROR_SERVER(
                    "%sshutdownServer attempting to close server but server has already been closed\n",
                    TAG);
                return;
            }
            if (internaldata->server_closed) {
                LOG_ERROR_SERVER(
                    "%sshutdownServer internaldata->server_closed attempting to close serverbut server has already been closed\n",
                    TAG);
                delete internaldata;
                internaldata = nullptr;
            } else SERVER_SHUTDOWN(server_name, internaldata);
        }

        void set_name(const char *name) {
            if (internaldata != nullptr) {
                LOG_ERROR_SERVER("%sattempting to change server name while server is running\n",
                                 TAG);
                return;
            }
            memset(&server_name, 0, 107);
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(
                    std::string(std::string("SERVER: ") + (default_server_name) + " : ").c_str());
                LOG_ERROR_SERVER("%sname was not supplied, conflicts are likely to happen\n", TAG);
                if (strlen(default_server_name) > 107) {
                    LOG_ERROR_SERVER("%sdefault name is longer than 107 characters, truncating\n",
                                     TAG);
                    memcpy(server_name, default_server_name, 107);
                } else memcpy(server_name, default_server_name, default_server_name_length);
            } else {
                if (strlen(server_name) > 107) {
                    LOG_ERROR_SERVER(
                        "%sname is longer than 107 characters, truncating, conflicts may happen\n",
                        TAG);
                    memcpy(server_name, name, 107);
                } else memcpy(server_name, name, strlen(name));
                // build new TAG
                TAG = strdup(std::string(std::string("SERVER: ") + server_name + " : ").c_str());
            }
        }

        void unset_name() {
            set_name(nullptr);
        }

        SOCKET_SERVER() {
            set_name(nullptr);
        }

        SOCKET_SERVER(const char *name) {
            set_name(name);
        }

        ~SOCKET_SERVER() {
            if (TAG != nullptr) {
                free(TAG);
                TAG = nullptr;
            }
        }

        bool socket_create(int &socket_fd, __kernel_sa_family_t __af, int __type, int __protocol) {
            socket_fd = socket(__af, __type, __protocol);
            if (socket_fd < 0) {
                LOG_ERROR_SERVER("%ssocket: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            LOG_INFO_SERVER("%sSocket made\n", TAG);
            return true;
        }

        bool socket_bind(int &socket_fd, __kernel_sa_family_t __af) {
            memset(&internaldata->server_addr, 0, sizeof(struct sockaddr_un));
            internaldata->server_addr.sun_family = __af;
            memcpy(internaldata->server_addr.sun_path, internaldata->socket_name, 108);
            LOG_INFO_SERVER("%sbinding socket fd %d to name %s\n", TAG, socket_fd,
                            &internaldata->server_addr.sun_path[1]);
            if (bind(socket_fd, (const struct sockaddr *) &internaldata->server_addr,
                     sizeof(struct sockaddr_un)) < 0) {
                LOG_ERROR_SERVER("%sbind: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            LOG_INFO_SERVER("%sBind made\n", TAG);
            return true;
        }

        //       The pending_connection_queue_size argument defines the maximum length to which the
        //       queue of pending connections for socket_fd may grow.
        //       If a connection request arrives when the queue is full, the client may receive an
        //       error with an indication of ECONNREFUSED or, if the underlying protocol supports
        //       retransmission, the request may be ignored so that a later reattempt at connection
        //       succeeds.
        //
        // sets internaldata->server_CAN_CONNECT to true on success
        bool socket_listen(int &socket_fd, int pending_connection_queue_size) {
            if (listen(socket_fd, pending_connection_queue_size) < 0) {
                LOG_ERROR_SERVER("%slisten: %s", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            LOG_INFO_SERVER("%sSocket listening for packages\n", TAG);
            internaldata->server_CAN_CONNECT = true;
            return true;
        }

        // returns false if internaldata->server_should_close is true or if an error has occured
        // otherwise returns true upon a successful accept attempt
        bool socket_accept(int &socket_fd, int &socket_data_fd) {
            for (;;) {
                if (internaldata->server_should_close) return false;
                socket_data_fd = accept(socket_fd, NULL, NULL);
                if (socket_data_fd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
                    else break;
                }
                break;
            }
            if (socket_data_fd < 0) {
                LOG_ERROR_SERVER("%saccept: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            return true;
        }

        // returns false if internaldata->server_should_close is true, if an error has occured,
        // or upon a failure to connect
        // otherwise returns true upon a successful accept attempt
        bool socket_accept_non_blocking(int &socket_fd, int &socket_data_fd) {
            if (internaldata->server_should_close) return false;
            socket_data_fd = accept(socket_fd, NULL, NULL);
            if (socket_data_fd < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return false;
                LOG_ERROR_SERVER("%saccept: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            return true;
        }

        bool socket_unaccept(int &socket_data_fd) {
            return SOCKET_CLOSE(TAG, socket_data_fd);
        }

        bool connection_is_alive(int &socket_data_fd) {
            int ret;
            for (;;) { // implement blocking
                // users may use this in sockets that send data
                // do not remove any data that may be queued
                void *nothing = malloc(1);
                ret = recv(socket_data_fd, nothing, 1, MSG_PEEK | MSG_DONTWAIT);
                free(nothing);
                if (ret < 0) {
                    if (errno == EINTR) continue;
                    else break;
                }
                break;
            }
            if (ret == 0) {
                // if recv returns zero, that means the connection has been closed
                LOG_INFO_SERVER("%sClient has closed the connection\n", TAG);
                return false;
            } else if (ret < 0) {
                LOG_ERROR_SERVER("%srecv: (errno: %2d) %s\n", TAG, errno, strerror(errno));
                return true;
            }
            return true;
        }

        bool connection_wait_until_disconnect(int &socket_data_fd) {
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
                LOG_INFO_SERVER("%sClient has closed the connection\n", TAG);
                return false;
            } else if (ret < 0) {
                LOG_ERROR_SERVER("%srecv: (errno: %2d) %s\n", TAG, errno, strerror(errno));
                return true;
            }
            return true;
        }

        void socket_close(int &socket_fd, int &socket_data_fd) {
            if (SOCKET_CLOSE(TAG, socket_data_fd) && SOCKET_CLOSE(TAG, socket_fd))
                internaldata->server_closed = true;
        }

        bool socket_create(__kernel_sa_family_t __af, int __type, int __protocol) {
            return socket_create(socket_fd, __af, __type, __protocol);
        }

        bool socket_bind(__kernel_sa_family_t __af) { return socket_bind(socket_fd, __af); }

        //       The pending_connection_queue_size argument defines the maximum length to which the
        //       queue of pending connections for socket_fd may grow.
        //       If a connection request arrives when the queue is full, the client may receive an
        //       error with an indication of ECONNREFUSED or, if the underlying protocol supports
        //       retransmission, the request may be ignored so that a later reattempt at connection
        //       succeeds.
        //
        // sets internaldata->server_CAN_CONNECT to true on success
        bool socket_listen(int pending_connection_queue_size) {
            return socket_listen(socket_fd, pending_connection_queue_size);
        }

        // returns false if internaldata->server_should_close is true or if an error has occured
        // otherwise returns true upon a successful accept attempt
        bool socket_accept() { return socket_accept(socket_fd, socket_data_fd); }

        // returns false if internaldata->server_should_close is true, if an error has occured,
        // or upon a failure to connect
        // otherwise returns true upon a successful accept attempt
        bool socket_accept_non_blocking() {
            return socket_accept_non_blocking(socket_fd, socket_data_fd);
        }

        bool socket_unaccept() { return socket_unaccept(socket_data_fd); }

        bool connection_is_alive() { return connection_is_alive(socket_data_fd); }

        bool connection_wait_until_disconnect() {
            return connection_wait_until_disconnect(socket_data_fd);
        }

        void socket_close() { socket_close(socket_fd, socket_data_fd); }

        bool socket_put_serial(serializer &S) {
            return SOCKET_SEND_SERIAL(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                                      server_name);
        }

        bool socket_get_serial(serializer &S) {
            return SOCKET_GET_SERIAL(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                                     server_name);
        }

        void socket_put_fd(int &fd) {
            SOCKET_SEND_FD(internaldata->DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, server_name);
        }

        char server_name[107];
};

Kernel SERVER_KERNEL;

char *SERVER_allocate_new_server(void *(*SERVER_MAIN)(void *), size_t &id) {
    Object *o = SERVER_KERNEL.newObject(0, 0, new SOCKET_SERVER);
    id = SERVER_KERNEL.table->findObject(o);
    static_cast<SOCKET_SERVER *>(o->resource)->set_name(std::to_string(id).c_str());
    if (SERVER_MAIN != nullptr) static_cast<SOCKET_SERVER *>(o->resource)->startServer(SERVER_MAIN);
    return static_cast<SOCKET_SERVER *>(o->resource)->server_name;
}

char *SERVER_allocate_new_server(size_t &id) { return SERVER_allocate_new_server(nullptr, id); }


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

SOCKET_SERVER *SERVER_get(size_t id) {
    return static_cast<SOCKET_SERVER *>(SERVER_KERNEL.table->table[id]->resource);
}

class SOCKET_CLIENT {
    public:
        int socket_data_fd = 0;
        struct sockaddr_un server_addr = {0};
        const char * default_client_name = "SOCKET_SERVER";
        const size_t default_client_name_length = strlen(default_client_name);
        char * TAG = nullptr;
        SOCKET_DATA_TRANSFER_INFO DATA_TRANSFER_INFO;
        void set_name(const char * name) {
            char socket_name[108]; // 108 sun_path length max
            memset(&socket_name, 0, 108);
            socket_name[0] = '\0';
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(std::string(std::string("CLIENT: ") + (default_client_name) + " : ").c_str());
                if (strlen(default_client_name) > 107) {
                    LOG_ERROR_SERVER("%sdefault name is longer than 107 characters, truncating\n",
                                     TAG);
                    memcpy(&socket_name[1], default_client_name, 107);
                } else memcpy(&socket_name[1], default_client_name, default_client_name_length);
            } else {
                if (strlen(name) > 107) {
                    LOG_ERROR_SERVER(
                        "%sname is longer than 107 characters, truncating, conflicts may happen\n",
                        TAG);
                    memcpy(&socket_name[1], name, 107);
                } else memcpy(&socket_name[1], name, strlen(name));
                // build new TAG
                TAG = strdup(std::string(std::string("CLIENT: ") + &socket_name[1] + " : ").c_str());
            }
            // clear for safety
            memset(&server_addr, 0, sizeof(struct sockaddr_un));
            server_addr.sun_family = AF_UNIX; // Unix Domain instead of AF_INET IP domain
            memcpy(server_addr.sun_path, socket_name, 108);
        }

        void unset_name() {
            set_name(nullptr);
        }
        SOCKET_CLIENT() {
            set_name(nullptr);
        }
        SOCKET_CLIENT(const char * name) {
            set_name(name);
        }

        ~SOCKET_CLIENT() {
            free(TAG);
            TAG = nullptr;
        }

        bool socket_put_serial(serializer &S) {
            return SOCKET_SEND_SERIAL(DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                                      &server_addr.sun_path[1]);
        }

        bool socket_get_serial(serializer &S) {
            return SOCKET_GET_SERIAL(DATA_TRANSFER_INFO, TAG, socket_data_fd, S,
                                     &server_addr.sun_path[1]);
        }

        void socket_get_fd(int &fd) {
            SOCKET_GET_FD(DATA_TRANSFER_INFO, TAG, socket_data_fd, fd, &server_addr.sun_path[1]);
        }

        bool connect_to_server() {
            socket_data_fd = socket(AF_UNIX, SOCK_STREAM, 0);
            if (socket_data_fd < 0) {
                LOG_ERROR_SERVER("%ssocket: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            LOG_INFO_SERVER("%sconnecting to server\n", TAG);
            ssize_t ret = 0;
            for (;;) {
                ret = connect(socket_data_fd, (const struct sockaddr *) &server_addr,
                              sizeof(struct sockaddr_un));
                if (ret >= 0) break;
            }
            if (ret < 0) {
                LOG_ERROR_SERVER("%sconnect: %d (%s)\n", TAG, errno, strerror(errno));
                return false;
            }
            LOG_INFO_SERVER("%sconnected to server\n", TAG);
            return true;
        }

        bool disconnect_from_server() {
            LOG_INFO_SERVER("%sclosing connection to server\n", TAG);
            assert(SOCKET_CLOSE(TAG, socket_data_fd));
            LOG_INFO_SERVER("%sclosed connection to server\n", TAG);
            if (SERVER_LOG_TRANSFER_INFO) LOG_INFO_SERVER("%sReturning response\n", TAG);
            return true;
        }

};

#endif //GLNE_SERVER_CORE_H