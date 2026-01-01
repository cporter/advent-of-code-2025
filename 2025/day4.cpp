#include "prelude/prelude.hpp"

enum GridState { EMPTY = 0, ROLL = 1 };

template <> struct fmt::formatter<GridState> : fmt::formatter<std::string_view> {
    auto format(const GridState &p, fmt::format_context &ctx) const {
        std::string s = GridState::ROLL == p ? "@" : ".";
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

template <typename T> class Grid {
  private:
    std::vector<std::vector<T>> _elts;
    int _rows, _cols;

  public:
    Grid(const std::vector<std::vector<T>> &elts)
        : _elts(std::move(elts)), _rows(_elts.size()), _cols(elts[0].size()) {}

    int rows() const { return _rows; }
    int cols() const { return _cols; }

    void set(const int r, const int c, const T &t) { _elts[r][c] = t; }

    struct Coord {
        int row, col;
    };
    struct Point {
        int row, col;
        T ref;
    };

    auto elts() const noexcept {
        return _elts | rv::enumerate | rv::transform([](auto &&rowpair) {
                   const auto [r, row] = rowpair;
                   return row | rv::enumerate | rv::transform([r](auto &&colpair) {
                              const auto [c, elt] = colpair;
                              Point point{static_cast<int>(r), static_cast<int>(c), elt};
                              return point;
                          });
               })
               | rv::join;
    }

    const T &at(int r, int c) const { return _elts.at(r).at(c); }

    static constexpr std::array<Coord, 9> deltas{
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

    auto neighbors(int r, int c) const {
        return deltas | rv::transform([r, c](const Coord &coord) {
                   return Coord{r + coord.row, c + coord.col};
               })
               | rv::filter([this](const Coord cc) {
                     return cc.row >= 0 && cc.row < _rows && cc.col >= 0 && cc.col < _cols;
                 })
               | rv::transform(
                   [this](const Coord &c) { return Point{c.row, c.col, _elts[c.row][c.col]}; });
    }
};

template <typename T> struct fmt::formatter<Grid<T>> : fmt::formatter<std::string_view> {
    auto format(const Grid<T> &g, fmt::format_context &ctx) const {
        std::string s;
        s.reserve((1 + g.rows()) + g.cols());
        for (int r = 0; r < g.rows(); ++r) {
            for (int c = 0; c < g.cols(); ++c) {
                s.append(fmt::format("{}", g.at(r, c)));
            }
            s.push_back('\n');
        }
        return fmt::formatter<std::string_view>::format(s, ctx);
    }
};

static GridState fromChar(const char ch) {
    switch (ch) {
    case '@':
        return GridState::ROLL;
    default:
        return GridState::EMPTY;
    }
}

using G = Grid<GridState>;

int fullNeighbors(const G &grid, const G::Point &p) {
    return grid.neighbors(p.row, p.col)
           | rv::transform([](const auto &p) { return p.ref == GridState::ROLL ? 1 : 0; })
           | prelude::sum;
}

auto removable(const G &grid) {
    return grid.elts() | rv::filter([](const auto &elt) { return elt.ref == GridState::ROLL; })
           | rv::transform(
               [grid](const auto &p) { return std::make_pair(p, fullNeighbors(grid, p)); })
           | rv::filter([](const auto &pp) {
                 const auto &[p, count] = pp;
                 return count <= 4;
             })
           | rv::transform([](const auto &pp) {
                 auto [p, count] = pp;
                 return p;
             });
}

int main(int, char **) {
    fmt::print("hello world\n");
    Grid<GridState> grid(prelude::line_view(std::cin) | rv::transform([](const auto &line) {
                             return line | rv::transform(fromChar) | prelude::collect<std::vector>;
                         })
                         | prelude::collect<std::vector>);

    int part1 = 0;
    int part2 = 0;
    while (true) {
        auto r = removable(grid) | prelude::collect<std::vector>;
        if (r.size() == 0) {
            break;
        }
        if (part1 == 0) {
            part1 = r.size();
        }
        part2 += r.size();
        for (auto &p : r) {
            grid.set(p.row, p.col, GridState::EMPTY);
        }
    }

    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);

    return 0;
}