#include <ranges>
#include <vector>

#include <fmt/core.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

TEST(PreludeTest, TestEnumerate) {
    std::vector<int> v = {5, 4, 3, 2, 1, 0};
    int count = 0;
    for (auto [a, b] : prelude::enumerate(v)) {
        ++count;
        std::cerr << a << " + " << b << " = " << a + b << "\n";
        EXPECT_EQ(5, a + b) << "a + b =/= 5: " << a << ", " << b;
    }
    EXPECT_EQ(count, v.size());
}

TEST(PreludeTest, TestZip) {
    std::vector<int> v0 = {5, 4, 3, 2, 1, 0};
    std::vector<int> v1 = {0, 1, 2, 3, 4, 5};
    auto v2 = std::views::iota(0) | rv::transform([](auto) { return -1; });

    auto good = prelude::zip(v0, v1, v2) | rv::transform([](const auto &tup) {
                    auto &[a, b, c] = tup;
                    return (a + b) * c;
                })
                | rv::transform([](auto x) { return x == -5; })
                | prelude::reduce(true, std::logical_and<>{});

    EXPECT_TRUE(good) << "hey what the heck?";
}
