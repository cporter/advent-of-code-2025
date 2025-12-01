#include <iostream>
#include <optional>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

constexpr int LOCK_SIZE = 100;

int lock_mod(int x) { return (x % LOCK_SIZE + LOCK_SIZE) % LOCK_SIZE; }

std::optional<int> turn_from_string(const std::string &s) {
    int x = 0;
    try {
        x = std::stoi(s.substr(1));
    } catch (const std::invalid_argument &e) {
        spdlog::error("Non-number: {} ({})", s.substr(1), e.what());
        return std::nullopt;
    }
    switch (s[0]) {
    case 'R':
        return x;
    case 'L':
        return -x;
    default:
        spdlog::error("Unrecognizable prefix: {}", s[0]);
        return std::nullopt;
    };
}

int main(int, char **) {
    auto collectedOpt
        = prelude::collect_optional(prelude::line_view(std::cin) | rv::transform(turn_from_string));
    if (!collectedOpt) {
        spdlog::error("Malformed input");
        return -1;
    }

    auto turns = std::move(*collectedOpt);

    int position = 50;
    int part1 = 0;
    for (auto &x : turns) {
        position = lock_mod(position + x);
        if (position == 0) {
            ++part1;
        }
    }

    fmt::print("part 1: {}\n", part1);

    return 0;
}