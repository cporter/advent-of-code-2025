#include <ranges>
#include <vector>

#include <gtest/gtest.h>

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
    std::vector<int> v1 = {1, 2, 3, 4, 5, 0};
    auto v2 = std::views::iota(0) | rv::transform([](auto) { return -1; });

    auto z = prelude::zip(v0, v1, v2);

    static_assert(std::ranges::range<decltype(z)>);

    auto t0 = z | rv::transform([](const auto &tup) {
                  auto &[a, b, c] = tup;
                  return (a + b) * c;
              });

    static_assert(std::ranges::range<decltype(t0)>, "t0 must be a range");
    // static_assert(std::same_as<std::ranges::range_value_t<decltype(t0)>, int>,
    //               "t0 must be a range of ints");

    // auto t1 = t0 | rv::transform([](const auto &x) { return x == -5; });
    // auto good = t1 | prelude::reduce(true, std::logical_and<>{});

    // EXPECT_TRUE(good);
}