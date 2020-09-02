//
// Created by smallville7123 on 5/08/20.
//

#ifndef AnyOpt_H
#define AnyOpt_H
#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>

// if ANYOPT_RUNTIME_ASSERTION is defined
// then AnyOpt uses runtime assertions instead of compile-time assertions

#define RUNTIME_WARNING_(msg) fprintf(\
        stderr,\
        "--- WARNING TRIGGERED ---\n"\
        "--- LOCATION: %s:%d ---\n"\
        "--- FUNCTION: %s ---\n"\
        "--- MESSAGE BEGIN ---\n"\
        "%s\n"\
        "--- MESSAGE END ---\n"\
        , __FILE__, __LINE__, __PRETTY_FUNCTION__, msg\
    ); \
    fflush(stderr)

#define RUNTIME_WARNING(expr, msg) if (!(expr)) { \
    RUNTIME_WARNING_(msg); \
}

#define RUNTIME_WARNING_EXECUTE_ADDITIONAL_CODE(expr, msg, code_block) if (!(expr)) { \
    RUNTIME_WARNING_(msg); \
    code_block \
}

#define RUNTIME_ASSERTION_(msg) fprintf(\
        stderr,\
        "--- ASSERTION TRIGGERED ---\n"\
        "--- LOCATION: %s:%d ---\n"\
        "--- FUNCTION: %s ---\n"\
        "--- MESSAGE BEGIN ---\n"\
        "%s\n"\
        "--- MESSAGE END ---\n"\
        , __FILE__, __LINE__, __PRETTY_FUNCTION__, msg\
    ); \
    fflush(stderr); \
    abort()

#define RUNTIME_ASSERTION(expr, msg) if (!(expr)) { \
    RUNTIME_ASSERTION_(msg); \
}

#ifdef ANYOPT_RUNTIME_ASSERTION
#define ASSERT_CODE(expr, msg) RUNTIME_ASSERTION(expr, msg)
#define ensure_flag_enabled(FLAGS, flag, message)         ASSERT_CODE(flag_is_set(FLAGS, flag), \
message "\n" \
"you can set it by appending\n" \
"|" #flag "\n" \
"to the flag list:\n" \
"AnyOptCustomFlags<Your_Flags|" #flag "> Your_Variable_Name;" \
)
#else
#define ASSERT_CODE(expr, msg) static_assert(expr, msg)
#define ensure_flag_enabled(FLAGS, flag, message)         ASSERT_CODE(flag_is_set(FLAGS, flag), \
"\n" message "\n" \
"you can set it by appending\n" \
"|" #flag "\n" \
"to the flag list:\n" \
"AnyOptCustomFlags<Your_Flags|" #flag "> Your_Variable_Name;" \
)
#endif

#define AnyOpt_GTEST_ASSERT_DEATH(code_block, flag_that_must_be_enabled) \
    ASSERT_DEATH(code_block, #flag_that_must_be_enabled)


#define AnyOpt_Catch_Flag_POSIX_REGEX(FLAG_NAME) #FLAG_NAME

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
static constexpr int AnyOpt_FLAG_DEBUG = 1 << 7;

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

    template<typename T>
    class storage : public dummy {
    public:
        T* data = nullptr;

        bool pointer_is_allocated = false;
        bool is_pointer = false;

        template<class P = void, typename = typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage * clone_impl(P * unused1 = nullptr, P * unused2 = nullptr) const {
            bool A = std::is_same<typename std::remove_reference<T>::type, void>::value;
            RUNTIME_ASSERTION(
                    A,
                    "this function cannot be invoked for non void types"
            )
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage clone void pointer");
                printf(
                        "AnyOptCustomFlags::storage clone void pointer - is_pointer: %s\n",
                        is_pointer ? "true" : "false"
                );
                printf(
                        "AnyOptCustomFlags::storage clone void pointer - pointer_is_allocated: %s\n",
                        pointer_is_allocated ? "true" : "false"
                );
                fflush(stdout);
            }
            // if data is allocated, a double free will occur
            if (pointer_is_allocated) {
                ensure_flag_enabled(
                        FLAGS,
                        AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE,
                        "allocated void pointers CANNOT be copied and must be moved instead"
                        " however the ability to convert a data copy to data move has not been"
                        " granted"
                );
                ensure_flag_enabled(
                        FLAGS,
                        AnyOpt_FLAG_MOVE_ONLY,
                        "allocated void pointers CANNOT be copied and must be moved instead"
                        " however the ability to move data has not been granted"
                );
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts(
                            "storage is being moved "
                            "because it has been marked as allocated and the "
                            "AnyOpt_FLAG_MOVE_ONLY "
                            "and "
                            "AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE "
                            "flags are set, and void pointers cannot be copied"
                    );
                    fflush(stdout);
                }
                // allocated
                storage *A = new storage(data, pointer_is_allocated);
                const_cast<storage<T> *>(this)->data = nullptr;
                const_cast<storage<T> *>(this)->pointer_is_allocated = false;
                const_cast<storage<T> *>(this)->is_pointer = false;
                return A;
            } else {
                // not allocated
                return new storage(data, pointer_is_allocated);
            }
        }

        template<class P = void, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage * clone_impl(P * unused1 = nullptr) const {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage clone");
                fflush(stdout);
            }
            const T & data_ = *data;
            return new storage(data_);
        }

        virtual storage * clone() const {
            return clone_impl();
        }

        template<class P, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage(const P &x) {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_COPY_ONLY,
                    "this function is not allowed unless the copy flag is set"
            );
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage copy constructor");
                fflush(stdout);
            }
            if (data == nullptr) {
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags::storage allocating and assigning data");
                    fflush(stdout);
                }
                const_cast<storage<T>*>(this)->data = new T(x);
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags::storage allocated and assigned data");
                    fflush(stdout);
                }
            } else {
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags::storage assigning data");
                    fflush(stdout);
                }
                *const_cast<storage<T>*>(this)->data = x;
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags::storage assigned data");
                    fflush(stdout);
                }
            }
        }

        template<class P, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage(P &&x) {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_MOVE_ONLY,
                    "this function is not allowed unless the move flag is set"
            );
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage move constructor");
                fflush(stdout);
            }
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(std::move(x));
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
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage pointer constructor");
                fflush(stdout);
            }
            data = x;
            is_pointer = true;
            pointer_is_allocated = allocation;
        }

        template<class P, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage &operator=(const P &x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_COPY_ONLY,
                    "this assignment operator is not allowed unless the copy flag is set"
            );
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage copy assignment");
                fflush(stdout);
            }
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(x);
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
            } else
                *const_cast<storage<T>*>(this)->data = x;
            return *const_cast<storage<T>*>(this);
        }

        template<class P, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, P>::type>
        storage &operator=(P &&x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_MOVE_ONLY,
                    "this assignment operator is not allowed unless the move flag is set"
            );
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage move assignment");
                fflush(stdout);
            }
            if (data == nullptr) {
                const_cast<storage<T>*>(this)->data = new T(std::move(x));
                const_cast<storage<T>*>(this)->pointer_is_allocated = true;
            }
            else
                std::swap(*data, x);
            return *const_cast<storage<T>*>(this);
        }

        storage &operator=(T * &x)  {
            ensure_flag_enabled(
                    FLAGS,
                    AnyOpt_FLAG_ENABLE_POINTERS,
                    "this assignment operator is not allowed unless pointers are enabled"
            );
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage pointer assignment");
                fflush(stdout);
            }
            return *const_cast<storage<T>*>(this);
        }

        // this MUST be able to be called regardless of stored type
        void deallocate() const {
            // an allocation could be cast to void* in order to avoid calling destructors
            // upon deletion
            if (pointer_is_allocated) {
                if (is_pointer) {
                    if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                        puts("AnyOptCustomFlags::storage data an allocated pointer");
                        fflush(stdout);
                    }
                    if (data != nullptr) {
                        delete data;
                        const_cast<storage<T>*>(this)->data = nullptr;
                    } else {
                        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                            puts("AnyOptCustomFlags::storage data is an allocated pointer however it is assigned to nullptr, this is likely a bug");
                            fflush(stdout);
                        }
                    }
                } else {
                    if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                        puts("AnyOptCustomFlags::storage data is a synthetic allocated pointer");
                        fflush(stdout);
                    }
                    if (data != nullptr) {
                        delete data;
                        const_cast<storage<T>*>(this)->data = nullptr;
                    } else {
                        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                            puts("AnyOptCustomFlags::storage data is a synthetic allocated pointer however it is assigned to nullptr, this is likely a bug");
                            fflush(stdout);
                        }
                    }
                }
            } else {
                if (is_pointer) {
                    if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                        puts("AnyOptCustomFlags::storage data a pointer however it is not allocated");
                        fflush(stdout);
                    }
                } else {
                    if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                        puts("AnyOptCustomFlags::storage data a synthetic pointer however it is not allocated, this is a bug");
                        fflush(stdout);
                    }
                }
            }
        }

        ~storage() {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags::storage destructor ");
                fflush(stdout);
            }
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
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t copy assignment");
            fflush(stdout);
        }
    }

    AnyOptCustomFlags(AnyNullOpt_t && opt): isAnyNullOpt(true) {
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
            fflush(stdout);
        }
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

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    void store_copy(const T & what, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        bool A = std::is_same<typename std::remove_reference<T>::type, AnyOptCustomFlags>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const AnyOptCustomFlags>::value;
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            printf("AnyOptCustomFlags copy %s\n", type);
            fflush(stdout);
        }
        if (A || B) {
            const AnyOptCustomFlags * constobj = reinterpret_cast<const AnyOptCustomFlags*>(&what);
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
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts(
                            "AnyOptCustomFlags is being moved "
                            "because it has been marked as allocated and the "
                            "AnyOpt_FLAG_MOVE_ONLY "
                            "and "
                            "AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE "
                            "flags are set"
                    );
                    fflush(stdout);
                }
                store_move(*obj, type);
            } else {
                deallocate();
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags copying data");
                    fflush(stdout);
                }
                copy(reinterpret_cast<const AnyOptCustomFlags *>(&what));
                if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                    puts("AnyOptCustomFlags copied data");
                    fflush(stdout);
                }
            }
        } else {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags allocating and assigning data");
                fflush(stdout);
            }
            // use swap, move the data
            if (data == nullptr) {
                const_cast<AnyOptCustomFlags*>(this)->data = new storage<typename std::remove_reference<T>::type>(what);
            } else {
                *static_cast<storage<typename std::remove_reference<T>::type>*>(data) = what;
            }
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags allocated and assigned data");
                fflush(stdout);
            }
            const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
            const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = true;
        }
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    void store_move(T && what, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        bool A = std::is_same<typename std::remove_reference<T>::type, AnyOptCustomFlags>::value;
        bool B = std::is_same<typename std::remove_reference<T>::type, const AnyOptCustomFlags>::value;
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            printf("AnyOptCustomFlags move %s\n", type);
            fflush(stdout);
        }
        if (A || B) {
            deallocate();
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags moving data");
                fflush(stdout);
            }
            move(const_cast<AnyOptCustomFlags*>(reinterpret_cast<const AnyOptCustomFlags*>(&what)));
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags moved data");
                fflush(stdout);
            }
        } else {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags allocating and assigning data");
                fflush(stdout);
            }
            // use swap, move the data
            if (data == nullptr) {
                const_cast<AnyOptCustomFlags*>(this)->data = new storage<typename std::remove_reference<T>::type>(std::move(what));
            } else {
                *static_cast<storage<typename std::remove_reference<T>::type>*>(data) = std::move(what);
            }
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags allocated and assigned data");
                fflush(stdout);
            }
            const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
            const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = true;
        }
    }

    template<typename T>
    void store_pointer(T * what, bool allocated, const char * type) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this function is not allowed unless pointers are enabled"
        );
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            printf("AnyOptCustomFlags pointer %s\n", type);
            fflush(stdout);
        }

        // TODO; should pointers be reallocated if they are synthetic (allocated == false)?

        deallocate();
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags allocating and assigning data");
            fflush(stdout);
        }
        const_cast<AnyOptCustomFlags*>(this)->data = new storage<T>(what, allocated);
        const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = allocated;
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags allocated and assigned data");
            fflush(stdout);
        }
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = false;
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags(const T &what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this constructor is not allowed unless the copy flag is set"
        );
        store_copy(what, "constructor");
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags(T &&what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this constructor is not allowed unless the move flag is set"
        );
        store_move(what, "constructor");
    }

    template<typename T>
    AnyOptCustomFlags(T * what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this constructor is not allowed unless pointers are enabled is set"
        );
        store_pointer(what, false, "constructor");
    }

    template<typename T>
    AnyOptCustomFlags(T * what, bool allocation) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_ENABLE_POINTERS,
                "this constructor is not allowed unless pointers are enabled is set"
        );
        store_pointer(what, allocation, "constructor");
    }

    AnyOptCustomFlags(const AnyOptCustomFlags &what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this constructor is not allowed unless the copy flag is set"
        );
        store_copy(what, "constructor");
    }

    AnyOptCustomFlags(AnyOptCustomFlags &&what) {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this constructor is not allowed unless the move flag is set"
        );
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags move constructor");
            fflush(stdout);
        }
        move(&what);
    }

    // this MUST be able to be called regardless of stored type
    void deallocate() const {
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags deallocating data");
            fflush(stdout);
        }
        if (data != nullptr) {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags data is not nullptr");
                puts("AnyOptCustomFlags deleting data");
                fflush(stdout);
            }
            delete data;
            const_cast<AnyOptCustomFlags*>(this)->data = nullptr;
            const_cast<AnyOptCustomFlags*>(this)->data_is_allocated = false;
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags deleted data");
                fflush(stdout);
            }
        } else {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                puts("AnyOptCustomFlags data is nullptr, data has not been allocated or has already been deallocated");
                fflush(stdout);
            }
        }
        const_cast<AnyOptCustomFlags*>(this)->isAnyNullOpt = true;
    }

    AnyOptCustomFlags &operator=(const AnyOptCustomFlags & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        store_copy(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &operator=(const AnyNullOpt_t & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t copy assignment");
            fflush(stdout);
        }
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &operator=(AnyNullOpt_t && what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this assignment operator is not allowed unless the move flag is set"
        );
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t move assignment");
            fflush(stdout);
        }
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags &operator=(const T & what)  {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this assignment operator is not allowed unless the copy flag is set"
        );
        store_copy(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags &operator=(T &&what)  {
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
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t copy store");
            fflush(stdout);
        }
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    AnyOptCustomFlags &store(AnyNullOpt_t && what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_MOVE_ONLY,
                "this function is not allowed unless the move flag is set"
        );
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags AnyNullOpt_t move store");
            fflush(stdout);
        }
        deallocate();
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags &store(const T & what) const {
        ensure_flag_enabled(
                FLAGS,
                AnyOpt_FLAG_COPY_ONLY,
                "this function is not allowed unless the copy flag is set"
        );
        store_copy(what, "assignment");
        return *const_cast<AnyOptCustomFlags*>(this);
    }

    template<class T, typename = typename std::enable_if<!std::is_same<typename std::remove_reference<T>::type, void>::value, T>::type>
    AnyOptCustomFlags &store(T && what) const {
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


//    bool compare(const AnyOptCustomFlags &lhs, const AnyOptCustomFlags &rhs) const {
// TYPES ARE REQUIRED TO BE KNOWN
//        puts("AnyOptCustomFlags comparison");
//        fflush(stdout);
//        return
//            (lhs.data == rhs.data) &&
//            (lhs.isAnyNullOpt == rhs.isAnyNullOpt) &&
//            (lhs.data_is_allocated == rhs.data_is_allocated);
//    }
//
//    bool operator!=(const AnyOptCustomFlags &rhs) const {
// TYPES ARE REQUIRED TO BE KNOWN
//        return !compare(*this, rhs);
//    }
//
//    bool operator==(const AnyOptCustomFlags &rhs) const {
// TYPES ARE REQUIRED TO BE KNOWN
//        return compare(*this, rhs);
//    }
//
    AnyOptCustomFlags() {
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags constructor");
            fflush(stdout);
        }
    }
    
    ~AnyOptCustomFlags() {
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags destructor");
            fflush(stdout);
        }
        if (!isAnyNullOpt) deallocate();
    }

    template<class T = void, typename = typename std::enable_if<std::is_pointer<T>::value, T>::type>
    T get_impl(T * unused1 = nullptr, T * unused2 = nullptr) const {
        if (data == nullptr) {
            if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
                RUNTIME_WARNING_(
                    "trying to obtain data when no data is stored,"
                    " nullptr will be returned instead"
                );
            }
            return nullptr;
        }
        storage<typename std::remove_pointer<T>::type>* s = static_cast<storage<typename std::remove_pointer<T>::type>*>(data);
        return s->data;
    }

    template<class T = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T>::type>
    T get_impl(T * unused1 = nullptr) const {
        RUNTIME_ASSERTION(data != nullptr, "trying to obtain data when no data is stored");
        storage<typename std::remove_pointer<T>::type>* s = static_cast<storage<typename std::remove_pointer<T>::type>*>(data);
        return *s->data;
    }

    template <typename T> T get() const {
        if (flag_is_set(FLAGS, AnyOpt_FLAG_DEBUG)) {
            puts("AnyOptCustomFlags get");
            fflush(stdout);
        }
        return get_impl<T>();
    }
};

typedef AnyOptCustomFlags<AnyOpt_FLAGS_DEFAULT> AnyOpt;

typedef AnyOptCustomFlags<AnyOpt_FLAGS_DEFAULT|AnyOpt_FLAG_DEBUG> AnyOptDebug;

template <typename T> constexpr T AnyOpt_Cast(const AnyOpt & val)  {
    return val.get<T>();
}

#endif //AnyOpt_H
