#include "prelude/prelude.hpp"

long calc_part1(std::vector<std::string> &lines) {
    long count = 0;
    for (size_t i = 1; i < lines.size(); ++i) {
        std::string &line = lines[i];
        for (size_t c = 0; c < line.size(); ++c) {
            if (line[c] == '.') {
                if ((c > 0 && line[c - 1] == '^') || ((c + 1) < line.size() && line[c + 1] == '^')
                    || lines[i - 1][c] == 'S' || lines[i - 1][c] == '|') {
                    line[c] = '|';
                }
            } else if (line[c] == '^') {
                if (lines[i - 1][c] == '|') {
                    ++count;
                }
            }
        }
    }

    return count;
}

long calc_part2(std::vector<std::string> &lines) {
    std::reverse(lines.begin(), lines.end());

    std::vector<std::vector<long>> counts;
    counts.push_back(lines.front()
                     | rv::transform([](const char ch) { return ch == '|' ? 1l : 0l; })
                     | prelude::collect<std::vector>);

    for (auto &line : lines | rv::drop(1)) {
        auto &pc = counts.back();
        std::vector<long> next;
        for (size_t i = 0; i < line.size(); ++i) {
            switch (line[i]) {
            case 'S':
            case '|':
                next.push_back(pc[i]);
                break;
            case '^':
                next.push_back(pc[i - 1] + pc[i + 1]);
                break;
            default:
                next.push_back(0l);
                break;
            }
        }
        counts.push_back(std::move(next));
    }

    return counts.back() | prelude::sum;
}

int main(int, char **) {
    auto lines = prelude::line_view(std::cin) | prelude::collect<std::vector>;

    long part1 = calc_part1(lines);
    long part2 = calc_part2(lines);

    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);
    return 0;
}