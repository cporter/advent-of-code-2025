#include "prelude/prelude.hpp"

using Graph = std::map<std::string, std::vector<std::string>>;

class DFS {
  private:
    struct key {
        std::string n;
        bool dac = false;
        bool fft = false;

        auto operator<=>(const key &) const = default;
    };

    std::map<key, long> _memo;
    const Graph &g;

  public:
    DFS(const Graph &_g) : g(_g) {}

    void clear() { _memo.clear(); }

    long dfs(const std::string &start, const std::string &dest, bool dac = false,
             bool fft = false) {
        if (start == dest) {
            return (dac && fft) ? 1 : 0;
        } else if (start == "dac") {
            dac = true;
        } else if (start == "fft") {
            fft = true;
        }
        key k{start, dac, fft};

        auto found = _memo.find(k);
        if (found == _memo.end()) {
            long tmp = 0;
            for (auto n : g.at(start)) {
                tmp += dfs(n, dest, dac, fft);
            }
            _memo[k] = tmp;
            auto [it, inserted] = _memo.insert(std::make_pair(k, tmp));
            if (inserted) {
                found = it;
            }
        }
        return _memo[k];
    }
};

int main(int, char **) {
    Graph g;

    for (auto &&r : prelude::line_view(std::cin)) {
        std::string line(std::ranges::begin(r), std::ranges::end(r));
        auto parts
            = line | rv::split(' ')
              | rv::transform([](auto &&r) { return std::string(std::begin(r), std::end(r)); })
              | prelude::collect<std::vector>;
        const auto name = parts[0].substr(0, 3);
        g[name].assign(std::ranges::next(std::ranges::begin(parts)), std::ranges::end(parts));
    }

    DFS dfs(g);
    auto part1 = dfs.dfs("you", "out", true, true);
    fmt::print("part 1: {}\n", part1);

    dfs.clear();
    auto part2 = dfs.dfs("svr", "out", false, false);
    fmt::print("part 2: {}\n", part2);
    return 0;
}
