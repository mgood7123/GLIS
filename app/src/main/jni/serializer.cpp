//
// Created by konek on 8/21/2019.
//

#include "serializer.h"

#ifndef __ANDROID__

void ser() {
    serializer X;
    X.add<size_t>(5); // allocates vector index
    X.add<uint64_t>(UINT64_MAX); // allocates vector index
    X.add<double>(6.8); // allocates vector index
    size_t size2[2] = {55, 58};
    X.add_pointer<size_t>(size2, 2); // allocates vector index
    X.construct(); // consumes all vector indexes
    serializer XX;
    XX.stream.allocate(X.stream.data_len);
    memcpy(XX.stream.data, X.stream.data, XX.stream.data_len);
    X.stream.deallocate();
    size_t V1;
    uint64_t V2;
    double V3;
    size_t *V4;
    XX.deconstruct(); // allocates vector indexes, MUST be followed by a free__()
    XX.get<size_t>(&V1); // consumes vector index
    XX.get<uint64_t>(&V2); // consumes vector index
    XX.get<double>(&V3); // consumes vector index
    size_t indexes = XX.get_pointer<size_t>(&V4); // consumes vector index, allocates a pointer
    LOG_INFO_serializer("V1 = %zu\n", V1);
    LOG_INFO_serializer("UINT64_MAX = %lu\n", UINT64_MAX);
    LOG_INFO_serializer("V2 =         %lu\n", V2);
    assert(V2 == UINT64_MAX);
    LOG_INFO_serializer("V3 = %G\n", V3);
    LOG_INFO_serializer("indexes = %zu\n", indexes);
    LOG_INFO_serializer("V4[0] = %zu\n", V4[0]);
    LOG_INFO_serializer("V4[1] = %zu\n", V4[1]);
    delete[] V4; // free pointer that was allocated on get_pointer
    XX.free__(); // free unused vectors, normally we do not know if all vectors add's are matched by
    // get even though we should, for example, we mey preserve indexes for future use or we may exit
    // early before all vectors are getted
}

int main() {
    ser();
    return 0;
}
#endif