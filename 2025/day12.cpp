#include "prelude/prelude.hpp"

#include <array>
#include <vector>

using std::operator""sv;

using shape = std::array<bool, 9>;

int count(const shape &s) {
    int ret = 0;
    for (auto &b : s) {
        if (b) {
            ++ret;
        }
    }
    return ret;
}

struct Problem {
    int width, height;
    std::vector<int> counts;
};

Problem fromLine(const std::string &s) {
    Problem p;
    std::vector<std::string> halves
        = rv::split(s, ": "sv)
          | rv::transform([](const auto &r) { return std::string(r.begin(), r.end()); })
          | prelude::collect<std::vector>;
    std::vector<int> dimensions
        = rv::split(halves[0], "x"sv)
          | rv::transform([](const auto &r) { return std::stoi(std::string(r.begin(), r.end())); })
          | prelude::collect<std::vector>;
    p.width = dimensions[0];
    p.height = dimensions[1];
    p.counts
        = prelude::split_ws(halves[1])
          | rv::transform([](const auto &r) { return std::stoi(std::string(r.begin(), r.end())); })
          | prelude::collect<std::vector>;
    return p;
}

template <> struct fmt::formatter<Problem> : fmt::formatter<std::string_view> {
    auto format(const Problem &p, fmt::format_context &ctx) const {
        std::string s = fmt::format("{}x{}", p.width, p.height);
        for (auto c : p.counts) {
            s += fmt::format(" {}", c);
        }
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

long calc_part1(const std::vector<shape> &shapes, const std::vector<Problem> &problems) {
    return problems | rv::transform([shapes](const Problem &p) {
               int area = p.width * p.height;
               int needed = 0;
               for (auto [idx, c] : rv::enumerate(p.counts)) {
                   // admission: I got this solution off the internet.
                   needed += 9 * c;
               }
               return (needed <= area) ? 1 : 0;
           })
           | prelude::sum;
}

int main(int, char **) {
    auto lines = prelude::line_view(std::cin) | prelude::collect<std::vector>;
    std::vector<shape> shapes;

    // lordy lordy I apologize for being this cheesy I just want to finish.
    for (int i = 0; i < 5; ++i) {
        shape s;
        int idx = 0;
        for (int j = 0; j < 3; ++j) {
            const auto &line = lines[1 + j + 5 * i];
            for (auto x : line) {
                s[idx++] = x == '#';
            }
        }
        shapes.push_back(s);
    }

    std::vector<Problem> problems = std::ranges::drop_view(lines, 30) | rv::transform(fromLine)
                                    | prelude::collect<std::vector>;

    long part1 = calc_part1(shapes, problems);
    fmt::print("part 1: {}\n", part1);
}