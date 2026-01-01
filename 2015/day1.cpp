#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

int part1(const std::string &line) {
    return line | rv::transform([](const char ch) {
               switch (ch) {
               case '(':
                   return 1;
               case ')':
                   return -1;
               default:
                   return 0;
               }
           })
           | prelude::sum;
}

int part2(const std::string &line) {
    int state = 0;
    for (auto [idx, ch] : rv::enumerate(line)) {
        switch (ch) {
        case '(':
            ++state;
            break;
        case ')':
            --state;
            break;
        default:
            break;
        }
        if (-1 == state) {
            return 1 + idx;
        }
    }
    spdlog::error("We never reach the basement. Wat.");
    return -1;
}

int main(int, char **) {
    auto input = prelude::front(prelude::line_view(std::cin));
    auto p1 = part1(input);
    fmt::print("part 1: {}\n", p1);
    auto p2 = part2(input);
    fmt::print("part 2: {}\n", p2);
    return 0;
}