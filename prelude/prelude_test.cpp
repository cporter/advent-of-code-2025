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

TEST(PreludeTest, TestPairwise) {
    std::vector<int> stuff = {1, 2, 3, 4, 5};
    std::vector<std::tuple<int, int>> expected{{1, 2}, {2, 3}, {3, 4}, {4, 5}};

    auto p = prelude::pairwise(stuff);
    int count = 0;
    for (auto [r, e] : prelude::zip(p, expected)) {
        ++count;
        EXPECT_EQ(r, e);
    }
    ASSERT_EQ(count, expected.size());
}

// TEST(PreludeTest, TestCombinations) {
//     std::vector<int> stuff = {1, 2, 3, 4};
//     std::vector<std::tuple<int, int>> expected{{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};
//     auto returned = prelude::combinations(stuff, 2) | prelude::collect<std::vector>;
//     std::ranges::sort(returned);
//     ASSERT_EQ(returned.size(), expected.size());
//     for (auto [r, e] : prelude::zip(returned, expected)) {
//         EXPECT_EQ(r, e);
//     }
// }

// TEST(PreludeTest, TestChunkBy) {
//     std::vector<std::string> words
//         = {"hello", "world", "", "second", "group", "", "and", "   three"};
// }
