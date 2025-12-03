#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

int main(int, char **) {
    int part1 = prelude::line_view(std::cin) | rv::transform([](const auto &line) {
                    auto nums = line | rv::transform([](const char ch) { return int(ch - '0'); })
                                | prelude::collect<std::vector>;
                    auto it = std::ranges::begin(nums);
                    auto end = std::ranges::end(nums);
                    auto stop = std::ranges::prev(end);
                    auto biggest = std::ranges::max_element(it, stop);
                    int second = *std::ranges::max_element(std::next(biggest), end);
                    return 10 * *biggest + second;
                })
                | prelude::sum;
    fmt::print("Part 1: {}\n", part1);
    return 0;
}