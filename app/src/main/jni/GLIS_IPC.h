//
// Created by konek on 8/28/2019.
//

#ifndef GLNE_GLIS_IPC_H
#define GLNE_GLIS_IPC_H

class IPC_MODE {
    public:
        // For threads the texture fd is passed directly
        int thread = 0;
        // For textures, the process is the same for threads except the texture is copied
        int texture = 1;
        // For sockets, SWAY uses a variable length ISA approach:
        // a header is first sent containing the instruction to be performed,
        // and the length of the expected data to be sent,
        //
        // this header is of a FIXED length
        //
        // then the data itself is then sent
        int socket = 2;
        // the same as texture except shared memory is used
        int shared_memory = 3;

} IPC_MODE;

int IPC = IPC_MODE.shared_memory;


#include "shm.h"

#endif //GLNE_GLIS_IPC_H
