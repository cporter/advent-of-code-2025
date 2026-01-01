#include "prelude/prelude.hpp"

long largest_subsequence(const std::vector<int> &v, int k) {
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

    return result | prelude::reduce(0l, [](long a, int b) { return 10 * a + b; });
}

int main(int, char **) {
    auto data = prelude::line_view(std::cin) | rv::transform([](const auto &line) {
                    return line | rv::transform([](const char ch) { return int(ch - '0'); })
                           | prelude::collect<std::vector>;
                })
                | prelude::collect<std::vector>;

    long part1 = data | rv::transform([](const auto &v) { return largest_subsequence(v, 2); })
                 | prelude::sum;
    long part2 = data | rv::transform([](const auto &v) { return largest_subsequence(v, 12); })
                 | prelude::sum;

    fmt::print("Part 1: {}\n", part1);
    fmt::print("Part 2: {}\n", part2);
    return 0;
}