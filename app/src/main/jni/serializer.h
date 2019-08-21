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

#ifndef __ANDROID__
#define LOG_INFO printf
    #define LOG_ERROR printf
#else

#include <strings.h>
    #include <android/log.h>

    #define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif


struct serializer_data {
    int type_size;
    void *data;
    size_t data_len;
};

typedef std::deque<struct serializer_data> Serial;

class serializer {
    private:
        template<typename type, typename matches>
        bool add_if_matches(type *data, size_t data_len) {
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                matches *x = new matches[data_len];
                memcpy(x, data, sizeof(matches) * data_len);
                struct serializer_data s;
                s.type_size = ts;
                s.data = x;
                s.data_len = data_len;
                serial.push_back(s);
                return true;
            }
            return false;
        }

        template<typename type, typename matches>
        bool remove_if_matches(type *data) {
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data data_ = serial.front();
                serial.pop_front();
                memcpy(data, static_cast<matches *>(data_.data), sizeof(matches) * data_.data_len);
                return true;
            }
            return false;
        }

        template<typename type, typename matches>
        bool remove_pointer_if_matches(type **data) {
            int ts = sizeof(type);
            if (ts == sizeof(matches)) {
                struct serializer_data data_ = serial.front();
                serial.pop_front();
                *static_cast<type **>(data) = new type[data_.data_len];
                memcpy(*data, static_cast<type *>(data_.data), sizeof(type) * data_.data_len);
                return true;
            }
            return false;
        }

        Serial serial;
    public:
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
        bool get_pointer(TYPE **data) {
            if (remove_pointer_if_matches<TYPE, int8_t>(data)) return true;
            if (remove_pointer_if_matches<TYPE, int16_t>(data)) return true;
            if (remove_pointer_if_matches<TYPE, int32_t>(data)) return true;
            if (remove_pointer_if_matches<TYPE, int64_t>(data)) return true;
            return false;
        }
};

void ser() {
    serializer X;
    X.add<size_t>(5);
    X.add<size_t>(3);
    size_t size[2];
    X.get<size_t>(&size[0]);
    X.get<size_t>(&size[1]);
    LOG_INFO("X[0] is %zu, X[1] is %zu\n", size[0], size[1]);
    assert(size[0] == 5);
    assert(size[1] == 3);
    X.add_pointer<size_t>(size, 2);
    size_t *size_;
    X.get_pointer<size_t>(&size_);
    LOG_INFO("X[0] is %zu, X[1] is %zu\n", size_[0], size_[1]);
    assert(size_[0] == 5);
    assert(size_[1] == 3);
}

#endif //GLNE_SERIALIZER_H
