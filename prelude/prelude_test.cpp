#include <vector>

#include <gtest/gtest.h>

#include "prelude/prelude.hpp"

TEST(PreludeTest, TestEnumerate) {
    std::vector<int> v = {5, 4, 3, 2, 1, 0};
    int count = 0;
    for (auto [a, b] : prelude::enumerate(v)) {
        ++count;
        EXPECT_EQ(5, a + b);
    }
    EXPECT_EQ(count, v.size());
}