#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

struct Point {
    long x, y;
    auto operator<=>(const Point &) const = default;
};

template <> struct fmt::formatter<Point> : fmt::formatter<std::string_view> {
    auto format(const Point &p, fmt::format_context &ctx) const {
        std::string s = fmt::format("({}, {})", p.x, p.y);
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

long size(const Point &a, const Point &b) {
    return (1l + std::abs(a.x - b.x)) * (1 + std::abs(a.y - b.y));
}

int main(int, char **) {
    auto points = prelude::line_view(std::cin) | rv::transform([](const std::string &line) {
                      auto parts = line | rv::split(',') | rv::transform([](auto &&r) {
                                       return std::stol(std::string(r.begin(), r.end()));
                                   });
                      auto it = parts.begin();
                      long x = *it++;
                      long y = *it++;
                      return Point{x, y};
                  })
                  | prelude::collect<std::vector>;
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());

    long part1 = 0;
    for (auto a : points) {
        for (auto b : points) {
            part1 = std::max(part1, size(a, b));
        }
    }

    fmt::print("part 1: {}\n", part1);

    return 0;
}