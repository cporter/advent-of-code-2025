#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

#include "util/util.hpp"

namespace rv = std::ranges::views;

int main() {
    std::vector<int> left, right;
    cp::line_view lines{std::cin};

    for (const auto &[l, r] : lines | rv::transform([](const std::string &line) {
                                  int l = 0, r = 0;
                                  std::stringstream(line) >> l >> r;
                                  return std::pair<int, int>(l, r);
                              })) {
        left.push_back(l);
        right.push_back(r);
    }

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    auto part1 = cp::zip2(left, right)
                 | rv::transform([](const auto &p) { return std::abs(p.first - p.second); })
                 | cp::sum;

    std::cout << "part 1: " << part1 << "\n";

    // todo: maybe implement group_by?
    std::map<int, int> histo;
    for (auto &x : right) {
        histo[x]++;
    }

    auto part2 = left | rv::transform([&histo](const auto &x) { return x * histo[x]; }) | cp::sum;

    std::cout << "part 2: " << part2 << "\n";

    return 0;
}