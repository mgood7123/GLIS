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
        LOG_INFO("SERVER: SERVER_SHUTDOWN attempting to close server %s but server has already been closed\n", server_name);
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
    delete msg[0]->deallocate();
    *msg = nullptr;
}

SOCKET_MSG * SOCKET_COMMAND(int command) {
    SOCKET_MSG * msg = SOCKET_HEADER();
    msg->put.command(command);
    return msg;
}

// returns false if an error has occurred otherwise true
bool SOCKET_READ(const char * TAG, ssize_t * ret, int data_socket, void *__buf, size_t __count, char * server_name, int flags, ssize_t total) {
    for (;;) { // implement blocking
        *ret = recv(data_socket, static_cast<void*>(static_cast<uint8_t*>(__buf)+total), __count-total, flags);
        if (*ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            else break;
        }
        break;
    }
    if (*ret == 0) {
        // if recv returns zero, that means the connection has been closed
        LOG_INFO("%sClient has closed the connection\n", TAG, server_name);
        return false;
    } else if (*ret < 0) {
        LOG_ERROR("%srecv: %s\n", TAG, server_name, strerror(errno));
        return false;
    }
    return true;
}

// returns false if an error has occurred otherwise true
bool SOCKET_WRITE(const char * TAG, ssize_t * ret, int data_socket, const void *__buf, size_t __count, char * server_name, int flags, ssize_t total) {
    for (;;) { // implement blocking
        *ret = send(data_socket, static_cast<const void*>(static_cast<const uint8_t*>(__buf)+total), __count-total, flags);
        if (*ret < 0) if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
        break;
    }
    if (*ret < 0) {
        LOG_ERROR("%swrite: %s\n", TAG, strerror(errno));
        return false;
    }
    return true;
}

bool SOCKET_GET(const char * TAG, int data_socket, void *__buf, size_t __count, char * server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_READ(TAG, &ret, data_socket, __buf, __count, server_name, MSG_PEEK | MSG_DONTWAIT, total)) {
            if (SOCKET_READ(TAG, &ret, data_socket, __buf, __count, server_name, 0, total)) {
                total+=ret;
                LOG_INFO("%sRECV %zu/%zu size", TAG, total, __count);
            } else return false; // an error occurred
        } else return false; // an error occurred
    }
    return true;
}

bool SOCKET_SEND(const char * TAG, int data_socket, const void *__buf, size_t __count, char * server_name) {
    assert(__count != 0);
    ssize_t total = 0;
    while(total != __count) {
        ssize_t ret = 0;
        if (SOCKET_WRITE(TAG, &ret, data_socket, __buf, __count, server_name, 0, total)) {
            total += ret;
            LOG_INFO("%sWRITE %zu/%zu size", TAG, total, __count);
        } else return false; // an error occurred
    }
    return true;
}

bool SOCKET_GET_HEADER(const char * TAG, int data_socket, SOCKET_MSG * msg, char * server_name) {
    return SOCKET_GET(TAG, data_socket, msg->header, msg->header_length, server_name);
}

bool SOCKET_SEND_HEADER(const char * TAG, int data_socket, SOCKET_MSG * msg, char * server_name) {
    return SOCKET_SEND(TAG, data_socket, msg->header, msg->header_length, server_name);
}

bool SOCKET_GET_DATA(const char * TAG, int data_socket, SOCKET_MSG * msg, char * server_name) {
    return SOCKET_GET(TAG, data_socket, msg->data, msg->data_length, server_name);
}

bool SOCKET_SEND_DATA(const char * TAG, int data_socket, SOCKET_MSG * msg, char * server_name) {
    return SOCKET_SEND(TAG, data_socket, msg->data, msg->data_length, server_name);
}

void* SERVER_START(void* na) {
    assert(na != nullptr);
    SOCKET_SERVER_DATA * internaldata = static_cast<SOCKET_SERVER_DATA*>(na);
    ssize_t ret = 0;
    int buffer = 0;
    int socket_fd = 0;
    int data_socket = 0;
    bool has_data_socket = false;

    // build TAG
    const char * TAG = std::string(std::string("SERVER: ") + (internaldata->socket_name+1) + " : ").c_str();

    LOG_INFO("%sStart server setup\n", TAG);

    // AF_UNIX for domain unix IPC and SOCK_STREAM since it works for the example
    socket_fd = socket(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if (socket_fd < 0) {
        LOG_ERROR("%ssocket: %s\n", TAG, strerror(errno));
        exit(EXIT_FAILURE);
    }
    LOG_INFO("%sSocket made\n", TAG);

    // clear for safety
    memset(&internaldata->server_addr, 0, sizeof(struct sockaddr_un));
    internaldata->server_addr.sun_family = AF_UNIX; // Unix Domain instead of AF_INET IP domain
    memcpy(internaldata->server_addr.sun_path, internaldata->socket_name, 108);
    LOG_INFO("%sbinding socket fd %d to name %s\n", TAG, socket_fd, internaldata->server_addr.sun_path +1);
    ret = bind(socket_fd, (const struct sockaddr *) &internaldata->server_addr, sizeof(struct sockaddr_un));
    if (ret < 0) {
        LOG_ERROR("%sbind: %s\n", TAG, strerror(errno));
        exit(EXIT_FAILURE);
    }
    LOG_INFO("%sBind made\n", TAG);

    // Open 8 back buffers for this demo
    ret = listen(socket_fd, 8);
    if (ret < 0) {
        LOG_ERROR("%slisten: %s", TAG, strerror(errno));
        exit(EXIT_FAILURE);
    }
    LOG_INFO("%sSocket listening for packages\n", TAG);
    internaldata->server_CAN_CONNECT = true;
    LOG_INFO("%sServer setup complete\n", TAG);

    while (!internaldata->server_should_close) {
        // Wait for incoming connection.
        has_data_socket = false;
        for(;;) {
            if (internaldata->server_should_close) {
                internaldata->server_should_close_during_accept = true;
                break;
            }
            data_socket = accept(socket_fd, NULL, NULL);
            if (data_socket < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                else break;
            }
            break;
        }
        if (internaldata->server_should_close_during_accept) continue;
        if (data_socket < 0) {
            LOG_ERROR("%saccept: %s\n", TAG, strerror(errno));
            char * server_name = SOCKET_SERVER_name_to_server_name(internaldata);
            SERVER_SHUTDOWN(server_name, internaldata);
            delete server_name;
            exit(EXIT_FAILURE);
        }
        has_data_socket = true;
        LOG_INFO("%sAccepted data\n", TAG, internaldata->server_addr.sun_path + 1);
        // This is the main loop for handling connections

        // first we obtain the header
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_header;
        internaldata->HEADER = SOCKET_HEADER();
        LOG_INFO("%swaiting for header\n", TAG, internaldata->server_addr.sun_path + 1);
        if (!SOCKET_GET_HEADER(TAG, data_socket, internaldata->HEADER, internaldata->server_addr.sun_path+1)) {
            SOCKET_DELETE(&internaldata->HEADER);
            continue;
        }
        LOG_INFO("%sReceived header\n", TAG, internaldata->server_addr.sun_path + 1);
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.received_header;
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_header;
        LOG_INFO("%sProcessing header\n", TAG, internaldata->server_addr.sun_path + 1);
        internaldata->reply(internaldata);
        while(internaldata->STATE != SOCKET_SERVER_DATA_STATE.processed_reply_to_header);
        LOG_INFO("%sProcessed header\n", TAG, internaldata->server_addr.sun_path + 1);
        LOG_INFO("%sSending reply\n", TAG, internaldata->server_addr.sun_path + 1);
        if (!SOCKET_SEND_HEADER(TAG, data_socket, internaldata->REPLY, internaldata->server_addr.sun_path+1)) {
            SOCKET_DELETE(&internaldata->REPLY);
            continue;
        }
        SOCKET_DELETE(&internaldata->REPLY);
        LOG_INFO("%sSent reply\n", TAG, internaldata->server_addr.sun_path + 1);
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.sent_reply_to_header;

        // then we obtain the data according to the header
        if (!internaldata->HEADER->get.expect_data()) {
            LOG_INFO("%sno data is expected\n", TAG, internaldata->server_addr.sun_path + 1);
            continue;
        }
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_data;
        internaldata->DATA = SOCKET_DATA_RESPONSE(internaldata->HEADER->get.length());
        LOG_INFO("%swaiting for data\n", TAG, internaldata->server_addr.sun_path + 1);
        if (!SOCKET_GET_DATA(TAG, data_socket, internaldata->DATA, internaldata->server_addr.sun_path+1)) {
            SOCKET_DELETE(&internaldata->DATA);
            continue;
        }
        LOG_INFO("%sReceived data\n", TAG, internaldata->server_addr.sun_path + 1);
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.received_data;
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.waiting_for_reply_to_data;
        // do not delete the header as we can use it to determine the reply type
        internaldata->reply(internaldata);
        while(internaldata->STATE != SOCKET_SERVER_DATA_STATE.processed_reply_to_data);
        // we no longer need the header and data
        SOCKET_DELETE(&internaldata->HEADER);
        SOCKET_DELETE(&internaldata->DATA);
        LOG_INFO("%sSending reply\n", TAG, internaldata->server_addr.sun_path + 1);
        if (!SOCKET_SEND_DATA(TAG, data_socket, internaldata->REPLY, internaldata->server_addr.sun_path+1)) {
            SOCKET_DELETE(&internaldata->REPLY);
            continue;
        }
        SOCKET_DELETE(&internaldata->REPLY);
        LOG_INFO("%sSent data\n", TAG, internaldata->server_addr.sun_path + 1);
        internaldata->STATE = SOCKET_SERVER_DATA_STATE.sent_reply_to_data;
    }
    LOG_INFO("%sclosing server\n", TAG);
    if (has_data_socket) close(data_socket);
    close(socket_fd);
    internaldata->server_closed = true;
    LOG_INFO("%sclosed server\n", TAG);
    return NULL;
}

class SOCKET_SERVER {
    private:
        pthread_t server_thread = 0;
        char server_name[107];
        const char * default_server_name = "SOCKET_SERVER";
        const size_t default_server_name_length = strlen(default_server_name);
        char * TAG = nullptr;
        SOCKET_SERVER_DATA * internaldata = nullptr;
        void (*reply)(SOCKET_SERVER_DATA *) = nullptr;
    public:
        void set_reply_callback(void (*callback)(SOCKET_SERVER_DATA *)) {
            if (internaldata == nullptr) {
                reply = callback;
            } else {
                if (reply != nullptr) reply = nullptr;
                internaldata->reply = callback;
            }
        }
        void startServer() {
            if (internaldata != nullptr) {
                LOG_INFO("%sattempting to start server %s but server has already been started\n", TAG, server_name);
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
            pthread_create(&server_thread, NULL, SERVER_START, internaldata);
            while(!internaldata->server_CAN_CONNECT) {}
        }
        void shutdownServer() {
            if (internaldata == nullptr) {
                LOG_INFO("%sshutdownServer attempting to close server %s but server has already been closed\n", TAG, server_name);
                return;
            }
            if (internaldata->server_closed) {
                LOG_INFO("%sshutdownServer internaldata->server_closed attempting to close server %s but server has already been closed\n", TAG, server_name);
                delete internaldata;
                internaldata = nullptr;
            } else SERVER_SHUTDOWN(server_name, internaldata);
        }
        SOCKET_SERVER(const char * name) {
            memset(&server_name, 0, 107);
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(std::string(std::string("SERVER: ") + (default_server_name) + " : ").c_str());
                LOG_ERROR("%sname was not supplied, conflicts are likely to happen\n", TAG);
                if (strlen(default_server_name) > 107) {
                    LOG_ERROR("%sdefault name is longer than 107 characters, truncating\n", TAG);
                    memcpy(server_name, default_server_name, 107);
                } else memcpy(server_name, default_server_name, default_server_name_length);
            } else {
                if (strlen(server_name) > 107) {
                    LOG_ERROR("%sname is longer than 107 characters, truncating, conflicts may happen\n", TAG);
                    memcpy(server_name, name, 107);
                } else memcpy(server_name, name, strlen(name));
                // build new TAG
                TAG = strdup(std::string(std::string("SERVER: ") + server_name + " : ").c_str());
            }
        }
        ~SOCKET_SERVER() {
            free(TAG);
            TAG = nullptr;
        }
};

class SOCKET_CLIENT {
    public:
        const size_t BUFFER_SIZE = 16;

        int data_socket;
        struct sockaddr_un server_addr;
        const char * default_client_name = "SOCKET_SERVER";
        const size_t default_client_name_length = strlen(default_client_name);
        char * TAG = nullptr;
        SOCKET_CLIENT(const char * name) {
            char socket_name[108]; // 108 sun_path length max
            memset(&socket_name, 0, 108);
            socket_name[0] = '\0';
            if (name == nullptr || name == NULL) {
                // build default TAG
                TAG = strdup(std::string(std::string("CLIENT: ") + (default_client_name) + " : ").c_str());
                if (strlen(default_client_name) > 107) {
                    LOG_ERROR("%sdefault name is longer than 107 characters, truncating\n", TAG);
                    memcpy(&socket_name[1], default_client_name, 107);
                } else memcpy(&socket_name[1], default_client_name, default_client_name_length);
            } else {
                if (strlen(name) > 107) {
                    LOG_ERROR("%sname is longer than 107 characters, truncating, conflicts may happen\n", TAG);
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

        ~SOCKET_CLIENT() {
            free(TAG);
            TAG = nullptr;
        }

        SOCKET_MSG * send(SOCKET_MSG * header, void * data, size_t len) {
            data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
            if (data_socket < 0) {
                LOG_ERROR("%ssocket: %s\n", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            LOG_INFO("%sconnecting to server\n", TAG);
            ssize_t ret = connect(data_socket, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr_un));
            if (ret < 0) {
                LOG_ERROR("%sconnect: %s\n", TAG, strerror(errno));
                exit(EXIT_FAILURE);
            }
            LOG_INFO("%sconnected to server\n", TAG);
            header->put.length(len);

            // TODO: server state CANNOT be relied on since this MUST work across machines
            LOG_INFO("%ssend header\n", TAG);
            SOCKET_SEND_HEADER(TAG, data_socket, header, server_addr.sun_path+1);
            SOCKET_MSG * response = SOCKET_HEADER();
            LOG_INFO("%sget header response\n", TAG);
            SOCKET_GET_HEADER(TAG, data_socket, response, server_addr.sun_path+1);
            LOG_INFO("%scheck header response\n", TAG);
            if (response->get.response() == SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK) {
                LOG_INFO("%sSuccess\n", TAG);
                SOCKET_DELETE(&response);
                SOCKET_MSG * data_ = SOCKET_DATA(data, len);
                LOG_INFO("%ssending data, size of data: %zu\n", TAG, len);
                SOCKET_SEND_DATA(TAG, data_socket, data_, server_addr.sun_path+1);
                response = SOCKET_DATA_RESPONSE(header->get.length());
                LOG_INFO("%sget data response\n", TAG);
                SOCKET_GET_DATA(TAG, data_socket, response, server_addr.sun_path+1);
                int da = static_cast<int *>(response->data)[0];
                LOG_INFO("%scheck data response\n", TAG);
                if (response->get.response() == SERVER_MESSAGES.SERVER_MESSAGE_RESPONSE.OK) {
                    LOG_INFO("%sSuccess\n", TAG);
                    LOG_INFO("%sclosing connection to server\n", TAG);
                    close(data_socket);
                    LOG_INFO("%sclosed connection to server\n", TAG);
                    LOG_INFO("%sReturning response\n", TAG);
                    return response;
                }
            }
            SOCKET_DELETE(&response);
            LOG_INFO("%sclosing connection to server\n", TAG);
            close(data_socket);
            LOG_INFO("%sclosed connection to server\n", TAG);
            LOG_INFO("%sReturning response\n", TAG);
            return SOCKET_HEADER();
        }

        SOCKET_MSG *send(void * data, size_t data_length) {
            SOCKET_MSG * header = SOCKET_COMMAND(SERVER_MESSAGES.SERVER_MESSAGE_TYPE.mirror);
            SOCKET_MSG * response = send(header, data, data_length);
            SOCKET_DELETE(&header);
            return response;
        }
};

#endif //GLNE_SERVER_CORE_H
