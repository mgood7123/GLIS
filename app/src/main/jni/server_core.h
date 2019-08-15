//
// Created by konek on 8/13/2019.
//

#ifndef GLNE_SERVER_CORE_H
#define GLNE_SERVER_CORE_H

#define SOCKET_HEADER_SIZE (sizeof(size_t)*4)

typedef class SOCKET_MSG {
    public:
        SOCKET_MSG * allocate_header(size_t len) {
            header_length = len;
            header = static_cast<size_t *>(malloc(header_length));
            memset(header, 0, header_length);
            return this;
        }
        SOCKET_MSG * allocate_data(size_t len) {
            data_length = len;
            data = malloc(data_length);
            memset(data, 0, data_length);
            return this;
        }
        SOCKET_MSG * deallocate_header() {
            if (header != nullptr) {
                free(header);
                header = nullptr;
            }
            header_length = 0;
            return this;
        }
        SOCKET_MSG * deallocate_data() {
            if (data != nullptr) {
                free(data);
                data = nullptr;
            }
            data_length = 0;
            return this;
        }
        SOCKET_MSG * allocate(size_t len) {
            return allocate_header(len)->allocate_data(len);
        }
        SOCKET_MSG * deallocate() {
            return deallocate_header()->deallocate_data();
        }

        size_t * header = nullptr;
        size_t header_length = 0;
        void * data = nullptr;
        size_t data_length = 0;
        SOCKET_MSG() {
            header = nullptr;
            header_length = 0;
            data = nullptr;
            data_length = 0;
            get.header = &header;
            put.header = &header;
        }
        class get {
            public:
                size_t **header = nullptr;
                int command() {
                    return static_cast<int>(header[0][0]);
                }
                size_t length() {
                    return header[0][1];
                }
                int response() {
                    return static_cast<int>(header[0][2]);
                }
                bool expect_data() {
                    return static_cast<bool>(header[0][3]);
                }
        } get;
        class put {
            public:
                size_t **header = nullptr;
                void command(int command) {
                    header[0][0] = static_cast<size_t>(command);
                }
                void length(size_t length) {
                    header[0][1] = length;
                }
                void response(int response) {
                    header[0][2] = static_cast<size_t>(response);
                }
                void expect_data(bool expect) {
                    header[0][3] = static_cast<size_t>(expect);
                }
        } put;
} SOCKET_MSG;

class SOCKET_SERVER_DATA_STATE {
    public:
        int NO_STATE = -1;
        int waiting_for_header = 0;
        int received_header = 1;
        int waiting_for_reply_to_header = 2;
        int processed_reply_to_header = 3;
        int sent_reply_to_header = 4;
        int waiting_for_data = 5;
        int received_data = 6;
        int waiting_for_reply_to_data = 7;
        int processed_reply_to_data = 8;
        int sent_reply_to_data = 9;
} SOCKET_SERVER_DATA_STATE;

class SOCKET_SERVER_DATA;
void SERVER_DEFAULT_REPLY(SOCKET_SERVER_DATA * internaldata);

class SOCKET_SERVER_DATA {
    public:
        bool server_CAN_CONNECT = false;
        bool server_should_close = false;
        bool server_should_close_during_accept = false;
        bool server_closed = false;
        struct sockaddr_un server_addr = {0};
        char socket_name[108] = {0}; // 108 sun_path length max
        int STATE = SOCKET_SERVER_DATA_STATE.NO_STATE;
        SOCKET_MSG * HEADER = nullptr;
        SOCKET_MSG * DATA = nullptr;
        SOCKET_MSG * REPLY = nullptr;
        void (*reply)(SOCKET_SERVER_DATA * internaldata) = SERVER_DEFAULT_REPLY;
};

SOCKET_MSG * SOCKET_HEADER() {
    SOCKET_MSG *msg = new SOCKET_MSG;
    return msg->allocate_header(SOCKET_HEADER_SIZE);
}

SOCKET_MSG * SOCKET_DATA(void * data, size_t len) {
    SOCKET_MSG * msg = SOCKET_HEADER();
    msg->allocate_data(len);
    memcpy(msg->data, data, len);
    return msg;
}

SOCKET_MSG * SOCKET_DATA_RESPONSE(size_t len) {
    SOCKET_MSG * msg = new SOCKET_MSG;
    return msg->allocate(len);
}

class SERVER_MESSAGES {
    public:
        struct SERVER_MESSAGE_TYPE {
            int texture = 1;
            int mirror = 2;
            int new_window = 3;
            int close_window = 4;
        } SERVER_MESSAGE_TYPE;
        struct SERVER_MESSAGE_RESPONSE {
            int OK = 0;
            int FAIL = 1;
        } SERVER_MESSAGE_RESPONSE;
} SERVER_MESSAGES;

void SERVER_PROCESS(SOCKET_SERVER_DATA * internaldata) {
    int CMD = internaldata->HEADER->get.command();
    if (internaldata->STATE == SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_header) {
        internaldata->HEADER->put.expect_data(
            CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.texture ||
            CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.mirror
        );
        internaldata->REPLY->put.expect_data(internaldata->REPLY->get.expect_data());
        internaldata->REPLY = SOCKET_HEADER();
        internaldata->REPLY->put.response(SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK);
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.processed_reply_to_header;
    } else if (internaldata->STATE == SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_data) {
        if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.texture) {
            // sent the texture back
            internaldata->REPLY = SOCKET_DATA(internaldata->DATA->data, internaldata->DATA->data_length);
            internaldata->STATE = SOCKET_SERVER_DATA_STATE.processed_reply_to_data;
        }
        else if (CMD == SERVER_MESSAGES.SERVER_MESSAGE_TYPE.mirror) {
            // sent data back
            internaldata->REPLY = SOCKET_DATA(internaldata->DATA->data, internaldata->DATA->data_length);
            internaldata->STATE = SOCKET_SERVER_DATA_STATE.processed_reply_to_data;
        }
        internaldata->REPLY->put.response(SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK);
    }
}

void SERVER_DEFAULT_REPLY(SOCKET_SERVER_DATA * internaldata) {
    SERVER_PROCESS(internaldata);
}

void SERVER_SHUTDOWN(char * server_name, SOCKET_SERVER_DATA * & internaldata) {
    if (internaldata->server_closed) {
        SERVER_LOG_ERROR(
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

void SOCKET_DELETE(SOCKET_MSG ** msg) {
    if (msg[0] == nullptr) return;
    delete msg[0]->deallocate();
    msg[0] = nullptr;
}

SOCKET_MSG * SOCKET_COMMAND(int command) {
    SOCKET_MSG * msg = SOCKET_HEADER();
    msg->put.command(command);
    return msg;
}

// returns false if an error has occurred otherwise true
bool SOCKET_READ(const char *TAG, ssize_t *ret, int socket_data_fd, void *__buf, size_t __count,
                 char *server_name, int flags, ssize_t total) {
    for (;;) { // implement blocking
        *ret = recv(socket_data_fd, static_cast<void *>(static_cast<uint8_t *>(__buf) + total),
                    __count - total, flags);
        if (*ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            else break;
        }
        break;
    }
    if (*ret == 0) {
        // if recv returns zero, that means the connection has been closed
        SERVER_LOG_INFO("%sClient has closed the connection\n", TAG);
        return false;
    } else if (*ret < 0) {
        SERVER_LOG_ERROR("%srecv: %s\n", TAG, strerror(errno));
        return false;
    }
    return true;
}

// returns false if an error has occurred otherwise true
bool
SOCKET_WRITE(const char *TAG, ssize_t *ret, int socket_data_fd, const void *__buf, size_t __count,
             char *server_name, int flags, ssize_t total) {
    for (;;) { // implement blocking
        *ret = send(socket_data_fd,
                    static_cast<const void *>(static_cast<const uint8_t *>(__buf) + total),
                    __count - total, flags);
        if (*ret < 0) if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
        break;
    }
    if (*ret < 0) {
        SERVER_LOG_ERROR("%swrite: %s\n", TAG, strerror(errno));
        return false;
    }
    return true;
}

bool
SOCKET_GET(const char *TAG, int socket_data_fd, void *__buf, size_t __count, char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_READ(TAG, &ret, socket_data_fd, __buf, __count, server_name,
                        MSG_PEEK | MSG_DONTWAIT, total)) {
            if (SOCKET_READ(TAG, &ret, socket_data_fd, __buf, __count, server_name, 0, total)) {
                total+=ret;
                SERVER_LOG_INFO("%sRECV %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    return true;
}

bool SOCKET_SEND(const char *TAG, int socket_data_fd, const void *__buf, size_t __count,
                 char *server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_WRITE(TAG, &ret, socket_data_fd, __buf, __count, server_name, 0, total)) {
            total += ret;
            SERVER_LOG_INFO("%sWRITE %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    return true;
}

bool SOCKET_GET_HEADER(const char *TAG, int socket_data_fd, SOCKET_MSG *msg, char *server_name) {
    return SOCKET_GET(TAG, socket_data_fd, msg->header, msg->header_length, server_name);
}

bool SOCKET_SEND_HEADER(const char *TAG, int socket_data_fd, SOCKET_MSG *msg, char *server_name) {
    return SOCKET_SEND(TAG, socket_data_fd, msg->header, msg->header_length, server_name);
}

bool SOCKET_GET_DATA(const char *TAG, int socket_data_fd, SOCKET_MSG *msg, char *server_name) {
    return SOCKET_GET(TAG, socket_data_fd, msg->data, msg->data_length, server_name);
}

bool SOCKET_SEND_DATA(const char *TAG, int socket_data_fd, SOCKET_MSG *msg, char *server_name) {
    return SOCKET_SEND(TAG, socket_data_fd, msg->data, msg->data_length, server_name);
}

void *SERVER_START(void *na);

class SOCKET_SERVER {
    private:
        pthread_t server_thread = 0;
        char server_name[107];
        const char *default_server_name = "SOCKET_SERVER";
        const size_t default_server_name_length = strlen(default_server_name);

        void (*reply)(SOCKET_SERVER_DATA *) = nullptr;

    public:
        char *TAG = nullptr;
        SOCKET_SERVER_DATA *internaldata = nullptr;
        int socket_fd = 0;
        int socket_data_fd = 0;

        void set_reply_callback(void (*callback)(SOCKET_SERVER_DATA *)) {
            if (internaldata == nullptr) {
                reply = callback;
            } else {
                if (reply != nullptr) reply = nullptr;
                internaldata->reply = callback;
            }
        }

        void startServer(void *(*SERVER_MAIN)(void *)) {
            if (internaldata != nullptr) {
                SERVER_LOG_ERROR("%sattempting to start server while running\n", TAG);
                return;
            }
            internaldata = new SOCKET_SERVER_DATA;
            if (reply != nullptr) {
                internaldata->reply = reply;
                reply = nullptr;
            }
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
                SERVER_LOG_ERROR(
                    "%sshutdownServer attempting to close server but server has already been closed\n",
                    TAG);
                return;
            }
            if (internaldata->server_closed) {
                SERVER_LOG_ERROR(
                    "%sshutdownServer internaldata->server_closed attempting to close serverbut server has already been closed\n",
                    TAG);
                delete internaldata;
                internaldata = nullptr;
            } else SERVER_SHUTDOWN(server_name, internaldata);
        }

        void set_name(const char *name) {
            if (internaldata != nullptr) {
                SERVER_LOG_ERROR("%sattempting to change server name while server is running\n",
                                 TAG);
                return;
            }
            memset(&server_name, 0, 107);
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(
                    std::string(std::string("SERVER: ") + (default_server_name) + " : ").c_str());
                SERVER_LOG_ERROR("%sname was not supplied, conflicts are likely to happen\n", TAG);
                if (strlen(default_server_name) > 107) {
                    SERVER_LOG_ERROR("%sdefault name is longer than 107 characters, truncating\n",
                                     TAG);
                    memcpy(server_name, default_server_name, 107);
                } else memcpy(server_name, default_server_name, default_server_name_length);
            } else {
                if (strlen(server_name) > 107) {
                    SERVER_LOG_ERROR(
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
                SERVER_LOG_ERROR("%ssocket: %s\n", TAG, strerror(errno));
                return false;
            }
            SERVER_LOG_INFO("%sSocket made\n", TAG);
            return true;
        }

        bool socket_bind(int &socket_fd, __kernel_sa_family_t __af) {
            memset(&internaldata->server_addr, 0, sizeof(struct sockaddr_un));
            internaldata->server_addr.sun_family = __af;
            memcpy(internaldata->server_addr.sun_path, internaldata->socket_name, 108);
            SERVER_LOG_INFO("%sbinding socket fd %d to name %s\n", TAG, socket_fd,
                            &internaldata->server_addr.sun_path[1]);
            if (bind(socket_fd, (const struct sockaddr *) &internaldata->server_addr,
                     sizeof(struct sockaddr_un)) < 0) {
                SERVER_LOG_ERROR("%sbind: %s\n", TAG, strerror(errno));
                return false;
            }
            SERVER_LOG_INFO("%sBind made\n", TAG);
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
                SERVER_LOG_ERROR("%slisten: %s", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            SERVER_LOG_INFO("%sSocket listening for packages\n", TAG);
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
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                    else break;
                }
                break;
            }
            if (socket_data_fd < 0) {
                SERVER_LOG_ERROR("%saccept: %s\n", TAG, strerror(errno));
                return false;
            }
            return true;
        }

        bool socket_get_header(int &socket_data_fd) {
            internaldata->HEADER = SOCKET_HEADER();
            if (!SOCKET_GET_HEADER(TAG, socket_data_fd, internaldata->HEADER,
                                   &internaldata->server_addr.sun_path[1])) {
                SOCKET_DELETE(&internaldata->HEADER);
                return false;
            }
            return true;
        }

        void socket_process_header() {
            internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_header;
            internaldata->reply(internaldata);
            // in case reply callback pthreads then returns, forking is Undefined Behaviour
            while (internaldata->STATE != SOCKET_SERVER_DATA_STATE.processed_reply_to_header);
        }

        bool socket_put_header(int &socket_data_fd) {
            if (!SOCKET_SEND_HEADER(TAG, socket_data_fd, internaldata->REPLY,
                                    &internaldata->server_addr.sun_path[1])) {
                SOCKET_DELETE(&internaldata->HEADER);
                SOCKET_DELETE(&internaldata->REPLY);
                return false;
            }
            SOCKET_DELETE(&internaldata->REPLY);
            return true;
        }

        bool socket_header_expect_data() {
            if (!internaldata->HEADER->get.expect_data()) {
                SOCKET_DELETE(&internaldata->HEADER);
                return false;
            }
            return true;
        }

        bool socket_get_data(int &socket_data_fd) {
            internaldata->DATA = SOCKET_DATA_RESPONSE(internaldata->HEADER->get.length());
            if (!SOCKET_GET_DATA(TAG, socket_data_fd, internaldata->DATA,
                                 &internaldata->server_addr.sun_path[1])) {
                SOCKET_DELETE(&internaldata->HEADER);
                SOCKET_DELETE(&internaldata->DATA);
                return false;
            }
            return true;
        }

        void socket_process_data() {
            internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_data;
            internaldata->reply(internaldata);
            while (internaldata->STATE != SOCKET_SERVER_DATA_STATE.processed_reply_to_data);
            SOCKET_DELETE(&internaldata->HEADER);
            SOCKET_DELETE(&internaldata->DATA);
        }

        bool socket_put_data(int &socket_data_fd) {
            if (!SOCKET_SEND_DATA(TAG, socket_data_fd, internaldata->REPLY,
                                  &internaldata->server_addr.sun_path[1])) {
                SOCKET_DELETE(&internaldata->REPLY);
                return false;
            }
            SOCKET_DELETE(&internaldata->REPLY);
            return true;
        }

        void socket_loop(int &socket_fd, int &socket_data_fd) {
            while (socket_accept(socket_fd, socket_data_fd)) {
                // OBTAIN AND PROCESS HEADER
                if (socket_get_header(socket_data_fd)) { // false if fails
                    socket_process_header();
                    if (socket_put_header(socket_data_fd)) { // false if fails
                        // IF WE EXPECT DATA, OBTAIN AND PROCESS IT
                        if (socket_header_expect_data()) { // false if fails
                            if (socket_get_data(socket_data_fd)) { // false if fails
                                socket_process_data();
                                socket_put_data(socket_data_fd);
                            }
                        }
                    }
                }
            }
        }

        void socket_close(int &socket_fd, int &socket_data_fd) {
            if (socket_data_fd) close(socket_data_fd);
            close(socket_fd);
            internaldata->server_closed = true;
        }

        bool socket_create(__kernel_sa_family_t __af, int __type, int __protocol) {
            return socket_create(socket_fd, __af, __type, __protocol);
        }

        bool socket_bind(__kernel_sa_family_t __af) { return socket_bind(socket_fd, __af); }

        bool socket_listen(int pending_connection_queue_size) {
            return socket_listen(socket_fd, pending_connection_queue_size);
        }

        bool socket_accept() { return socket_accept(socket_fd, socket_data_fd); }

        bool socket_get_header() { return socket_get_header(socket_data_fd); }

        bool socket_put_header() { return socket_put_header(socket_data_fd); }

        bool socket_get_data() { return socket_get_data(socket_data_fd); }

        bool socket_put_data() { return socket_put_data(socket_data_fd); }

        void socket_loop() { socket_loop(socket_fd, socket_data_fd); }

        void socket_close() { socket_close(socket_fd, socket_data_fd); }
};

void *SERVER_START_REPLY_AUTOMATICALLY(void *na) {
    assert(na != nullptr);
    SOCKET_SERVER *server = static_cast<SOCKET_SERVER *>(na);

    server->socket_create(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    server->socket_bind(AF_UNIX);
    server->socket_listen(1);
    server->socket_loop();
    server->socket_close();
    return NULL;
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

class SOCKET_CLIENT {
    public:
        const size_t BUFFER_SIZE = 16;

        int socket_data_fd;
        struct sockaddr_un server_addr;
        const char * default_client_name = "SOCKET_SERVER";
        const size_t default_client_name_length = strlen(default_client_name);
        char * TAG = nullptr;
        void set_name(const char * name) {
            char socket_name[108]; // 108 sun_path length max
            memset(&socket_name, 0, 108);
            socket_name[0] = '\0';
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(std::string(std::string("CLIENT: ") + (default_client_name) + " : ").c_str());
                if (strlen(default_client_name) > 107) {
                    SERVER_LOG_ERROR("%sdefault name is longer than 107 characters, truncating\n",
                                     TAG);
                    memcpy(&socket_name[1], default_client_name, 107);
                } else memcpy(&socket_name[1], default_client_name, default_client_name_length);
            } else {
                if (strlen(name) > 107) {
                    SERVER_LOG_ERROR(
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

        SOCKET_MSG * send(SOCKET_MSG * header, void * data, size_t len) {
            socket_data_fd = socket(AF_UNIX, SOCK_STREAM, 0);
            if (socket_data_fd < 0) {
                SERVER_LOG_ERROR("%ssocket: %s\n", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            SERVER_LOG_INFO("%sconnecting to server\n", TAG);
            ssize_t ret = 0;
            for (;;) {
                ret = connect(socket_data_fd, (const struct sockaddr *) &server_addr,
                              sizeof(struct sockaddr_un));
                if (ret >= 0) break;
            }
            if (ret < 0) {
                SERVER_LOG_ERROR("%sconnect: %s\n", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            SERVER_LOG_INFO("%sconnected to server\n", TAG);

            header->put.length(len);
            SERVER_LOG_INFO("%ssend header\n", TAG);
            SOCKET_SEND_HEADER(TAG, socket_data_fd, header, &server_addr.sun_path[1]);
            SOCKET_MSG * response = SOCKET_HEADER();
            SERVER_LOG_INFO("%sget header response\n", TAG);
            SOCKET_GET_HEADER(TAG, socket_data_fd, response, &server_addr.sun_path[1]);
            SERVER_LOG_INFO("%scheck header response\n", TAG);
            if (response->get.response() == SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK) {
                SERVER_LOG_INFO("%sSuccess\n", TAG);
                if (response->get.expect_data()) {
                    size_t response_len = response->get.length();
                    SOCKET_DELETE(&response);
                    SOCKET_MSG *data_ = SOCKET_DATA(data, len);
                    SERVER_LOG_INFO("%ssending data, size of data: %zu\n", TAG, len);
                    SOCKET_SEND_DATA(TAG, socket_data_fd, data_, &server_addr.sun_path[1]);
                    SERVER_LOG_INFO("%ssent data, size of data: %zu\n", TAG, len);
                    if (response_len != 0) {
                        SERVER_LOG_INFO("%sget data response, expect %zu\n", TAG, response_len);
                        response = SOCKET_DATA_RESPONSE(response_len);
                        SOCKET_GET_DATA(TAG, socket_data_fd, response, &server_addr.sun_path[1]);
                        SERVER_LOG_INFO("%scheck data response\n", TAG);
                        if (response->get.response() ==
                            SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK) {
                            SERVER_LOG_INFO("%sSuccess\n", TAG);
                            SERVER_LOG_INFO("%sclosing connection to server\n", TAG);
                            close(socket_data_fd);
                            SERVER_LOG_INFO("%sclosed connection to server\n", TAG);
                            SERVER_LOG_INFO("%sReturning response\n", TAG);
                            return response;
                        }
                    }
                } else
                    SERVER_LOG_INFO("%sServer is not expecting data\n", TAG);
            }
            SOCKET_DELETE(&response);
            SERVER_LOG_INFO("%sclosing connection to server\n", TAG);
            close(socket_data_fd);
            SERVER_LOG_INFO("%sclosed connection to server\n", TAG);
            SERVER_LOG_INFO("%sReturning response\n", TAG);
            return SOCKET_HEADER();
        }

        SOCKET_MSG *send(int type, void *data, size_t data_length) {
            SOCKET_MSG *header = SOCKET_COMMAND(type);
            SOCKET_MSG * response = send(header, data, data_length);
            SOCKET_DELETE(&header);
            return response;
        }
};

#endif //GLNE_SERVER_CORE_H
