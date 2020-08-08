//
// Created by smallville7123 on 5/08/20.
//

#ifndef ANDROIDCOMPOSITOR_AnyOpt_H
#define ANDROIDCOMPOSITOR_AnyOpt_H
#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>

// TODO: test flags

// https://stackoverflow.com/a/17431484 - How to write runnable tests of static_assert?

// https://stackoverflow.com/questions/31607928/is-it-possible-to-assert-does-not-compile-with-gtest
// -
// Just a random stupid idea:
// #define static_assert assert
// at the top of the test file. Ugly but maybe it's what you want.

// const AnyOpt a(new int, true);
// alternative to
// const AnyOpt a; a.store(new int, true);

static constexpr int AnyOpt_FLAG_COPY_ONLY = 1 << 0;
static constexpr int AnyOpt_FLAG_MOVE_ONLY = 1 << 1;
static constexpr int AnyOpt_FLAG_COPY_ONLY_AND_MOVE_ONLY = AnyOpt_FLAG_COPY_ONLY | AnyOpt_FLAG_MOVE_ONLY;
static constexpr int AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE = 1 << 2;
static constexpr int AnyOpt_FLAG_ENABLE_NON_POINTERS = 1 << 3;
static constexpr int AnyOpt_FLAG_ENABLE_OPTIONAL_VALUE = 1 << 4;
static constexpr int AnyOpt_FLAG_ENABLE_POINTERS = 1 << 5;
static constexpr int AnyOpt_FLAG_IS_ALLOCATED = 1 << 6;

static constexpr int AnyOpt_FLAGS_DEFAULT = AnyOpt_FLAG_COPY_ONLY_AND_MOVE_ONLY | \
                                 AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE | \
                                 AnyOpt_FLAG_ENABLE_OPTIONAL_VALUE | \
                                 AnyOpt_FLAG_ENABLE_POINTERS | \
                                 AnyOpt_FLAG_ENABLE_NON_POINTERS;

// most of the time, this is the function you want
constexpr bool flag_is_set(uint64_t flags, uint64_t flag) {
    return (flags & flag) != 0;
}

constexpr bool flag_is_not_set(uint64_t flags, uint64_t flag) {
    return !flag_is_set(flags, flag);
}

#define ensure_flag_enabled(FLAGS, flag, message)         static_assert(flag_is_set(FLAGS, flag), \
message "\n" \
"you can set it by appending\n" \
"|" #flag "\n" \
"to the flag list:\n" \
"AnyOptCustomFlags<Your_Flags|" #flag "> Your_Variable_Name;" \
)


#define enableFunctionIf(T, E) template<class T, typename = typename std::enable_if<E, T>::type>
#define enableFunctionIfFlagIsSet(T, FLAGS, FLAG) enableFunctionIf(T, (FLAGS & FLAG) == 0)
#define enableFunctionIfFlagIsNotSet(T, FLAGS, FLAG) enableFunctionIf(T, (FLAGS & FLAG) != 0)

class AnyNullOpt_t {
public:
    constexpr AnyNullOpt_t() {}
    constexpr AnyNullOpt_t(const AnyNullOpt_t &x) {}
    constexpr AnyNullOpt_t(AnyNullOpt_t &&x) {}

    AnyNullOpt_t &operator=(const AnyNullOpt_t &x)  {
        puts("AnyNullOpt_t copy assignment");
        fflush(stdout);
        return *const_cast<AnyNullOpt_t*>(this);
    }

    AnyNullOpt_t &operator=(AnyNullOpt_t &&x)  {
        puts("AnyNullOpt_t move assignment");
        fflush(stdout);
        return *const_cast<AnyNullOpt_t*>(this);
    }
};

static constexpr AnyNullOpt_t AnyNullOpt {};

template <int FLAGS> class AnyOptCustomFlags {
public:
    class dummy {
    public:
        virtual dummy *clone() const = 0;
        virtual ~dummy() = default;
    };
    template<typename T> class storage : public dummy {
    public:
        T* data = nullptr;

        bool pointer_is_allocated = false;
        bool is_pointer = false;

        virtual storage * clone() const {
            puts("AnyOptCustomFlags::storage clone");
            fflush(stdout);
            return new storage(*data);
        }

        storage(const T &x) {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_COPY_ONLY,
                    "this function is not allowed unless the copy flag is set"
            );
            puts("AnyOptCustomFlags::storage copy constructor");
            fflush(stdout);
            if (data == nullptr) {
                puts("AnyOptCustomFlags::storage allocating and assigning data");
                fflush(stdout);
                const_cast<storage<T>*>(this)->data = new T(x);
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
                puts("AnyOptCustomFlags::storage allocated and assigned data");
                fflush(stdout);
            } else {
                puts("AnyOptCustomFlags::storage assigning data");
                fflush(stdout);
                *const_cast<storage<T>*>(this)->data = x;
                puts("AnyOptCustomFlags::storage assigned data");
                fflush(stdout);
            }
        }

        storage(T &&x) {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_MOVE_ONLY,
                    "this function is not allowed unless the move flag is set"
            );
            puts("AnyOptCustomFlags::storage move constructor");
            fflush(stdout);
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(std::forward<T>(x));
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
            } else {
                std::swap(*data, x);
            }
        }

        storage(T * x, bool allocation) {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_ENABLE_POINTERS,
                    "this function is not allowed unless pointers are enabled"
            );
            puts("AnyOptCustomFlags::storage pointer constructor");
            fflush(stdout);
            data = x;
            is_pointer = true;
            pointer_is_allocated = allocation;
        }

        storage &operator=(const T &x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_COPY_ONLY,
                    "this assignment operator is not allowed unless the copy flag is set"
            );
            puts("AnyOptCustomFlags::storage copy assignment");
            fflush(stdout);
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(x);
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
            } else
                *const_cast<storage<T>*>(this)->data = x;
            return *const_cast<storage<T>*>(this);
        }

        storage &operator=(T &&x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_MOVE_ONLY,
                    "this assignment operator is not allowed unless the move flag is set"
            );
            puts("AnyOptCustomFlags::storage move assignment");
            fflush(stdout);
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(std::forward<T>(x));
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
            }
            else
                std::swap(*data, x);
            return *const_cast<AnyOptCustomFlags*>(this);
        }

        storage &operator=(T * &x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_ENABLE_POINTERS,
                    "this assignment operator is not allowed unless pointers are enabled"
            );
            puts("AnyOptCustomFlags::storage pointer assignment");
            fflush(stdout);
            return *const_cast<AnyOptCustomFlags*>(this);
        }

        void deallocate() const {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_ENABLE_POINTERS,
                    "this function is not allowed unless pointers are enabled"
            );
            if (pointer_is_allocated) {
                if (is_pointer) {
                    puts("AnyOptCustomFlags::storage data an allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        const_cast<storage<T>*>(this)->data = nullptr;
                    } else {
                        puts("AnyOptCustomFlags::storage data is an allocated pointer however it is assigned to nullptr, this is likely a bug");
                        fflush(stdout);
                    }
                } else {
                    puts("AnyOptCustomFlags::storage data is a synthetic allocated pointer");
                    fflush(stdout);
                    if (data != nullptr) {
                        delete data;
                        const_cast<storage<T>*>(this)->data = nullptr;
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

        ~storage() {
            puts("AnyOptCustomFlags::storage destructor ");
            fflush(stdout);
            deallocate();
        }
    };

    dummy *data = nullptr;
    bool isAnyNullOpt = false;
    bool data_is_allocated = false;

    bool has_value() const {
        return !isAnyNullOpt;
    }

    AnyOptCustomFlags(const AnyNullOpt_t & opt): isAnyNullOpt(true) {
        puts("AnyOptCustomFlags AnyNullOpt_t copy assignment");
        fflush(stdout);
    }

    AnyOptCustomFlags(AnyNullOpt_t && opt): isAnyNullOpt(true) {
        puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
        fflush(stdout);
    }

    void move(const AnyOptCustomFlags * obj) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        const_cast<AnyOptCustomFlags*>(this)->data = obj->data;
        const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = obj->data_is_allocated;
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = obj->isAnyNullOpt;
        const_cast<AnyOptCustomFlags*>(obj)->data = nullptr;
        const_cast<AnyOptCustomFlags*>(obj)->isAnyNullOpt = true;
        const_cast<AnyOptCustomFlags*>(obj)->data_is_allocated = false;
    }

    void copy(const AnyOptCustomFlags * obj) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        const_cast<AnyOptCustomFlags*>(this)->data = obj->data->clone();
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
        const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = false;
    }

    template<typename T> void store_move(T && what, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        bool A = std::is_same<typename std::remove_reference<T>::type, AnyOptCustomFlags>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const AnyOptCustomFlags>::value;
        printf("AnyOptCustomFlags move %s\n", type);
        fflush(stdout);
        if (A || B) {
            deallocate();
            puts("AnyOptCustomFlags moving data");
            fflush(stdout);
            move(const_cast<AnyOptCustomFlags*>(reinterpret_cast<const AnyOptCustomFlags*>(&what)));
            puts("AnyOptCustomFlags moved data");
            fflush(stdout);
        } else {
            puts("AnyOptCustomFlags allocating and assigning data");
            fflush(stdout);
            // use swap, move the data
            if (data == nullptr) {
                const_cast<AnyOptCustomFlags*>(this)->data = new storage<typename std::remove_reference<T>::type>(std::forward<T>(what));
            } else {
                *static_cast<storage<typename std::remove_reference<T>::type>*>(data) = std::forward<T>(what);
            }
            puts("AnyOptCustomFlags allocated and assigned data");
            fflush(stdout);
            const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
            const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = true;
        }
    }

    template<typename T> void store_copy(const T * what, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        printf("AnyOptCustomFlags copy %s\n", type);
        fflush(stdout);
        bool A = std::is_same<typename std::remove_reference<T>::type, AnyOptCustomFlags>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const AnyOptCustomFlags>::value;
        printf("AnyOptCustomFlags move %s\n", type);
        fflush(stdout);
        if (A || B) {
            const AnyOptCustomFlags * constobj = reinterpret_cast<const AnyOptCustomFlags*>(what);
            AnyOptCustomFlags * obj = const_cast<AnyOptCustomFlags*>(constobj);
            if (
                    obj->data &&
                    obj->data_is_allocated &&
                    flag_is_set(
                            FLAGS,
                            AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE
                    )
                    ) {
                ensure_flag_enabled(
                        FLAGS,
                        AnyOpt_FLAG_MOVE_ONLY,
                        "AnyOptCustomFlags is eligible for moving,"
                        " however it has not been granted the ability to move data"
                );
                puts(
                        "AnyOptCustomFlags is being moved "
                        "because it has been marked as allocated and the "
                        "AnyOpt_FLAG_MOVE_ONLY "
                        "and "
                        "AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE "
                        "flags are set"
                );
                fflush(stdout);
                store_move(*obj, type);
            }
        } else copy(what);
    }

    template<typename T> void store_pointer(T * what, bool allocated, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this function is not allowed unless pointers are enabled"
        );
        printf("AnyOptCustomFlags pointer %s\n", type);
        fflush(stdout);

        // TODO; should pointers be reallocated if they are synthetic (allocated == false)?

        deallocate();
        puts("AnyOptCustomFlags allocating and assigning data");
        fflush(stdout);
        const_cast<AnyOptCustomFlags*>(this)->data = new storage<T>(what, allocated);
        const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = allocated;
        puts("AnyOptCustomFlags allocated and assigned data");
        fflush(stdout);
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
    }

    template<typename T> AnyOptCustomFlags(const T &what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this constructor is not allowed unless the copy flag is set"
        );
        store_copy(&what, "constructor");
    }

    template<typename T> AnyOptCustomFlags(T &&what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this constructor is not allowed unless the move flag is set"
        );
        store_move(what, "constructor");
    }

    template<typename T> AnyOptCustomFlags(T * what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this constructor is not allowed unless pointers are enabled is set"
        );
        store_pointer(what, false, "constructor");
    }

    template<typename T> AnyOptCustomFlags(T * what, bool allocation) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this constructor is not allowed unless pointers are enabled is set"
        );
        store_pointer(what, allocation, "constructor");
    }

    /* Copy constructor */
    AnyOptCustomFlags(const AnyOptCustomFlags &what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this constructor is not allowed unless the copy flag is set"
        );
        store_copy(&what, "constructor");
    }

    /* Move constructor */
    AnyOptCustomFlags(AnyOptCustomFlags &&what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this constructor is not allowed unless the move flag is set"
        );
        puts("AnyOptCustomFlags move constructor");
        fflush(stdout);
        move(&what);
    }

    void deallocate() const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this function is not allowed unless pointers are enabled"
        );
        puts("AnyOptCustomFlags deallocating data");
        fflush(stdout);
        if (data != nullptr) {
            puts("AnyOptCustomFlags data is not nullptr");
            fflush(stdout);
            puts("AnyOptCustomFlags deleting data");
            fflush(stdout);
            delete data;
            const_cast<AnyOptCustomFlags*>(this)->data = nullptr;
            const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = false;
            puts("AnyOptCustomFlags deleted data");
            fflush(stdout);
        } else {
            puts("AnyOptCustomFlags data is nullptr, data has not been allocated or has already been deallocated");
            fflush(stdout);
        }
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = true;
    }

    AnyOptCustomFlags &operator=(const AnyOptCustomFlags & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        store_copy(&what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &operator=(const AnyNullOpt_t & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        puts("AnyOptCustomFlags AnyNullOpt_t copy assignment");
        fflush(stdout);
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &operator=(AnyNullOpt_t && what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this assignment operator is not allowed unless the move flag is set"
        );
        puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
        fflush(stdout);
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &operator=(const T & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        store_copy(&what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &operator=(T &&what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this assignment operator is not allowed unless the move flag is set"
        );
        store_move(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &operator=(T * what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this assignment operator is not allowed unless pointers are enabled"
        );
        store_pointer(what, false, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &store(const AnyNullOpt_t & what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        puts("AnyOptCustomFlags AnyNullOpt_t copy store");
        fflush(stdout);
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &store(AnyNullOpt_t && what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        puts("AnyOptCustomFlags AnyNullOpt_t move store");
        fflush(stdout);
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &store(const T & what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        store_copy(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &store(T && what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        // function accepting a forward reference
        store_move(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<typename T> AnyOptCustomFlags &store(T * what, bool allocated) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this function is not allowed unless pointers are enabled"
        );
        store_pointer(what, allocated, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
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

    template <typename T> T * get() const {
        if (data != nullptr) {
            return static_cast<storage<T>*>(data)->data;
        } else return nullptr;
    }
};

typedef AnyOptCustomFlags<AnyOpt_FLAGS_DEFAULT> AnyOpt;

template <typename T> constexpr T AnyOpt_Cast(const AnyOpt & val)  {
    return val.get<T>();
}

#endif //ANDROIDCOMPOSITOR_AnyOpt_H