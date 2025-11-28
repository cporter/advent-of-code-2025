#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

#include <fmt/core.h>

#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;

// template <typename R> class drange {
//   private:
//     R _r;
//     std::ranges::iterator_t<R> _begin, _end;

//   public:
//     using T = std::ranges::value_t<R>;
//     drange(R&& r) : _r(std::forward<R>(r)), _begin(std::ranges(begin(_r)),
//     _end(std::ranges::end(_r)) {}

//     T& operator*() const {
//         return *_begin; }
//     bool done() const {
//         return _begin == _end; }
//     void advance() {
//         ++_begin; }
// };

int main() {
    std::vector<int> left, right;
    prelude::line_view lines{std::cin};

    prelude::line_view(std::cin) | rv::transform([](const std::string &line) {
        int l = 0, r = 0;
        std::stringstream(line) >> l >> r;
        return std::pair<int, int>(l, r);
    }) | prelude::for_each([&left, &right](const auto &p) {
        left.push_back(p.first);
        right.push_back(p.second);
    });

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    auto part1 = prelude::zip(left, right) | rv::transform([](const auto &tup) {
                     auto &[a, b] = tup;
                     return std::abs(a - b);
                 })
                 | prelude::sum;

    fmt::print("part 1: {}\n", part1);

    auto lrl = prelude::run_length(left) | prelude::collect<std::vector>;
    auto rrl = prelude::run_length(right) | prelude::collect<std::vector>;

    int part2 = 0;
    for (size_t li = 0, ri = 0; li < lrl.size() && ri < rrl.size();) {
        if (lrl[li].first < rrl[ri].first) {
            ++li;
        } else if (lrl[li].first > rrl[ri].first) {
            ++ri;
        } else { // lrl[li].first == rrl[ri].first
            part2 += lrl[li].first * lrl[li].second * rrl[ri].second;
            ++li;
            ++ri;
        }
    }

    fmt::print("part 2: {}\n", part2);

    return 0;
}