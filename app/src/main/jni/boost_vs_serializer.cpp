// welp Boost's archives ARE infact larger, and text vs binary archives do not seem to make any
// difference at all

/*
g++ -std=c++17 -O2 -Wall -pedantic -pthread -lboost_system -lboost_serialization main.cpp && ./a.out
single
stream length: 10
stream index:|   0|   1|   2|   3|   4|   5|   6|   7|   8|   9|
stream data :|   1|   1|   0|   0|   0|   0|   0|   0|   0|   1|
multiple
stream length: 30
stream index:|   0|   1|   2|   3|   4|   5|   6|   7|   8|   9|  10|  11|  12|  13|  14|  15|  16|  17|  18|  19|  20|  21|  22|  23|  24|  25|  26|  27|  28|  29|
stream data :|   1|   1|   0|   0|   0|   0|   0|   0|   0|   1|   1|   1|   0|   0|   0|   0|   0|   0|   0|   2|   1|   1|   0|   0|   0|   0|   0|   0|   0|   3|
array
stream length: 12
stream index:|   0|   1|   2|   3|   4|   5|   6|   7|   8|   9|  10|  11|
stream data :|   1|   3|   0|   0|   0|   0|   0|   0|   0|   1|   2|   3|
BOOST ARCHIVE single text
22 serialization::archive 17 1
30
BOOST ARCHIVE single binary
22 serialization::archive 17 1
30
BOOST ARCHIVE multiple text
22 serialization::archive 17 1 2 3
34
BOOST ARCHIVE multiple binary
22 serialization::archive 17 1 2 3
34
BOOST ARCHIVE array text
22 serialization::archive 17 3 1 2 3
36
BOOST ARCHIVE array binary
22 serialization::archive 17 3 1 2 3
36
*/

#define LOG_INFO_serializer printf
#define LOG_ERROR_serializer printf

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

        bool constructAndMerge(int8_t ** out, size_t * out_length) {
            size_t index = 0;
            size_t offset_index = 0;
            struct serializer_data first;
            bool first_set = false;
            while (in.size() != 0) {
                // type_size (1), data length (8), data (*)
                struct serializer_data data_ = in.front();
                in.pop_front();
                if (!first_set) {
                    memcpy(&first.type_size, &data_.type_size, sizeof(int8_t));
                    memcpy(&first.data_len, &data_.data_len, sizeof(size_t));
                    first_set = true;
                    stream.append(sizeof(int8_t) * 1);
                    stream.data[index++] = data_.type_size;
                    stream.append(sizeof(int8_t) * sizeof(size_t));
                    reinterpret_cast<size_t *>(stream.data + index)[0] = data_.data_len;
                    index += sizeof(size_t);
                    offset_index += index;
                }
                bool cont = true;
                bool first_pass = memcmp(&first.type_size, &data_.type_size, sizeof(int8_t)) == 0;
                if (!first_pass) {
                    LOG_ERROR_serializer(
                        "first pass failed: type size mismatch, expected %d, got %d",
                        first.type_size, data_.type_size
                    );
                    cont = false;
                }
                if (cont) {
                    bool second_pass = memcmp(&first.data_len, &data_.data_len, sizeof(size_t)) == 0;
                    if (!second_pass) {
                        LOG_ERROR_serializer(
                            "second pass failed: length mismatch, expected %zu, got %zu",
                            first.data_len, data_.data_len
                        );
                        cont = false;
                    }
                    if (cont) {
                        stream.append(data_.type_size * data_.data_len);
                        memcpy(&stream.data[index], data_.data, data_.type_size * data_.data_len);
                        index += data_.type_size * data_.data_len;
                    }
                }
                delete[] data_.data;
                if (!cont) return cont;
            }
            *out_length = index - offset_index;
            *out = new int8_t[first.type_size * (index - offset_index)];
            return true;
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
            for (size_t i = 0; i < stream.data_len; i++) {
                if (i < 10) si += "|   " + std::to_string(i);
                else if (i < 100) si += "|  " + std::to_string(i);
                else si += "| " + std::to_string(i);
            }
            si += "|";
            LOG_INFO_serializer("%s\n", si.c_str());
            std::string sd("stream data :");
            for (size_t i = 0; i < stream.data_len; i++) {
                if (stream.data[i] < 0) {
                    if (stream.data[i] > -10) sd += "|  " + std::to_string(stream.data[i]);
                    else if (stream.data[i] > -100) sd += "| " + std::to_string(stream.data[i]);
                    else if (stream.data[i] > -1000) sd += "|" + std::to_string(stream.data[i]);
                } else if (stream.data[i] < 10) sd += "|   " + std::to_string(stream.data[i]);
                else if (stream.data[i] < 100) sd += "|  " + std::to_string(stream.data[i]);
                else sd += "| " + std::to_string(stream.data[i]);
            }
            sd += "|";
            LOG_INFO_serializer("%s\n", sd.c_str());
        }
};

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <iostream>

struct A {
    int a;
    template <typename Ar> void serialize(Ar& ar, unsigned) {
        ar & a;
    }
};

BOOST_IS_BITWISE_SERIALIZABLE(A)

int main() {
    serializer single;
    single.add<int8_t>(1);
    single.construct();
    puts("single");
    single.info();

    serializer multiple;
    multiple.add<int8_t>(1);
    multiple.add<int8_t>(2);
    multiple.add<int8_t>(3);
    multiple.construct();
    puts("multiple");
    multiple.info();

    // optimize into an array
    serializer array;
    int8_t a[3] = { 1, 2, 3};
    array.add_pointer<int8_t>(a, 3);
    array.construct();
    puts("array");
    array.info();

    // construct BOOST archives

    std::ostringstream oss;
    std::ostringstream ossb;

    boost::archive::text_oarchive oa(oss);
    boost::archive::text_oarchive oab(ossb);

    oa << 1;
    oab << 1;

    puts("BOOST ARCHIVE single text");
    std::cout << oss.str() << std::endl;
    std::cout << oss.str().size() << std::endl;

    puts("BOOST ARCHIVE single binary");
    std::cout << ossb.str() << std::endl;
    std::cout << ossb.str().size() << std::endl;

    std::ostringstream ossM;
    std::ostringstream ossbM;

    boost::archive::text_oarchive oaM(ossM);
    boost::archive::text_oarchive oabM(ossbM);

    oaM << 1;
    oaM << 2;
    oaM << 3;
    oabM << 1;
    oabM << 2;
    oabM << 3;

    puts("BOOST ARCHIVE multiple text");
    std::cout << ossM.str() << std::endl;
    std::cout << ossM.str().size() << std::endl;

    puts("BOOST ARCHIVE multiple binary");
    std::cout << ossbM.str() << std::endl;
    std::cout << ossbM.str().size() << std::endl;

    std::ostringstream ossA;
    std::ostringstream ossbA;

    boost::archive::text_oarchive oaA(ossA);
    boost::archive::text_oarchive oabA(ossbA);

    oaA << a;
    oabA << a;

    puts("BOOST ARCHIVE array text");
    std::cout << ossA.str() << std::endl;
    std::cout << ossA.str().size() << std::endl;

    puts("BOOST ARCHIVE array binary");
    std::cout << ossbA.str() << std::endl;
    std::cout << ossbA.str().size() << std::endl;

    return 0;
}