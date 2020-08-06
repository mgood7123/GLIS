//
// Created by smallville7123 on 5/08/20.
//

#ifndef ANDROIDCOMPOSITOR_WindowsAPIAny_H
#define ANDROIDCOMPOSITOR_WindowsAPIAny_H
#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>

class WindowsAPIAny {
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

        storage(T &x) : data(new T(x)), pointer_is_allocated(true) {
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
    bool data_is_allocated = false; // to auto move if data cannot be copied

    class NullOpt {};

    bool has_value() {
        return !isNullOpt;
    }

    WindowsAPIAny(NullOpt && opt): isNullOpt(true) {
        puts("WindowsAPIAny NullOpt move assignment");
        fflush(stdout);
    }

    void move(WindowsAPIAny * obj) {
        data = obj->data;
        data_is_allocated = obj->data_is_allocated;
        isNullOpt = obj->isNullOpt;
        obj->data = nullptr;
        obj->isNullOpt = true;
        obj->data_is_allocated = false;
    }

    template<typename T> WindowsAPIAny(T &&what) {
        if (std::is_same<typename std::remove_reference<T>::type, WindowsAPIAny>::value) {
            puts("WindowsAPIAny WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny allocating and assigning data");
            fflush(stdout);
            WindowsAPIAny * obj = reinterpret_cast<WindowsAPIAny*>(const_cast<WindowsAPIAny*>(&what));
            move(obj);
            puts("WindowsAPIAny allocated and assigned data");
            fflush(stdout);
        } else {
            puts("WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsAPIAny allocated and assigned data");
            fflush(stdout);
        }
    }

    template<typename T> WindowsAPIAny(T * what) {
        puts("WindowsAPIAny pointer assignment");
        fflush(stdout);
        puts("WindowsAPIAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, false);
        puts("WindowsAPIAny allocated and assigned data");
        fflush(stdout);
    }

    template<typename T> WindowsAPIAny(T * what, bool allocation) {
        puts("WindowsAPIAny pointer assignment");
        fflush(stdout);
        puts("WindowsAPIAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, allocation);
        data_is_allocated = allocation;
        puts("WindowsAPIAny allocated and assigned data");
        fflush(stdout);
    }

    /* Copy constructor */
    WindowsAPIAny(const WindowsAPIAny &what) {
        puts("WindowsAPIAny copy constructor");
        fflush(stdout);
        printf("WindowsAPIAny data_is_allocated is %s\n", data_is_allocated ? "true" : "false");
        fflush(stdout);
        printf("WindowsAPIAny what.data_is_allocated is %s\n", what.data_is_allocated ? "true" : "false");
        fflush(stdout);
        if (what.data != nullptr) {
            if (what.data_is_allocated) {
                puts("WindowsAPIAny needs to be moved because it has been marked as allocated");
                fflush(stdout);
                move(const_cast<WindowsAPIAny *>(&what));
            } else {
                data = what.data->clone();
            }
        }
    }

    /* Move constructor */
    WindowsAPIAny(WindowsAPIAny &&what) {
        puts("WindowsAPIAny move constructor");
        fflush(stdout);
        move(&what);
    }

    void deallocate() {
        puts("WindowsAPIAny deallocating data");
        fflush(stdout);
        if (data != nullptr) {
            puts("WindowsAPIAny data is not nullptr");
            fflush(stdout);
            puts("WindowsAPIAny deleting data");
            fflush(stdout);
            delete data;
            data = nullptr;
            data_is_allocated = false;
            puts("WindowsAPIAny deleted data");
            fflush(stdout);
        } else {
            puts("WindowsAPIAny data is nullptr, data has not been allocated or has already been deallocated");
            fflush(stdout);
        }
        isNullOpt = true;
    }

    WindowsAPIAny &operator=(const WindowsAPIAny & what) {
        puts("WindowsAPIAny WindowsAPIAny copy assignment");
        fflush(stdout);
        printf("WindowsAPIAny data_is_allocated is %s\n", data_is_allocated ? "true" : "false");
        fflush(stdout);
        printf("WindowsAPIAny what.data_is_allocated is %s\n", what.data_is_allocated ? "true" : "false");
        fflush(stdout);
        if (what.data != nullptr) {
            if (what.data_is_allocated) {
                puts("WindowsAPIAny needs to be moved because it has been marked as allocated");
                fflush(stdout);
                *this = std::move(what);
            } else {
                puts("WindowsAPIAny copying data");
                fflush(stdout);
                data = what.data->clone();
                puts("WindowsAPIAny copied data");
                fflush(stdout);
            }
        }
        return *this;
    }

    WindowsAPIAny &operator=(const NullOpt & what) {
        puts("WindowsAPIAny NullOpt copy assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    WindowsAPIAny &operator=(NullOpt && what) {
        puts("WindowsAPIAny NullOpt move assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> WindowsAPIAny &operator=(T &&what) {
        bool A = std::is_same<typename std::remove_reference<T>::type, WindowsAPIAny>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const WindowsAPIAny>::value;
        if (A || B) {
            puts("WindowsAPIAny WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny assigning data");
            fflush(stdout);
            WindowsAPIAny * obj = const_cast<WindowsAPIAny*>(&what);
            move(obj);
            puts("WindowsAPIAny assigned data");
            fflush(stdout);
        } else {
            puts("WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsAPIAny allocated and assigned data");
            fflush(stdout);
            isNullOpt = false;
            data_is_allocated = true;
        }
        return *this;
    }
    template<typename T> WindowsAPIAny &operator=(T * what) {
        puts("WindowsAPIAny pointer assignment");
        fflush(stdout);
        deallocate();
        puts("WindowsAPIAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, false);
        puts("WindowsAPIAny allocated and assigned data");
        fflush(stdout);
        isNullOpt = false;
        data_is_allocated = true;
        return *this;
    }

    WindowsAPIAny &store(NullOpt && what) {
        puts("WindowsAPIAny NullOpt move store");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> WindowsAPIAny &store(T && what) {
        if (std::is_same<typename std::remove_reference<T>::type, WindowsAPIAny>::value) {
            puts("WindowsAPIAny WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny assigning data");
            fflush(stdout);
            WindowsAPIAny * obj = reinterpret_cast<WindowsAPIAny*>(const_cast<WindowsAPIAny*>(reinterpret_cast<const WindowsAPIAny*>(&what)));
            move(obj);
            puts("WindowsAPIAny assigned data");
            fflush(stdout);
        } else {
            puts("WindowsAPIAny move assignment");
            fflush(stdout);
            puts("WindowsAPIAny allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("WindowsAPIAny allocated and assigned data");
            fflush(stdout);
            isNullOpt = false;
            data_is_allocated = true;
        }
        return *this;
    }

    template<typename T> WindowsAPIAny &store(T * what, bool allocated) {
        puts("WindowsAPIAny pointer store");
        fflush(stdout);
        deallocate();
        puts("WindowsAPIAny allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, allocated);
        data_is_allocated = allocated;
        puts("WindowsAPIAny allocated and assigned data");
        fflush(stdout);
        isNullOpt = false;
        return *this;
    }

    WindowsAPIAny() {
        puts("WindowsAPIAny constructor");
        fflush(stdout);
    }
    ~WindowsAPIAny() {
        puts("WindowsAPIAny destructor");
        fflush(stdout);
        printf("WindowsAPIAny isNullOpt is %s\n", isNullOpt ? "true" : "false");
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

#endif //ANDROIDCOMPOSITOR_WindowsAPIAny_H
