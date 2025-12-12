#include "prelude/prelude.hpp"
#include <set>
namespace rv = std::ranges::views;
struct Machine {
    short desiredState;
    std::vector<short> buttons;
    std::vector<long> joltageRequirement;
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

std::vector<long> commaNums(const std::string &s) {
    return rv::split(s, ',') | rv::transform([](const auto &&r) {
               return std::string(std::ranges::begin(r), std::ranges::end(r));
           })
           | rv::transform([](const std::string &s) { return std::stol(s); })
           | prelude::collect<std::vector>;
}

short makeButton(const std::vector<long> &ns) {
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
        m.buttons.push_back(makeButton(commaNums(contained(parts[i]))));
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
            q.push_front(std::make_tuple(current, presses + 1));
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

long calcMachine2(const Machine &machine) {
    std::set<std::vector<long>> seen;
    std::deque<std::pair<std::vector<long>, long>> q;

    q.push_back(std::make_pair(std::vector<long>(machine.joltageRequirement.size()), 0));

    while (0 < q.size()) {
        auto [state, presses] = q.back();
        // std::string s;
        // for (size_t i = 0; i < state.size(); ++i) {
        //     s += fmt::format("{}, ", state[i]);
        // }
        // spdlog::info("presses = {}, state = {}", presses, s);
        q.pop_back();
        if (seen.contains(state)) {
            continue;
        }
        seen.insert(state);
        for (auto button : machine.buttons) {
            auto current = press(state, button);
            if (current == machine.joltageRequirement) {
                return presses + 1;
            }
            q.push_front(std::make_tuple(current, presses + 1));
        }
    }
    return -1;
}

long calc_part1(const std::vector<Machine> &machines) {
    return machines | rv::transform(calcMachine1) | prelude::sum;
}

long calc_part2(const std::vector<Machine> &machines) {
    return machines | rv::transform(calcMachine2) | prelude::sum;
}

int main(int, char **) {
    auto machines
        = prelude::line_view(std::cin) | rv::transform(fromString) | prelude::collect<std::vector>;

    long part1 = calc_part1(machines);
    fmt::print("part 1: {}\n", part1);

    long part2 = calc_part2(machines);
    fmt::print("part 2: {}\n", part2);
    return 0;
}