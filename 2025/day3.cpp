#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

int part1_joltage(const std::vector<int> &nums) {
    auto it = std::ranges::begin(nums);
    auto end = std::ranges::end(nums);
    auto stop = std::ranges::prev(end);
    auto biggest = std::ranges::max_element(it, stop);
    int second = *std::ranges::max_element(std::next(biggest), end);
    return 10 * *biggest + second;
}

long par2_largest_subsequence(const std::vector<int> &v, int k) {
    int n = v.size();
    int to_remove = n - k;

    std::vector<int> result;
    result.reserve(k);

    for (int d : v) {
        while (!result.empty() && to_remove > 0 && result.back() < d) {
            result.pop_back();
            --to_remove;
        }
        result.push_back(d);
    }

    result.resize(k);

    // Convert to long
    return result | prelude::reduce(0l, [](long a, int b) { return 10 * a + b; });
}

int main(int, char **) {
    auto data = prelude::line_view(std::cin) | rv::transform([](const auto &line) {
                    return line | rv::transform([](const char ch) { return int(ch - '0'); })
                           | prelude::collect<std::vector>;
                })
                | prelude::collect<std::vector>;
    int part1 = data | rv::transform(part1_joltage) | prelude::sum;
    long part2 = data | rv::transform([](const auto &v) { return par2_largest_subsequence(v, 12); })
                 | prelude::sum;

    fmt::print("Part 1: {}\n", part1);
    fmt::print("Part 2: {}\n", part2);
    return 0;
}