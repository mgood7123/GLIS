//
// Created by smallville7123 on 5/08/20.
//

#ifndef ANDROIDCOMPOSITOR_AnyOpt_H
#define ANDROIDCOMPOSITOR_AnyOpt_H
#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>

#define AnyOpt_FLAG_COPY_ONLY 1 << 0
#define AnyOpt_FLAG_MOVE_ONLY 1 << 1
#define AnyOpt_FLAG_COPY_OR_MOVE 1 << 2
#define AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE 1 << 3
#define AnyOpt_FLAG_ENABLE_OPTIONAL_VALUE 1 << 4
#define AnyOpt_FLAGS_DEFAULT AnyOpt_FLAG_COPY_OR_MOVE | \
                                 AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE | \
                                 AnyOpt_FLAG_ENABLE_OPTIONAL_VALUE

class AnyNullOpt_t {};

constexpr AnyNullOpt_t AnyNullOpt {};

template <int FLAGS> class AnyOptCustomFlags {
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
            puts("AnyOptCustomFlags::storage clone");
            fflush(stdout);
            return is_pointer ? new storage(data, pointer_is_allocated) : new storage(*data);
        }

        storage(T &x) : data(new T(x)), pointer_is_allocated(true) {
            puts("AnyOptCustomFlags::storage copy constructor");
            fflush(stdout);
        }

        storage(T &&x) : data(new T(std::forward<T>(x))), pointer_is_allocated(true) {
            puts("AnyOptCustomFlags::storage move constructor ");
            fflush(stdout);
        }

        storage(T * x, bool allocation) : data(x), is_pointer(true), pointer_is_allocated(allocation) {
            puts("AnyOptCustomFlags::storage pointer constructor");
            fflush(stdout);
        }

        ~storage() {
            puts("AnyOptCustomFlags::storage destructor ");
            fflush(stdout);
            if (pointer_is_allocated) {
                if (is_pointer) {
                    puts("AnyOptCustomFlags::storage data an allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        data = nullptr;
                    } else {
                        puts("AnyOptCustomFlags::storage data is an allocated pointer however it is assigned to nullptr, this is likely a bug");
                        fflush(stdout);
                    }
                } else {
                    puts("AnyOptCustomFlags::storage data is a synthetic allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        data = nullptr;
                    } else {
                        puts("AnyOptCustomFlags::storage data is a synthetic allocated pointer however it is assigned to nullptr, this is likely a bug");
                        fflush(stdout);
                    }
                }
            } else {
                if (is_pointer) {
                    puts("AnyOptCustomFlags::storage data a pointer however it is not allocated");
                    fflush(stdout);
                } else {
                    puts("AnyOptCustomFlags::storage data a synthetic pointer however it is not allocated, this is a bug");
                    fflush(stdout);
                }
            }
        }
    };

    dummy *data = nullptr;
    bool isAnyNullOpt = false;
    bool data_is_allocated = false; // to auto move if data cannot be copied

    bool has_value() {
        return !isAnyNullOpt;
    }

    AnyOptCustomFlags(AnyNullOpt_t && opt): isAnyNullOpt(true) {
        puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
        fflush(stdout);
    }

    void move(AnyOptCustomFlags * obj) {
        data = obj->data;
        data_is_allocated = obj->data_is_allocated;
        isAnyNullOpt = obj->isAnyNullOpt;
        obj->data = nullptr;
        obj->isAnyNullOpt = true;
        obj->data_is_allocated = false;
    }

    void copy(const AnyOptCustomFlags * obj) {
        data = obj->data->clone();
    }

    template<typename T> void store_move(T && what, const char * type) {
        bool A = std::is_same<typename std::remove_reference<T>::type, AnyOptCustomFlags>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const AnyOptCustomFlags>::value;
        printf("AnyOptCustomFlags move %s\n", type);
        fflush(stdout);
        if (A || B) {
            puts("AnyOptCustomFlags moving data");
            fflush(stdout);
            move(const_cast<AnyOptCustomFlags*>(reinterpret_cast<const AnyOptCustomFlags*>(&what)));
            puts("AnyOptCustomFlags moved data");
            fflush(stdout);
        } else {
            puts("AnyOptCustomFlags allocating and assigning data");
            fflush(stdout);
            data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            puts("AnyOptCustomFlags allocated and assigned data");
            fflush(stdout);
            isAnyNullOpt = false;
            data_is_allocated = true;
        }
    }

    template<typename T> void store_copy(const T * what, const char * type) {
        printf("AnyOptCustomFlags copy %s\n", type);
        fflush(stdout);
        if (what->data != nullptr) {
            if (what->data_is_allocated) {
                puts("AnyOptCustomFlags needs to be moved because it has been marked as allocated");
                fflush(stdout);
                store_move(*what, type);
            } else {
                copy(what);
            }
        }
    }

    template<typename T> void store_pointer(T * what, bool allocated, const char * type) {
        printf("AnyOptCustomFlags pointer %s\n", type);
        fflush(stdout);
        deallocate();
        puts("AnyOptCustomFlags allocating and assigning data");
        fflush(stdout);
        data = new storage<T>(what, allocated);
        data_is_allocated = allocated;
        puts("AnyOptCustomFlags allocated and assigned data");
        fflush(stdout);
        isAnyNullOpt = false;
    }

    template<typename T> AnyOptCustomFlags(T &&what) {
        store_move(what, "constructor");
    }

    template<typename T> AnyOptCustomFlags(T * what) {
        store_pointer(what, false, "constructor");
    }

    template<typename T> AnyOptCustomFlags(T * what, bool allocation) {
        store_pointer(what, allocation, "constructor");
    }

    /* Copy constructor */
    AnyOptCustomFlags(const AnyOptCustomFlags &what) {
        store_copy(&what, "constructor");
    }

    /* Move constructor */
    AnyOptCustomFlags(AnyOptCustomFlags &&what) {
        puts("AnyOptCustomFlags move constructor");
        fflush(stdout);
        move(&what);
    }

    void deallocate() {
        puts("AnyOptCustomFlags deallocating data");
        fflush(stdout);
        if (data != nullptr) {
            puts("AnyOptCustomFlags data is not nullptr");
            fflush(stdout);
            puts("AnyOptCustomFlags deleting data");
            fflush(stdout);
            delete data;
            data = nullptr;
            data_is_allocated = false;
            puts("AnyOptCustomFlags deleted data");
            fflush(stdout);
        } else {
            puts("AnyOptCustomFlags data is nullptr, data has not been allocated or has already been deallocated");
            fflush(stdout);
        }
        isAnyNullOpt = true;
    }

    AnyOptCustomFlags &operator=(const AnyOptCustomFlags & what) {
        store_copy(&what, "assignment");
        return *this;
    }

    AnyOptCustomFlags &operator=(const AnyNullOpt_t & what) {
        puts("AnyOptCustomFlags AnyNullOpt_t copy assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    AnyOptCustomFlags &operator=(AnyNullOpt_t && what) {
        puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> AnyOptCustomFlags &operator=(T &&what) {
        store_move(what, "assignment");
        return *this;
    }

    template<typename T> AnyOptCustomFlags &operator=(T * what) {
        store_pointer(what, false, "assignment");
        return *this;
    }

    AnyOptCustomFlags &store(AnyNullOpt_t && what) {
        puts("AnyOptCustomFlags AnyNullOpt_t move store");
        fflush(stdout);
        deallocate();
        return *this;
    }

    template<typename T> AnyOptCustomFlags &store(T && what) {
        store_move(what, "assignment");
        return *this;
    }

    template<typename T> AnyOptCustomFlags &store(T * what, bool allocated) {
        store_pointer(what, allocated, "assignment");
        return *this;
    }

    AnyOptCustomFlags() {
        puts("AnyOptCustomFlags constructor");
        fflush(stdout);
    }
    ~AnyOptCustomFlags() {
        puts("AnyOptCustomFlags destructor");
        fflush(stdout);
        printf("AnyOptCustomFlags isAnyNullOpt is %s\n", isAnyNullOpt ? "true" : "false");
        fflush(stdout);
        if (!isAnyNullOpt) deallocate();
    }

    template <typename T> T * get() {
        if (data != nullptr) {
            storage<T> * s = static_cast<storage<T>*>(data);
            return s->data;
        } else return nullptr;
    }
};

#define AnyOpt AnyOptCustomFlags<AnyOpt_FLAGS_DEFAULT>

#endif //ANDROIDCOMPOSITOR_AnyOpt_H
