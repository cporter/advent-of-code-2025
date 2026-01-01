#include "prelude/prelude.hpp"

const std::string_view vowels = "aeiou"sv;
const std::array<std::string_view, 4> disallowed = {"ab"sv, "cd"sv, "pq"sv, "xy"sv};

bool isNice1(std::string_view s) {
    // no disallowed words
    if (0 < std::ranges::distance(disallowed | rv::filter([s](std::string_view x) {
                                      return s.find(x) != std::string_view::npos;
                                  }))) {
        return false;
    }
    // has at least 3 vowels
    if (3 > std::ranges::distance(
            s | rv::filter([](char x) { return vowels.find(x) != std::string_view::npos; }))) {
        return false;
    }
    // has at least one repeated char
    return 0 < std::ranges::distance(prelude::pairwise(s)
                                     | rv::filter([](auto p) { return p.first == p.second; }));
}

bool repeatSpaced(std::string_view s) {
    const int N = s.size();
    for (int i = 0; i < N - 2; ++i) {
        if (s[i] == s[2 + i]) {
            return true;
        }
    }
    return false;
}

bool doubleRepeater(std::string_view s) {
    const int N = s.size();
    for (int i = 0; i < N - 3; ++i) {
        std::string_view sub = s.substr(i, 2);
        std::string_view rest = s.substr(i + 2);
        if (rest.find(sub) != std::string_view::npos) {
            return true;
        }
    }
    return false;
}

bool isNice2(std::string_view s) { return repeatSpaced(s) && doubleRepeater(s); }

int main(int, char **) {
    int part1 = 0;
    int part2 = 0;
    prelude::line_view(std::cin) | prelude::for_each([&](std::string_view s) {
        if (isNice1(s)) {
            ++part1;
        }
        if (isNice2(s)) {
            ++part2;
        }
    });
    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);
    return 0;
}