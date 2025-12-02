#include <iostream>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "prelude/prelude.hpp"

using namespace std::literals;
namespace rv = std::ranges::views;

bool doubleseq(long n) {
    int digits = static_cast<int>(std::floor(std::log10(n))) + 1;

    if (digits % 2 != 0) {
        return false;
    }

    const int half = digits / 2;
    const long divisor = static_cast<long>(std::pow(10, half));

    const long first_half = n / divisor;
    const long second_half = n % divisor;

    return first_half == second_half;
}

struct num_pair {
    long a, b;
};

template <> struct fmt::formatter<num_pair> : fmt::formatter<std::string_view> {
    auto format(const num_pair &p, fmt::format_context &ctx) const {
        std::string s = fmt::format("{}<->{}", p.a, p.b);
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

int main(int, char **) {
    std::string line;
    std::getline(std::cin, line);
    auto inputs = std::views::all(line) | std::views::split(","sv) | rv::transform([](auto &&r) {
                      auto dash = std::ranges::find(r, '-');
                      auto a = std::string(std::ranges::begin(r), dash);
                      auto b = std::string(std::ranges::next(dash), std::ranges::end(r));
                      return num_pair{std::stol(a), std::stol(b)};
                  });

    long part1 = 0;
    for (auto &&p : inputs) {
        for (long i = p.a; i <= p.b; ++i) {
            if (doubleseq(i)) {
                part1 += i;
            }
        }
    }

    fmt::print("part 1: {}\n", part1);

    return 0;
}