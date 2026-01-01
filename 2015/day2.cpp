#include "prelude/prelude.hpp"

namespace rv = std::ranges::views;
using std::operator""sv;

struct Package {
    int height, length, width;

    int wrappingRequired() const {
        return 3 * height * length + 2 * height * width + 2 * length * width;
    }
    int ribbonRequired() const { return 2 * (height + length) + height * length * width; }
};

Package fromString(const std::string &s) {
    std::vector<int> dims = rv::split(s, "x"sv) | rv::transform([](const auto &r) {
                                return std::stoi(std::string(std::begin(r), std::end(r)));
                            })
                            | prelude::collect<std::vector>;
    // guaranteeing that height <= length <= width
    std::sort(dims.begin(), dims.end());
    return Package{dims[0], dims[1], dims[2]};
}

int main(int, char **) {
    std::vector<Package> packages
        = prelude::line_view(std::cin) | rv::transform(fromString) | prelude::collect<std::vector>;
    int part1 = packages | rv::transform(&Package::wrappingRequired) | prelude::sum;
    int part2 = packages | rv::transform(&Package::ribbonRequired) | prelude::sum;
    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);
}