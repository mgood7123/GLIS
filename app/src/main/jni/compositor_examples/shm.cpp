//
// Created by konek on 8/20/2019.
//

#include "../GLIS.h"
#define LOG_TAG "shm"

size_t recshm() {
    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_MSG *socket = SOCKET_CLIENT().send(SERVER_MESSAGES.SERVER_MESSAGE_TYPE.shm, nullptr,
                                              0);
//    assert(socket != nullptr);
//    assert(socket->data != nullptr);
//    size_t id = reinterpret_cast<size_t *>(socket->data)[0];
//    SOCKET_DELETE(&socket);
//    return id;
}

int main() {
    LOG_INFO("recieved id: %zu", recshm());
    return 0;
}