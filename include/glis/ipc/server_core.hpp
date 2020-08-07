//
// Created by konek on 8/13/2019.
//

#pragma once

#include <glis/ipc/serializer.hpp>
#include <WINAPI/SDK/include/Windows/Kernel/WindowsAPIKernel.h>
#include <sys/un.h>
#include <sys/socket.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
extern bool SERVER_LOG_TRANSFER_INFO;

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
};

extern class SERVER_MESSAGES SERVER_MESSAGES;

void SERVER_SHUTDOWN(char * server_name, SOCKET_SERVER_DATA * & internaldata);

char *SOCKET_SERVER_name_to_server_name(SOCKET_SERVER_DATA * internaldata);

/* Return current time in milliseconds */
long now_ms(void);

/* Return current time in nanoseconds */
long now_nanoseconds(void);

char *str_humanise_bytes(off_t bytes);

// returns false if an error has occurred otherwise true
bool SOCKET_READ(const char *TAG, ssize_t *ret, int &socket_data_fd, void *__buf, size_t __count,
                 int flags, ssize_t total);

// returns false if an error has occurred otherwise true
bool
SOCKET_WRITE(const char *TAG, ssize_t *ret, int &socket_data_fd, const void *__buf, size_t __count,
             int flags, ssize_t total);

bool
SOCKET_GET(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, void *__buf,
           size_t __count, char *server_name);

bool
SOCKET_SEND(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, const void *__buf,
            size_t __count, char *server_name);

// returns false if an error has occurred otherwise true
bool SOCKET_READ_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, msghdr *__msg,
                         int flags, ssize_t total);

// returns false if an error has occurred otherwise true
bool SOCKET_WRITE_MESSAGE(const char *TAG, ssize_t *ret, int &socket_data_fd, const msghdr *__msg,
                          int flags, ssize_t total);

bool
SOCKET_GET_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                   msghdr *__msg,
                   size_t __count, char *server_name);

bool
SOCKET_SEND_MESSAGE(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                    const msghdr *__msg,
                    size_t __count, char *server_name);

bool
SOCKET_SEND_SERIAL(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd,
                   serializer &S,
                   char *server_name);

bool
SOCKET_GET_SERIAL(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, serializer &S,
                  char *server_name);

/*
 * send fd by socket
 */
void SOCKET_SEND_FD(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, int &fd,
                    char *server_name);

void SOCKET_GET_FD(SOCKET_DATA_TRANSFER_INFO &s, const char *TAG, int &socket_data_fd, int &fd,
                   char *server_name);

bool SOCKET_CLOSE(const char *TAG, int & socket_fd);

extern const char * LOG_TAG_SERVER;

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

        int log_info(const char* fmt, ...);

        void startServer(void *(*SERVER_MAIN)(void *));

        void shutdownServer();

        void set_name(const char *name);

        void unset_name();

        SOCKET_SERVER();

        SOCKET_SERVER(const char *name);

        SOCKET_SERVER& operator=(const SOCKET_SERVER&);

        ~SOCKET_SERVER();

        bool socket_create(int &socket_fd, int __af, int __type, int __protocol);

        bool socket_bind(int &socket_fd, int __af);

        //       The pending_connection_queue_size argument defines the maximum length to which the
        //       queue of pending connections for socket_fd may grow.
        //       If a connection request arrives when the queue is full, the client may receive an
        //       error with an indication of ECONNREFUSED or, if the underlying protocol supports
        //       retransmission, the request may be ignored so that a later reattempt at connection
        //       succeeds.
        //
        // sets internaldata->server_CAN_CONNECT to true on success
        bool socket_listen(int &socket_fd, int pending_connection_queue_size);

        // returns false if internaldata->server_should_close is true or if an error has occured
        // otherwise returns true upon a successful accept attempt
        bool socket_accept(int &socket_fd, int &socket_data_fd);

        // returns false if internaldata->server_should_close is true, if an error has occured,
        // or upon a failure to connect
        // otherwise returns true upon a successful accept attempt
        bool socket_accept_non_blocking(int &socket_fd, int &socket_data_fd);

        bool socket_unaccept(int &socket_data_fd);

        bool connection_is_alive(int &socket_data_fd);

        bool connection_wait_until_disconnect(int &socket_data_fd);

        void socket_close(int &socket_fd, int &socket_data_fd);

        bool socket_create(int __af, int __type, int __protocol);

        bool socket_bind(int __af);

        //       The pending_connection_queue_size argument defines the maximum length to which the
        //       queue of pending connections for socket_fd may grow.
        //       If a connection request arrives when the queue is full, the client may receive an
        //       error with an indication of ECONNREFUSED or, if the underlying protocol supports
        //       retransmission, the request may be ignored so that a later reattempt at connection
        //       succeeds.
        //
        // sets internaldata->server_CAN_CONNECT to true on success
        bool socket_listen(int pending_connection_queue_size);

        // returns false if internaldata->server_should_close is true or if an error has occured
        // otherwise returns true upon a successful accept attempt
        bool socket_accept();

        // returns false if internaldata->server_should_close is true, if an error has occured,
        // or upon a failure to connect
        // otherwise returns true upon a successful accept attempt
        bool socket_accept_non_blocking();

        bool socket_unaccept();

        bool connection_is_alive();

        bool connection_wait_until_disconnect();

        void socket_close();

        bool socket_put_serial(serializer &S);

        bool socket_get_serial(serializer &S);

        void socket_put_fd(int &fd);

        void socket_get_fd(int &fd);


    char server_name[107];
};

extern Kernel SERVER_KERNEL;

SOCKET_SERVER *SERVER_get(size_t id);

char *SERVER_allocate_new_server(void *(*SERVER_MAIN)(void *), size_t &id);

char *SERVER_allocate_new_server(size_t &id);

void SERVER_deallocate_server(size_t &id);

void *SERVER_START_REPLY_MANUALLY(void *na);

class SOCKET_CLIENT {
    public:
        int socket_data_fd = 0;
        struct sockaddr_un server_addr = {0};
        const char * default_client_name = "SOCKET_SERVER";
        const size_t default_client_name_length = strlen(default_client_name);
        char * TAG = nullptr;
        SOCKET_DATA_TRANSFER_INFO DATA_TRANSFER_INFO;

        int log_info(const char* fmt, ...);

        void set_name(const char * name);

        void unset_name();
        SOCKET_CLIENT();
        SOCKET_CLIENT(const char * name);

        ~SOCKET_CLIENT();

        bool socket_put_serial(serializer &S);

        bool socket_get_serial(serializer &S);

        void socket_put_fd(int &fd);

        void socket_get_fd(int &fd);

        bool connect_to_server();

        bool disconnect_from_server();

};
#pragma clang diagnostic pop