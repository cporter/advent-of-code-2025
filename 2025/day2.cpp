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

bool repeatedDigits(long n, int digits) {
    const long divisor = static_cast<long>(std::pow(10, digits));
    const long orig = n % divisor;
    if (orig < 1) {
        return false;
    }
    long accum = orig;
    while (accum < n) {
        accum = accum * divisor + orig;
    }
    return accum == n;
}

bool repeatedDigits(long n) {
    int digits = static_cast<int>(std::floor(std::log10(n))) + 1;
    const int half = digits / 2;

    for (int k = 1; k <= half; ++k) {
        if (digits % k == 0) {
            if (repeatedDigits(n, k)) {
                return true;
            }
        }
    }
    return false;
}

int main(int, char **) {
    std::string line;
    std::getline(std::cin, line);
    auto inputs = std::views::all(line) | std::views::split(","sv) | rv::transform([](auto &&r) {
                      auto dash = std::ranges::find(r, '-');
                      auto a = std::string(std::ranges::begin(r), dash);
                      auto b = std::string(std::ranges::next(dash), std::ranges::end(r));
                      return std::make_pair(std::stol(a), std::stol(b));
                  });

    long part1 = 0;
    long part2 = 0;
    for (auto &&p : inputs) {
        for (long i = p.first; i <= p.second; ++i) {
            if (doubleseq(i)) {
                part1 += i;
            }
            if (repeatedDigits(i)) {
                part2 += i;
            }
        }
    }

    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);

    return 0;
}