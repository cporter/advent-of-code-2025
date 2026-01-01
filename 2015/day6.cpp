#include "prelude/prelude.hpp"
#include <regex>
#include <variant>

const int N = 1000;

struct coord {
    int x, y;
};

struct Turn {
    bool on;
    coord tl;
    coord br;
};

struct Toggle {
    coord tl;
    coord br;
};

using Instruction = std::variant<Turn, Toggle>;

const std::regex turn_re("turn (on|off) (\\d+),(\\d+) through (\\d+),(\\d+)");
const std::regex toggle_re("toggle (\\d+),(\\d+) through (\\d+),(\\d+)");
Instruction parseInstruction(std::string_view _s) {
    std::string s(_s);
    std::smatch match;
    if (std::regex_match(s, match, turn_re)) {
        bool direction = match[1].str() == "on"sv;
        coord a{std::stoi(match[2].str()), std::stoi(match[3].str())};
        coord b{std::stoi(match[4].str()), std::stoi(match[5].str())};
        return Turn{direction,
                    {std::min(a.x, b.x), std::min(a.y, b.y)},
                    {std::max(a.x, b.x), std::max(a.y, b.y)}};
    } else if (std::regex_match(s, match, toggle_re)) {
        coord a{std::stoi(match[1].str()), std::stoi(match[2].str())};
        coord b{std::stoi(match[3].str()), std::stoi(match[4].str())};
        return Toggle{{std::min(a.x, b.x), std::min(a.y, b.y)},
                      {std::max(a.x, b.x), std::max(a.y, b.y)}};
    } else {
        throw std::runtime_error(fmt::format("malformed input: {}", s));
    }
}

template <typename T> class Grid {
  private:
    std::vector<T> _g;
    int _rows, _cols;

  public:
    Grid(const int rows, const int cols) : _g(rows * cols), _rows(rows), _cols(cols) {}

    int rows() const { return _rows; }
    int cols() const { return _cols; }

    const std::vector<T> &elts() const { return _g; }

    T get(int r, int c) const {
        assert(r >= 0 && r < _rows);
        assert(c >= 0 && c < _cols);
        return _g[r * _cols + c];
    }

    void set(int r, int c, const T &x) {
        assert(r >= 0 && r < _rows);
        assert(c >= 0 && c < _cols);
        _g[r * _cols + c] = x;
    }
};

int main(int, char **) {
    Grid<bool> g(N, N);
    for (auto line : prelude::line_view(std::cin)) {
        auto inst = parseInstruction(line);
        std::visit(
            [&g](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                for (int r = arg.tl.y; r <= arg.br.y; ++r) {
                    for (int c = arg.tl.x; c <= arg.br.x; ++c) {
                        if constexpr (std::is_same_v<T, Turn>) {
                            g.set(r, c, arg.on);
                        } else if constexpr (std::is_same_v<T, Toggle>) {
                            g.set(r, c, !g.get(r, c));
                        } else {
                            throw std::runtime_error("Whoa, not a type we expected");
                        }
                    }
                }
            },
            inst);
    }

    int part1 = std::ranges::distance(g.elts() | rv::filter([](bool b) { return b; }));
    fmt::print("part 1: {}\n", part1);

    return 0;
}