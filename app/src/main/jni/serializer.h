//
// Created by konek on 8/21/2019.
//

#ifndef GLNE_SERIALIZER_H
#define GLNE_SERIALIZER_H

#ifndef __ANDROID__
#define LOG_INFO_serializer printf
    #define LOG_ERROR_serializer printf
#else

    #include <strings.h>
    #include <android/log.h>

    #define LOG_TAG_serializer "serializer"
    #define LOG_INFO_serializer(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG_serializer, __VA_ARGS__)
    #define LOG_ERROR_serializer(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG_serializer, __VA_ARGS__)
#endif

#include <deque> // std::deque
#include <vector> // std::vector
#include <memory> // std::unique_ptr
#include <cassert> // assert
#include <stdio.h> // printf
#include <memory.h> // mem*
#include <inttypes.h> // fixes size types
#include <cerrno> // errno, errors
#include <stdlib.h> // malloc/realloc/free
#include <limits.h> // *_MAX

struct serializer_data {
    int8_t type_size;
    char *data;
    size_t data_len;
};

class serializer_stream {
    public:
        int8_t *data = nullptr;
        size_t data_len = 0;

        bool allocate(size_t len) {
            if (data == nullptr) {
                data = static_cast<int8_t *>(malloc(len));
                if (data == NULL) if (errno == ENOMEM) return false;
                data_len = len;
                memset(data, 0, data_len);
            }
            return true;
        }

        bool resize(size_t len) {
            if (data == nullptr && len != 0) return allocate(len);
            if (data_len == len && data != nullptr && len != 0) return true;
            int8_t *data_tmp = static_cast<int8_t *>(realloc(data, len));
            if (data_tmp == NULL) if (errno == ENOMEM) return false;
            data = data_tmp;
            data_len = len;
            if (data_len == 0 && data != 0) {
                free(data);
                data = nullptr;
            }
            return true;
        }

        bool deallocate() {
            return resize(static_cast<size_t>(0));
        }

        bool append(size_t len) {
            return resize(data_len + len);
        }

        bool retract(size_t len) {
            return resize(data_len - len);
        }

        serializer_stream retract_from_front(size_t len) {
            assert(data != NULL);
            serializer_stream data_;
            data_.allocate(len);
            memcpy(data_.data, data, len);
            memmove(data, data + len, data_len - len);
            resize(data_len - len);
            return data_;
        }

        ~serializer_stream() {
            deallocate();
        }
};

typedef std::deque<struct serializer_data> Serial;

class serializer {
    private:
        template<typename type, typename matches>
        bool add_if_matches(type *data, size_t data_len) {
            int8_t ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data s;
                s.type_size = ts;
                s.data = new char[sizeof(matches) * data_len];
                memcpy(s.data, data, sizeof(matches) * data_len);
                s.data_len = data_len;
                in.push_back(s);
                return true;
            }
            return false;
        }

        template<typename type, typename matches>
        bool remove_if_matches(type *data) {
            if (out.empty()) return false;
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data data_ = out.front();
                out.pop_front();
                memcpy(data, reinterpret_cast<matches *>(data_.data),
                       sizeof(matches) * data_.data_len);
                delete[] data_.data;
                return true;
            }
            return false;
        }

        template<typename type, typename matches>
        size_t remove_raw_pointer_if_matches(type **data) {
            if (out.empty()) return 0;
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data data_ = out.front();
                out.pop_front();
                *static_cast<type **>(data) = new type[data_.data_len];
                memcpy(*data, reinterpret_cast<type *>(data_.data), sizeof(type) * data_.data_len);
                delete[] data_.data;
                return data_.data_len;
            }
            return 0;
        }

        template<typename type, typename matches>
        size_t remove_vector_pointer_if_matches(std::vector<type> &data) {
            if (out.empty()) return 0;
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data data_ = out.front();
                out.pop_front();
                data.resize(data_.data_len);
                memcpy(data.data(), reinterpret_cast<type *>(data_.data),
                       sizeof(type) * data_.data_len);
                delete[] data_.data;
                return data_.data_len;
            }
            return 0;
        }
    public:
        Serial in;
        struct serializer_stream stream;
        Serial out;
        template<typename TYPE>
        bool add(TYPE data) {
            return add_pointer<TYPE>(&data, static_cast<size_t>(1));
        }

        template<typename TYPE>
        bool get(TYPE *data) {
            if (remove_if_matches<TYPE, int8_t>(data)) return true;
            if (remove_if_matches<TYPE, int16_t>(data)) return true;
            if (remove_if_matches<TYPE, int32_t>(data)) return true;
            if (remove_if_matches<TYPE, int64_t>(data)) return true;
            #ifdef __SIZEOF_INT128__
            if (remove_if_matches<TYPE, __int128_t>(data)) return true;
            #endif
            return false;
        }

        template<typename TYPE>
        bool add_pointer(TYPE *data, size_t index_count) {
            if (add_if_matches<TYPE, int8_t>(data, index_count)) return true;
            if (add_if_matches<TYPE, int16_t>(data, index_count)) return true;
            if (add_if_matches<TYPE, int32_t>(data, index_count)) return true;
            if (add_if_matches<TYPE, int64_t>(data, index_count)) return true;
            #ifdef __SIZEOF_INT128__
            if (add_if_matches<TYPE, __int128_t>(data, index_count)) return true;
            #endif
            return false;
        }

        template<typename TYPE>
        size_t get_raw_pointer(TYPE **data) {
            size_t s;
            s = remove_raw_pointer_if_matches<TYPE, int8_t>(data);
            if (s != 0) return s;
            s = remove_raw_pointer_if_matches<TYPE, int16_t>(data);
            if (s != 0) return s;
            s = remove_raw_pointer_if_matches<TYPE, int32_t>(data);
            if (s != 0) return s;
            s = remove_raw_pointer_if_matches<TYPE, int64_t>(data);
            if (s != 0) return s;
            #ifdef __SIZEOF_INT128__
            s = remove_raw_pointer_if_matches<TYPE, __int128_t>(data);
            if (s != 0) return s;
            #endif
            return 0;
        }

        template<typename TYPE>
        size_t get_vector_pointer(std::vector<TYPE> &data) {
            size_t s;
            s = remove_vector_pointer_if_matches<TYPE, int8_t>(data);
            if (s != 0) return s;
            s = remove_vector_pointer_if_matches<TYPE, int16_t>(data);
            if (s != 0) return s;
            s = remove_vector_pointer_if_matches<TYPE, int32_t>(data);
            if (s != 0) return s;
            s = remove_vector_pointer_if_matches<TYPE, int64_t>(data);
            if (s != 0) return s;
            #ifdef __SIZEOF_INT128__
            s = remove_vector_pointer_if_matches<TYPE, __int128_t>(data);
            if (s != 0) return s;
            #endif
            return 0;
        }

        void construct() {
            size_t index = 0;
            while (in.size() != 0) {
                // type_size (1), data length (8), data (*)
                struct serializer_data data_ = in.front();
                in.pop_front();
                stream.append(sizeof(int8_t) * 1);
                stream.data[index++] = data_.type_size;
                stream.append(sizeof(int8_t) * sizeof(size_t));
                reinterpret_cast<size_t *>(stream.data + index)[0] = data_.data_len;
                index += sizeof(size_t);
                stream.append(data_.type_size * data_.data_len);
                memcpy(&stream.data[index], data_.data, data_.type_size * data_.data_len);
                index += data_.type_size * data_.data_len;
                delete[] data_.data;
            }
        }

        void deconstruct() {
            while (stream.data_len != 0) {
                struct serializer_data data2;
                // type_size (1), data length (8), data (*)
                serializer_stream type = stream.retract_from_front(sizeof(int8_t) * 1);
                memcpy(&data2.type_size, type.data, type.data_len);
                type.deallocate();
                serializer_stream len = stream.retract_from_front(sizeof(size_t) * 1);
                memcpy(&data2.data_len, len.data, len.data_len);
                len.deallocate();
                serializer_stream data = stream.retract_from_front(
                    data2.type_size * data2.data_len);
                data2.data = new char[data2.type_size * data2.data_len];
                memcpy(data2.data, data.data, data2.type_size * data2.data_len);
                data.deallocate();
                out.push_back(data2);
            }
        }

        void free__() {
            while (!in.empty()) {
                assert(in.size() != 0);
                assert(in.size() > 0);
                delete[] in.front().data;
                in.pop_front();
            }
            while (!out.empty()) {
                assert(out.size() != 0);
                assert(out.size() > 0);
                delete[] out.front().data;
                out.pop_front();
            }
        }

        ~serializer() {
            free__();
        }

        void info() {
            LOG_INFO_serializer("stream length: %zu\n", stream.data_len);
            std::string si("stream index:");
            for (int i = 0; i < stream.data_len; i++) {
                if (i < 10) si += "|  " + std::to_string(i);
                else if (i < 100) si += "| " + std::to_string(i);
                else si += "|" + std::to_string(i);
            }
            si += "|";
            LOG_INFO_serializer("%s", si.c_str());
            std::string sd("stream data :");
            for (int i = 0; i < stream.data_len; i++) {
                if (stream.data[i] < 10) sd += "|  " + std::to_string(stream.data[i]);
                else if (stream.data[i] < 100) sd += "| " + std::to_string(stream.data[i]);
                else sd += "|" + std::to_string(stream.data[i]);
            }
            sd += "|";
            LOG_INFO_serializer("%s", sd.c_str());
        }
};

void serializer_demo() {
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
    size_t indexes = XX.get_raw_pointer<size_t>(&V4); // consumes vector index, allocates a pointer
    LOG_INFO_serializer("V1 = %zu\n", V1);
    LOG_INFO_serializer("UINT64_MAX = %llu\n", UINT64_MAX);
    LOG_INFO_serializer("V2 =         %llu\n", V2);
    assert(V2 == UINT64_MAX);
    LOG_INFO_serializer("V3 = %G\n", V3);
    LOG_INFO_serializer("indexes = %zu\n", indexes);
    LOG_INFO_serializer("V4[0] = %zu\n", V4[0]);
    LOG_INFO_serializer("V4[1] = %zu\n", V4[1]);
    delete[] V4; // free pointer that was allocated on get_raw_pointer
    XX.free__(); // free unused vectors, normally we do not know if all vectors add's are matched by
    // get even though we should, for example, we mey preserve indexes for future use or we may exit
    // early before all vectors are getted, NOTE: this is called in the deconstructor aswell

    serializer ARG;
    int argc = 2;
    const char *argv[argc];
    argv[0] = "argv1";
    argv[1] = "argv2";
    ARG.add<int>(argc);
    ARG.add_pointer<const char>(argv[0], strlen(argv[0]) + 1); // copy the null aswell
    ARG.add_pointer<const char>(argv[1], strlen(argv[1]) + 1); // copy the null aswell
    ARG.construct();

    serializer ARG_COPY_raw_pointer;
    serializer ARG_COPY_vector;

    // transfer ARG to ARG_COPY
    ARG_COPY_raw_pointer.stream.allocate(ARG.stream.data_len);
    ARG_COPY_vector.stream.allocate(ARG.stream.data_len);
    memcpy(ARG_COPY_raw_pointer.stream.data, ARG.stream.data, ARG_COPY_raw_pointer.stream.data_len);
    memcpy(ARG_COPY_vector.stream.data, ARG.stream.data, ARG_COPY_vector.stream.data_len);
    ARG.stream.deallocate();

    // process raw pointer
    ARG_COPY_raw_pointer.deconstruct();
    int argc_raw_pointer;
    ARG_COPY_raw_pointer.get<int>(&argc_raw_pointer);
    char *argv_raw_pointer[argc_raw_pointer];
    ARG_COPY_raw_pointer.get_raw_pointer<char>(&argv_raw_pointer[0]);
    ARG_COPY_raw_pointer.get_raw_pointer<char>(&argv_raw_pointer[1]);
    LOG_INFO_serializer(
        "ARG_COPY_raw_pointer: argc_raw_pointer: %d, argv_raw_pointer[0]: %s, argv_raw_pointer[1]: %s\n",
        argc_raw_pointer, argv_raw_pointer[0], argv_raw_pointer[1]);
    // raw pointers must be deleted
    delete[] argv_raw_pointer[0];
    delete[] argv_raw_pointer[1];

    // process vector
    ARG_COPY_vector.deconstruct();
    int argc_vector;
    ARG_COPY_vector.get<int>(&argc_vector);
    std::vector<char> argv_vector[argc_vector];
    ARG_COPY_vector.get_vector_pointer<char>(argv_vector[0]);
    ARG_COPY_vector.get_vector_pointer<char>(argv_vector[1]);
    LOG_INFO_serializer(
        "ARG_COPY_vector: argc_vector: %d, argv_vector[0]: %s, argv_vector[1]: %s\n", argc_vector,
        argv_vector[0].data(), argv_vector[1].data());
    // vectors get deleted automatically
}


#endif //GLNE_SERIALIZER_H
