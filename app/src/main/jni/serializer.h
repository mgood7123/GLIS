//
// Created by konek on 8/21/2019.
//

#ifndef GLNE_SERIALIZER_H
#define GLNE_SERIALIZER_H

#include <deque> // std::deque
#include <cassert> // assert
#include <stdio.h> // printf
#include <memory.h> // mem*
#include <inttypes.h>
#include <cerrno> // errno, errors
#include <stdlib.h> // malloc/realloc/free
#include <limits.h>

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
            if (data == nullptr) return allocate(len);
            if (data_len == len) return true;
            int8_t *data_tmp = static_cast<int8_t *>(realloc(data, len));
            if (data_tmp == NULL) if (errno == ENOMEM) return false;
            data = data_tmp;
            data_len = len;
            if (data_len == 0 && data != NULL) {
                free(data);
                data = nullptr;
            }
            return true;
        }

        bool deallocate() {
            return resize(0);
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
            resize(0);
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
            if (out.empty()) return 0;
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
        size_t remove_pointer_if_matches(type **data) {
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
    public:
        Serial in;
        struct serializer_stream stream;
        Serial out;
        template<typename TYPE>
        bool add(TYPE data) {
            return add_pointer<TYPE>(&data, 1);
        }

        template<typename TYPE>
        bool get(TYPE *data) {
            if (remove_if_matches<TYPE, int8_t>(data)) return true;
            if (remove_if_matches<TYPE, int16_t>(data)) return true;
            if (remove_if_matches<TYPE, int32_t>(data)) return true;
            if (remove_if_matches<TYPE, int64_t>(data)) return true;
            return false;
        }

        template<typename TYPE>
        bool add_pointer(TYPE *data, size_t data_len) {
            if (add_if_matches<TYPE, int8_t>(data, data_len)) return true;
            if (add_if_matches<TYPE, int16_t>(data, data_len)) return true;
            if (add_if_matches<TYPE, int32_t>(data, data_len)) return true;
            if (add_if_matches<TYPE, int64_t>(data, data_len)) return true;
            return false;
        }

        template<typename TYPE>
        size_t get_pointer(TYPE **data) {
            size_t s;
            s = remove_pointer_if_matches<TYPE, int8_t>(data);
            if (s != 0) return s;
            s = remove_pointer_if_matches<TYPE, int16_t>(data);
            if (s != 0) return s;
            s = remove_pointer_if_matches<TYPE, int32_t>(data);
            if (s != 0) return s;
            s = remove_pointer_if_matches<TYPE, int64_t>(data);
            if (s != 0) return s;
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
                if (data_.type_size == sizeof(int8_t)) {
                    stream.append(sizeof(int8_t) * sizeof(int8_t));
                    reinterpret_cast<int8_t *>(stream.data +
                                               index)[0] = static_cast<int8_t>(data_.data_len);
                    index += sizeof(int8_t);
                } else if (data_.type_size == sizeof(int16_t)) {
                    stream.append(sizeof(int8_t) * sizeof(int16_t));
                    reinterpret_cast<int16_t *>(stream.data +
                                                index)[0] = static_cast<int16_t>(data_.data_len);
                    index += sizeof(int16_t);
                } else if (data_.type_size == sizeof(int32_t)) {
                    stream.append(sizeof(int8_t) * sizeof(int32_t));
                    reinterpret_cast<int32_t *>(stream.data + index)[0] = data_.data_len;
                    index += sizeof(int32_t);
                } else if (data_.type_size == sizeof(int64_t)) {
                    stream.append(sizeof(int8_t) * sizeof(int64_t));
                    reinterpret_cast<int64_t *>(stream.data + index)[0] = data_.data_len;
                    index += sizeof(int64_t);
                }
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
                LOG_INFO_serializer("in.size() is %zu\n", in.size());
                LOG_INFO_serializer("in.empty() is %s\n", in.empty() ? "true" : "false");
                assert(in.size() != 0);
                assert(in.size() > 0);
                free(in.front().data);
                in.pop_front();
                LOG_INFO_serializer("in.empty() is %s\n", in.empty() ? "true" : "false");
            }
            while (!out.empty()) {
                LOG_INFO_serializer("out.size() is %zu\n", out.size());
                LOG_INFO_serializer("out.empty() is %s\n", out.empty() ? "true" : "false");
                assert(out.size() != 0);
                assert(out.size() > 0);
                free(out.front().data);
                out.pop_front();
                LOG_INFO_serializer("out.empty() is %s\n", out.empty() ? "true" : "false");
            }
        }

        ~serializer() {
            free__();
            // stream has its own deconstructor
        }
};

#endif //GLNE_SERIALIZER_H
