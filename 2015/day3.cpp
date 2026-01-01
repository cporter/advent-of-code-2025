#include "prelude/prelude.hpp"
#include <set>

struct coord {
    int x, y;
    auto operator<=>(const coord &) const = default;
};

coord operator+(const coord &a, const coord &b) { return coord{a.x + b.x, a.y + b.y}; }
const std::map<char, coord> directions
    = {{'>', {1, 0}}, {'<', {-1, 0}}, {'^', {0, 1}}, {'v', {0, -1}}};

int countHouses(const std::vector<coord> &moves) {
    std::set<coord> houses;
    coord position{0, 0};
    houses.insert(position);
    for (const auto &move : moves) {
        position = position + move;
        houses.insert(position);
    }
    return houses.size();
}

int roboHouses(const std::vector<coord> &moves) {
    std::set<coord> seen;
    coord s{0, 0}, r{0, 0};
    seen.insert(s);
    for (auto [idx, m] : rv::enumerate(moves)) {
        if (idx % 2 == 0) {
            s = s + m;
            seen.insert(s);
        } else {
            r = r + m;
            seen.insert(r);
        }
    }
    return seen.size();
}

int main(int, char **) {
    std::vector<coord> moves
        = prelude::front(prelude::line_view(std::cin)) | rv::transform([](const char ch) {
              auto it = directions.find(ch);
              if (it != directions.end()) {
                  return it->second;
              }
              throw std::logic_error(fmt::format("No such direction {}", ch));
          })
          | prelude::collect<std::vector>;

    int part1 = countHouses(moves);
    fmt::print("part 1: {}\n", part1);
    int part2 = roboHouses(moves);
    fmt::print("part 2: {}\n", part2);
    return 0;
}