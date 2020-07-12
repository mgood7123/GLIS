//
// Created by konek on 8/21/2019.
//

#pragma once

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
#include <string> // std::to_string

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
struct serializer_data {
    int8_t type_size;
    char *data;
    size_t data_len;
};



class serializer_stream {
    public:
        int8_t *data = nullptr;
        size_t data_len = 0;

        bool allocate(size_t len);

        bool resize(size_t len);

        bool deallocate();

        bool append(size_t len);

        bool retract(size_t len);

        serializer_stream retract_from_front(size_t len);

        ~serializer_stream();
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

        void construct();

        bool constructAndMerge(int8_t ** out, size_t * out_length);

        void deconstruct();

        void free__();

        ~serializer();

        void info();
};

void serializer_demo();
#pragma clang diagnostic pop