#include <iostream>
#include <optional>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "prelude/prelude.hpp"

constexpr int LOCK_SIZE = 100;

int lock_mod(int x) { return (x % LOCK_SIZE + LOCK_SIZE) % LOCK_SIZE; }

int turn_from_string(const std::string &s) {
    int x = std::stoi(s.substr(1));

    switch (s[0]) {
    case 'R':
        return x;
    case 'L':
        return -x;
    default:
        throw std::invalid_argument(fmt::format("unrecognized prefix: {}", s[0]));
    };
}

// we only want numbers - (LOCK_SIZE-1) < x < (LOCK_SIZE-1),
// but before we mod our turns figure out how many full turns
// we go through otherwise for counting the times we pass zero.
int full_turns(int x) { return std::abs(x / 100); }

int main(int, char **) {
    auto turns = prelude::line_view(std::cin) | rv::transform(turn_from_string);

    int position = 50;
    int part1 = 0;
    int part2 = 0;
    for (auto x : turns) {
        part2 += full_turns(x);
        x %= LOCK_SIZE;
        if (position != 0 && (position + x > LOCK_SIZE || position + x < 0)) {
            ++part2;
        }
        position = lock_mod(position + x);
        if (position == 0) {
            ++part1;
        }
    }

    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part1 + part2);

    return 0;
}