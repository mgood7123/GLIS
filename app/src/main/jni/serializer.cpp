//
// Created by konek on 8/21/2019.
//

#include "serializer.h"

#ifndef __ANDROID__

void ser() {
    serializer X;
    X.add<size_t>(5);
    X.add<uint64_t>(UINT64_MAX);
    X.add<double>(6.8);
    size_t size2[2] = {55, 58};
    X.add_pointer<size_t>(size2, 2);
    X.construct();
    X.free__();
    serializer XX;
    XX.stream.allocate(X.stream.data_len);
    memcpy(XX.stream.data, X.stream.data, XX.stream.data_len);
    X.stream.deallocate();
    size_t V1;
    uint64_t V2;
    double V3;
    size_t *V4;
    XX.deconstruct();
    XX.get<size_t>(&V1);
    XX.get<uint64_t>(&V2);
    XX.get<double>(&V3);
    size_t indexes = XX.get_pointer<size_t>(&V4);
    LOG_INFO_serializer("V1 = %zu\n", V1);
    LOG_INFO_serializer("UINT64_MAX = %lu\n", UINT64_MAX);
    LOG_INFO_serializer("V2 =         %lu\n", V2);
    assert(V2 == UINT64_MAX);
    LOG_INFO_serializer("V3 = %G\n", V3);
    LOG_INFO_serializer("indexes = %zu\n", indexes);
    LOG_INFO_serializer("V4[0] = %zu\n", V4[0]);
    LOG_INFO_serializer("V4[1] = %zu\n", V4[1]);
    XX.free__();
    delete[] V4;
}

int main() {
    ser();
    return 0;
}
#endif