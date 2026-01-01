#include <iostream>

#include <fmt/core.h>

#include "prelude/prelude.hpp"

int main(int, char **) {

    std::vector<int> a = {1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 5};
    std::vector<int> b
        = a | rv::transform([](int x) { return x * x; }) | prelude::collect<std::vector>;

    auto zipped = rv::zip(a, b);

    static_assert(std::ranges::range<decltype(zipped)>);

    for (auto [a, b] : zipped) {
        fmt::print("{}, {}\n", a, b);
    }

    return 0;
}