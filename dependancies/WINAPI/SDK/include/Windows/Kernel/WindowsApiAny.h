//
// Created by smallville7123 on 5/08/20.
//

#ifndef ANDROIDCOMPOSITOR_WINDOWSAPIANY_H
#define ANDROIDCOMPOSITOR_WINDOWSAPIANY_H
#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>

class WindowsApiAny {
public:
    class dummy {
    public:
        virtual dummy *clone() const = 0;
        virtual ~dummy() = default;
    };
    template<typename T> class storage : public dummy {
    public:
        T* data;

        bool pointer_is_allocated = false;
        bool is_pointer = false;

        virtual storage * clone() const {
            puts("storage clone");
            fflush(stdout);
            return is_pointer ? new storage(data, pointer_is_allocated) : new storage(*data);
        }

        storage(const T &x) : data(new T(x)), pointer_is_allocated(true) {
            puts("storage copy constructor");
            fflush(stdout);
        }

        storage(T &&x) : data(new T(std::forward<T>(x))), pointer_is_allocated(true) {
            puts("storage move constructor ");
            fflush(stdout);
        }

        storage(T * x, bool allocation) : data(x), is_pointer(true), pointer_is_allocated(allocation) {
            puts("storage pointer constructor");
            fflush(stdout);
        }

        ~storage() {
            puts("storage destructor ");
            fflush(stdout);
            if (pointer_is_allocated) {
                if (is_pointer) {
                    puts("storage data an allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        data = nullptr;
                    } else {
                        puts("storage data is an allocated pointer however it is assigned to nullptr, this is likely a bug");
                        fflush(stdout);
                    }
                } else {
                    puts("storage data is a synthetic allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        data = nullptr;
                    } else {
                        puts("storage data is a synthetic allocated pointer however it is assigned to nullptr, this is likely a bug");
                        fflush(stdout);
                    }
                }
            } else {
                if (is_pointer) {
                    puts("storage data a pointer however it is not allocated");
                    fflush(stdout);
                } else {
                    puts("storage data a synthetic pointer however it is not allocated, this is a bug");
                    fflush(stdout);
                }
            }
        }
    };

    dummy *data = nullptr;
    bool isNullOpt = false;

    class NullOpt {};

    bool has_value() {
        return isNullOpt == false;
    }

    WindowsApiAny(NullOpt && opt): isNullOpt(true) {
        puts("WindowsApiAny NullOpt move assignment constructor");
        fflush(stdout);
    }

    template<typename T> WindowsApiAny(T &&what) {
        if (std::is_same<typename std::remove_reference<T>::type, WindowsApiAny>::value) {
            puts("WindowsApiAny WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny allocating and assigning data");
            fflush(stdout);
            WindowsApiAny * obj = reinterpret_cast<WindowsApiAny*>(&what);
            if (obj->data != nullptr) {
                data = obj->data;
                obj->data = nullptr;
                obj->isNullOpt = true;
            }
            puts("WindowsApiAny allocated and assigned data");
            fflush(stdout);
        } else {
            puts("WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsApiAny allocated and assigned data");
            fflush(stdout);
        }
    }

    template<typename T> WindowsApiAny(T * what) {
        puts("WindowsApiAny pointer assignment constructor");
        fflush(stdout);
        puts("WindowsApiAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, false);
        puts("WindowsApiAny allocated and assigned data");
        fflush(stdout);
    }

    /* Copy constructor */
    WindowsApiAny(const WindowsApiAny &p2) {
        puts("WindowsApiAny copy constructor");
        fflush(stdout);
        if (p2.data != nullptr)
            data = p2.data->clone();
    }

    /* Move constructor */
    WindowsApiAny(WindowsApiAny &&p2) {
        puts("WindowsApiAny move constructor");
        fflush(stdout);
        data = p2.data; // this is moving the pointer, so this should not affect the data pointed to
        p2.data = nullptr;
    }

    void deallocate() {
        puts("WindowsApiAny deallocating data");
        fflush(stdout);
        if (data != nullptr) {
            puts("WindowsApiAny data is not nullptr");
            fflush(stdout);
            puts("WindowsApiAny deleting data");
            fflush(stdout);
            delete data;
            data = nullptr;
            puts("WindowsApiAny deleted data");
            fflush(stdout);
        } else {
            puts("WindowsApiAny data is nullptr, data has not been allocated or has already been deallocated");
            fflush(stdout);
        }
        isNullOpt = true;
    }

    WindowsApiAny &operator=(const WindowsApiAny & what) {
        puts("WindowsApiAny WindowsApiAny copy assignment constructor");
        fflush(stdout);
        puts("WindowsApiAny allocating and assigning data");
        fflush(stdout);
        if (what.data != nullptr)
            data = what.data->clone();
        puts("WindowsApiAny allocated and assigned data");
        fflush(stdout);
        return *this;
    }

    WindowsApiAny &operator=(const NullOpt & what) {
        puts("WindowsApiAny NullOpt copy assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    WindowsApiAny &operator=(NullOpt && what) {
        puts("WindowsApiAny NullOpt move assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> WindowsApiAny &operator=(T &&what) {
        if (std::is_same<typename std::remove_reference<T>::type, WindowsApiAny>::value) {
            puts("WindowsApiAny WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny assigning data");
            fflush(stdout);
            WindowsApiAny * obj = reinterpret_cast<WindowsApiAny*>(&what);
            if (obj->data != nullptr) {
                data = obj->data;
                obj->data = nullptr;
                obj->isNullOpt = true;
                isNullOpt = false;
            } else {
                isNullOpt = true;
            }
            puts("WindowsApiAny assigned data");
            fflush(stdout);
        } else {
            puts("WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsApiAny allocated and assigned data");
            fflush(stdout);
            isNullOpt = false;
        }
        return *this;
    }
    template<typename T> WindowsApiAny &operator=(T * what) {
        puts("WindowsApiAny pointer assignment");
        fflush(stdout);
        deallocate();
        puts("WindowsApiAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, false);
        puts("WindowsApiAny allocated and assigned data");
        fflush(stdout);
        isNullOpt = false;
        return *this;
    }

    WindowsApiAny &store(NullOpt && what) {
        puts("WindowsApiAny NullOpt move store");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> WindowsApiAny &store(T && what) {
        if (std::is_same<typename std::remove_reference<T>::type, WindowsApiAny>::value) {
            puts("WindowsApiAny WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny assigning data");
            fflush(stdout);
            WindowsApiAny * obj = reinterpret_cast<WindowsApiAny*>(&what);
            if (obj->data != nullptr) {
                data = obj->data;
                obj->data = nullptr;
                obj->isNullOpt = true;
                isNullOpt = false;
            } else {
                isNullOpt = true;
            }
            puts("WindowsApiAny assigned data");
            fflush(stdout);
        } else {
            puts("WindowsApiAny move assignment constructor");
            fflush(stdout);
            puts("WindowsApiAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsApiAny allocated and assigned data");
            fflush(stdout);
            isNullOpt = false;
        }
        return *this;
    }

    template<typename T> WindowsApiAny &store(T * what, bool allocated) {
        puts("WindowsApiAny pointer store");
        fflush(stdout);
        deallocate();
        puts("WindowsApiAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, allocated);
        puts("WindowsApiAny allocated and assigned data");
        fflush(stdout);
        isNullOpt = false;
        return *this;
    }

    WindowsApiAny() {
        puts("WindowsApiAny constructor");
        fflush(stdout);
    }
    ~WindowsApiAny() {
        puts("WindowsApiAny destructor");
        fflush(stdout);
        printf("WindowsApiAny isNullOpt is %s\n", isNullOpt ? "true" : "false");
        fflush(stdout);
        if (!isNullOpt) deallocate();
    }

    template <typename T> T * get() {
        if (data != nullptr) {
            storage<T> * s = static_cast<storage<T>*>(data);
            return s->data;
        } else return nullptr;
    }
};

#endif //ANDROIDCOMPOSITOR_WINDOWSAPIANY_H
