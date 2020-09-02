//
// Created by smallville7123 on 9/08/20.
//

#include <gtest/gtest.h>


// if ANYOPT_RUNTIME_ASSERTION is defined
// then AnyOpt uses runtime assertions instead of compile-time assertions

// AnyOpt defaults to compile-time assertions

// use runtime assertions to allow assertion testing using death tests
#define ANYOPT_RUNTIME_ASSERTION

#include <AnyOpt.h>

/*
class AnyOpt_Core : public ::testing::Test {
protected:
    AnyOpt a;
};

*/

TEST(AnyOpt_Core, initialization_no_data_checking) {
    AnyOpt a;
}

TEST(AnyOpt_Core, initialization_data_checking) {
    AnyOpt a;
    ASSERT_EQ(a.data, nullptr);
    ASSERT_EQ(a.data_is_allocated, false);
    ASSERT_EQ(a.isAnyNullOpt, false);
}

TEST(AnyOpt_Core, initialization_AnyNullOpt_no_data_checking) {
    AnyOpt a = AnyNullOpt;
}

TEST(AnyOpt_Core, initialization_AnyNullOpt_data_checking) {
    AnyOpt a = AnyNullOpt;
    ASSERT_EQ(a.data, nullptr);
    ASSERT_EQ(a.data_is_allocated, false);
    ASSERT_EQ(a.isAnyNullOpt, true);
}

TEST(AnyOpt_Flags_Copy, valid_copy_1) {
    const int z = 5;
    AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> a = z;
}

TEST(AnyOpt_Flags_Copy, valid_copy_2) {
    const int z = 5;
    const AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> a = z;
    AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> b = a;
}

TEST(AnyOpt_Flags_Copy, invalid_copy_1) {
    // this will invoke the move constructor
    ASSERT_DEATH(
            {
                 AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> a = 5;
             },
            AnyOpt_Catch_Flag_POSIX_REGEX(AnyOpt_FLAG_MOVE_ONLY)
     );
}

TEST(AnyOpt_Flags_Copy, invalid_copy_2) {
    // this will invoke the move constructor
    ASSERT_DEATH(
            {
                const int z = 5;
                AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> a = z;
                AnyOptCustomFlags<AnyOpt_FLAG_COPY_ONLY> b = a;
            },
            AnyOpt_Catch_Flag_POSIX_REGEX(AnyOpt_FLAG_MOVE_ONLY)
    );
}

TEST(AnyOpt_Flags_Move, valid_move) {
    // If other is an rvalue expression,
    // move constructor will be selected by overload resolution
    // and called during copy-initialization.
    // There is no such term as move-initialization.
    // THIS only applies if a USER-DEFINED move constructor is present
    AnyOptCustomFlags<AnyOpt_FLAG_MOVE_ONLY> a = 5;
}

TEST(AnyOpt_Flags_Pointer, valid_void_pointer_1) {
    void * x = nullptr;
    AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS> a = x;
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_1) {
    void * x = nullptr;
    const AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS|AnyOpt_FLAG_COPY_ONLY> a = x;
    AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS|AnyOpt_FLAG_COPY_ONLY> b = a;
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_1_fail_1) {
    AnyOpt_GTEST_ASSERT_DEATH(
            {
                void *x = nullptr;
                const AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS> a = x;
                AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS> b = a;
            },
            AnyOpt_FLAG_COPY_ONLY
    );
}

TEST(AnyOpt_Flags_Pointer, valid_void_pointer_2) {
    void * x = new int;
    AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS> a = AnyOptCustomFlags<AnyOpt_FLAG_ENABLE_POINTERS>(x, true);
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_2) {
    constexpr int flags =
            AnyOpt_FLAG_ENABLE_POINTERS|
            AnyOpt_FLAG_COPY_ONLY_AND_MOVE_ONLY|
            AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE;
    void * x = new int;
    const AnyOptCustomFlags<flags> a = AnyOptCustomFlags<flags>(x, true);
    AnyOptCustomFlags<flags> b = a;
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_2_fail_1) {
    AnyOpt_GTEST_ASSERT_DEATH(
        {
            constexpr int flags = AnyOpt_FLAG_ENABLE_POINTERS;
            void *x = new int;
            const AnyOptCustomFlags<flags> a = AnyOptCustomFlags<flags>(x, true);
            AnyOptCustomFlags<flags> b = a;
        },
        AnyOpt_FLAG_COPY_ONLY
    );
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_2_fail_2) {
    AnyOpt_GTEST_ASSERT_DEATH(
            {
                constexpr int flags = AnyOpt_FLAG_ENABLE_POINTERS|AnyOpt_FLAG_COPY_ONLY;
                void *x = new int;
                const AnyOptCustomFlags<flags> a = AnyOptCustomFlags<flags>(x, true);
                AnyOptCustomFlags<flags> b = a;
            },
            AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE
    );
}

TEST(AnyOpt_Flags_Pointer, copy_valid_void_pointer_2_fail_3) {
    AnyOpt_GTEST_ASSERT_DEATH(
            {
                constexpr int flags =
                        AnyOpt_FLAG_ENABLE_POINTERS|
                        AnyOpt_FLAG_COPY_ONLY|
                        AnyOpt_FLAG_ENABLE_CONVERSION_OF_ALLOCATION_COPY_TO_ALLOCATION_MOVE;
                void *x = new int;
                const AnyOptCustomFlags<flags> a = AnyOptCustomFlags<flags>(x, true);
                AnyOptCustomFlags<flags> b = a;
            },
            AnyOpt_FLAG_MOVE_ONLY
    );
}

TEST(AnyOpt_Core_Data, data_obtaining) {
    AnyOpt a = 5;
    ASSERT_EQ(a.get<int>(), 5);
    ASSERT_EQ(a.get<int*>()[0], 5);
    AnyOpt b = AnyOpt(new int(5), true);
    ASSERT_EQ(b.get<int>(), 5);
    ASSERT_EQ(b.get<int*>()[0], 5);
    void * n = nullptr;
    AnyOpt c = n;
    ASSERT_EQ(c.get<void*>(), nullptr);
}

TEST(AnyOpt_Core, equality_test) {
    ASSERT_EQ(AnyOpt(5).get<int>(), AnyOpt(5).get<int>());
}
