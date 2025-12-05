#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

bool is_blank(const std::string &line) {
    return line.empty() || std::ranges::all_of(line, isspace);
}

struct Range {
    // inclusive! And we're trusting the input to be ordered.
    long begin, end;

    static Range fromString(const std::string &s) {
        auto parts = s | std::views::split('-');
        auto it = parts.begin();
        long b = std::stol(std::string((*it).begin(), (*it).end()));
        ++it;
        long e = std::stol(std::string((*it).begin(), (*it).end()));
        return Range{b, e};
    }

    bool contains(const long x) const { return x >= begin && x <= end; }
    bool overlaps(const Range &o) const { return end >= o.begin && begin <= o.end; }
    long size() const { return 1 + end - begin; }
    Range combine(const Range &o) const {
        return Range{std::min(begin, o.begin), std::max(end, o.end)};
    }
};

int main(int, char **) {
    auto lines = prelude::line_view(std::cin);
    auto ranges = lines
                  | std::views::take_while([](const std::string &line) { return !is_blank(line); })
                  | rv::transform(Range::fromString) | prelude::collect<std::vector>;
    auto num_lines
        = lines | std::views::drop_while([](const std::string &line) { return is_blank(line); })
          | rv::transform([](const std::string &s) { return std::stol(s); })
          | prelude::collect<std::vector>;

    auto part1 = std::ranges::distance(num_lines | rv::filter([ranges](const long x) {
                                           for (auto &r : ranges) {
                                               if (r.contains(x)) {
                                                   return true;
                                               }
                                           }
                                           return false;
                                       }));
    fmt::print("part 1: {}\n", part1);

    std::sort(ranges.begin(), ranges.end(),
              [](const auto &a, const auto &b) { return a.begin < b.begin; });

    std::vector<Range> merged;
    for (auto const &r : ranges) {
        if (merged.empty() || !merged.back().overlaps(r)) {
            merged.push_back(r);
        } else {
            merged.back() = merged.back().combine(r);
        }
    }

    auto part2 = merged | rv::transform([](const auto &r) { return r.size(); }) | prelude::sum;
    fmt::print("part 2: {}\n", part2);

    return 0;
}