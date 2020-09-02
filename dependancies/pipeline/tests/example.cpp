//
// Created by smallville7123 on 9/08/20.
//

#include <gtest/gtest.h>

class Example : public ::testing::Test {
protected:
    int a = 5;

    void test() {
        ASSERT_EQ(a, 5);
    }
};

TEST_F(Example, 1) {
    test();
}