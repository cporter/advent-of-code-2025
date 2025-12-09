#include <ranges>
#include <vector>

#include <fmt/core.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

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

TEST(PreludeTest, TestEnumerate) {
    std::vector<int> all5 = {5, 5, 5, 5, 5};
    int total = 0;
    for (auto [a, b] : prelude::enumerate(all5)) {
        total += a * b;
    }

    total = 0;
    for (auto [a, b] : all5 | prelude::enumerate) {
        total += a * b;
    }
    EXPECT_EQ(total, 5 + 10 + 15 + 20);
}

// TEST(PreludeTest, TestChunkBy) {
//     std::vector<std::string> words
//         = {"hello", "world", "", "second", "group", "", "and", "   three"};
// }
