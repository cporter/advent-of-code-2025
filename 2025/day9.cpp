
#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

struct Point {
    double x, y;
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

long calc_part1(const std::vector<Point> &points) {
    long part1 = 0;
    for (auto a : points) {
        for (auto b : points) {
            part1 = std::max(part1, size(a, b));
        }
    }
    return part1;
}

struct Edge {
    double constant, begin, end;
};

template <> struct fmt::formatter<Edge> : fmt::formatter<std::string_view> {
    auto format(const Edge &e, fmt::format_context &ctx) const {
        std::string s = fmt::format("({}, {}, {})", e.constant, e.begin, e.end);
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

std::pair<std::vector<Edge>, std::vector<Edge>> edges(const std::vector<Point> &points) {
    std::vector<Edge> h, v;
    for (auto [a, b] : prelude::pairwise(points)) {
        if (a.x == b.x) {
            h.emplace_back(a.x, std::min(a.y, b.y), std::max(a.y, b.y));
        } else {
            v.emplace_back(a.y, std::min(a.x, b.x), std::max(a.x, b.x));
        }
    }
    return std::make_pair(std::move(h), std::move(v));
}

bool isInside(const Point &p, const std::vector<Point> &points) {
    bool ret = false;
    for (auto [a, b] : prelude::pairwise(points)) {
        if ((a.y > p.y) != (b.y > p.y)) {
            auto intersect_x = (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x;
            if (p.x < intersect_x) {
                ret = !ret;
            }
        }
    }
    return ret;
}

bool isRectIntruded(const Point &min, const Point &max, const std::vector<Edge> &ve,
                    const std::vector<Edge> &he) {
    for (auto [vx, vy_min, vy_max] : ve) {
        if (min.x < vx && vx < max.x) {
            if (std::max(vy_min, min.y) < std::min(vy_max, max.y)) {
                return true;
            }
        }
    }
    for (auto [hy, hx_min, hx_max] : he) {
        if (min.y < hy && hy < max.y) {
            if (std::max(hx_min, min.x) < std::min(hx_max, max.x)) {
                return true;
            }
        }
    }
    return false;
}

long calc_part2(std::vector<Point> &points) {
    points.push_back(points.front());
    auto [ve, he] = edges(points);

    long part2 = 0;
    size_t N = points.size() - 1;
    for (size_t ai = 0; ai < N; ++ai) {
        for (size_t bi = 1 + ai; bi < N; ++bi) {
            const Point &a = points[ai];
            const Point &b = points[bi];
            const Point min{std::min(a.x, b.x), std::min(a.y, b.y)};
            const Point max{std::max(a.x, b.x), std::max(a.y, b.y)};
            const Point inside{min.x + .5, min.y + .5};
            long s = size(a, b);
            if (s < part2) {
                continue;
            }
            if (isRectIntruded(min, max, ve, he)) {
                continue;
            }
            if (!isInside(inside, points)) {
                continue;
            }
            part2 = s;
        }
    }

    return part2;
}

int main(int, char **) {
    auto points = prelude::line_view(std::cin) | rv::transform([](const std::string &line) {
                      auto parts = line | rv::split(',') | rv::transform([](auto &&r) {
                                       return std::stol(std::string(r.begin(), r.end()));
                                   });
                      auto it = parts.begin();
                      long x = *it++;
                      long y = *it++;
                      return Point{static_cast<double>(x), static_cast<double>(y)};
                  })
                  | prelude::collect<std::vector>;
    // std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());

    long part1 = calc_part1(points);

    fmt::print("part 1: {}\n", part1);

    long part2 = calc_part2(points);

    fmt::print("part 2: {}\n", part2);

    return 0;
}