#include <iostream>
#include <string.h>
#include <assert.h>
#include <Windows/Kernel/WindowsAPIKernel.h>

template <typename T> struct memory_array {
    T ** array = nullptr;
    uint64_t index_count = 0;
    
    bool allocation_increment() {
        if (index_count == UINT64_MAX) return false;
        if (array == nullptr) {
            array = new T*[1];
        } else {
            T ** array_tmp = new T*[index_count+1];
            memcpy(array_tmp, array, sizeof(T)*index_count);
            delete[] array;
            array = array_tmp;
        }
        index_count++;
        return true;
    }

    void allocation_decrement() {
        if (array == nullptr) return;
        if (index_count == 1) {
            delete[] array;
            array = nullptr;
        } else {
            T **array_tmp = new T*[index_count - 1];
            memcpy(array_tmp, array, sizeof(T) * index_count - 1);
            delete[] array;
            array = array_tmp;
        }
        index_count--;
    }
    
    void assign(const T * what) {
        assert(array != nullptr);
        array[index_count-1] = const_cast<T*>(what);
    }
};

struct font {
    memory_array<char> font;
    
    void add_font(const char * font_name) {
        font.allocation_increment();
        font.assign(font_name);
    }
    
    void find_font(const char * font_name) {
        
    }
    
    void remove_font(const char * font_name) {
        font.assign(nullptr);
        font.allocation_decrement();
    }
};

int main(int argc, char **argv) {
    std::cout << "Hello, world!" << std::endl;
    font f;
    f.add_font("hello");
    f.remove_font("hello");
    Kernel k;
    return 0;
}
