#include "prelude/prelude.hpp"
#include <deque>
#include <set>

#include <z3++.h>

namespace rv = std::ranges::views;
struct Machine {
    short desiredState;
    std::vector<short> buttons;
    std::vector<std::vector<short>> numButtons;
    std::vector<short> joltageRequirement;
};

short bits(const std::vector<bool> &bs) {
    long ret = 0;
    for (size_t i = 0; i < bs.size(); ++i) {
        if (bs[i]) {
            ret |= (1 << i);
        }
    }
    return ret;
}

auto contained(const std::string &s) { return s.substr(1, s.size() - 2); }

short stos(const std::string &s) { return static_cast<short>(std::stoi(s)); }

std::vector<short> commaNums(const std::string &s) {
    return rv::split(s, ',') | rv::transform([](const auto &&r) {
               return std::string(std::ranges::begin(r), std::ranges::end(r));
           })
           | rv::transform([](const std::string &s) { return stos(s); })
           | prelude::collect<std::vector>;
}

short makeButton(const std::vector<short> &ns) {
    short ret = 0;
    for (auto n : ns) {
        ret |= (1 << n);
    }
    return ret;
}

Machine fromString(const std::string &line) {
    Machine m;
    auto parts = rv::split(line, ' ') | rv::transform([](auto &&r) {
                     return std::string(std::ranges::begin(r), std::ranges::end(r));
                 })
                 | prelude::collect<std::vector>;

    m.desiredState
        = bits(contained(parts.front()) | rv::transform([](const char ch) { return ch == '#'; })
               | prelude::collect<std::vector>);

    for (size_t i = 1; i < parts.size() - 1; ++i) {
        m.numButtons.push_back(commaNums(contained(parts[i])));
        m.buttons.push_back(makeButton(m.numButtons.back()));
    }
    m.joltageRequirement = commaNums(contained(parts.back()));

    return m;
};

long calcMachine1(const Machine &machine) {
    std::set<short> seen;
    std::deque<std::pair<short, long>> q;

    q.push_back({0, 0});

    while (0 < q.size()) {
        auto [state, presses] = q.back();
        q.pop_back();
        if (seen.contains(state)) {
            continue;
        }
        seen.insert(state);
        for (auto button : machine.buttons) {
            auto current = state ^ button;
            if (current == machine.desiredState) {
                return presses + 1;
            }
            q.push_front({current, presses + 1});
        }
    }
    return -1;
}

std::vector<long> press(const std::vector<long> &start, short button) {
    std::vector<long> current = start;
    int i = 0;
    while (button > 0) {
        if (button & 1) {
            current[i]++;
        }
        button /= 2;
        ++i;
    }
    return current;
}

int calcMachine2(const Machine &machine) {
    const int N = machine.joltageRequirement.size();
    const int M = machine.numButtons.size();

    z3::context c;

    std::vector<std::vector<int>> B(N, std::vector<int>(M, 0));

    for (int j = 0; j < M; ++j) {
        for (auto i : machine.numButtons[j]) {
            B[i][j] += 1;
        }
    }
    z3::expr_vector buttons(c);
    for (int j = 0; j < M; ++j) {
        std::string name = fmt::format("b_{}", j);
        buttons.push_back(c.int_const(name.c_str()));
    }

    z3::optimize opt(c);
    for (int j = 0; j < M; ++j) {
        opt.add(buttons[j] >= 0);
    }

    for (int i = 0; i < N; ++i) {
        z3::expr sum_expr = c.int_val(0);
        for (int j = 0; j < M; ++j) {
            sum_expr = sum_expr + (B[i][j] * buttons[j]);
        }
        opt.add(sum_expr == machine.joltageRequirement[i]);
    }

    opt.minimize(z3::sum(buttons));

    if (opt.check() != z3::sat) {
        spdlog::error("NO SOLUTION");
        return -1;
    }

    z3::model model = opt.get_model();
    int ret = 0;

    for (int j = 0; j < M; ++j) {
        ret += model.eval(buttons[j]).as_int64();
    }

    return ret;
}

long calc_part1(const std::vector<Machine> &machines) {
    return machines | rv::transform(calcMachine1) | prelude::sum;
}

int calc_part2(const std::vector<Machine> &machines) {
    return machines | rv::transform(calcMachine2) | prelude::sum;
}

int main(int, char **) {
    auto machines
        = prelude::line_view(std::cin) | rv::transform(fromString) | prelude::collect<std::vector>;

    long part1 = calc_part1(machines);
    fmt::print("part 1: {}\n", part1);

    int part2 = calc_part2(machines);
    fmt::print("part 2: {}\n", part2);
    return 0;
}