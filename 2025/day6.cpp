#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

enum Operand { SUM, PRODUCT, NONE };

Operand operandFromString(const std::string &s) {
    if (s == "+") {
        return Operand::SUM;
    } else if (s == "*") {
        return Operand::PRODUCT;
    } else {
        return Operand::NONE;
    }
}

template <typename R> long compute(Operand o, R &&r) {
    switch (o) {
    case Operand::SUM:
        return r | prelude::sum;
    case Operand::PRODUCT:
        return r | prelude::product;
    default:
        return 0;
    }
}

void part1(const std::vector<std::string> &raw_input) {
    std::vector<std::vector<std::string>> orig_input = raw_input | rv::transform(prelude::chomp)
                                                       | rv::transform(prelude::split_ws)
                                                       | prelude::collect<std::vector>;

    auto operands
        = orig_input.back() | rv::transform(operandFromString) | prelude::collect<std::vector>;
    orig_input.pop_back();

    std::vector<std::vector<long>> nums;
    nums.resize(orig_input[0].size());
    for (size_t i = 0; i < orig_input[0].size(); ++i) {
        for (size_t j = 0; j < orig_input.size(); ++j) {
            nums[i].push_back(std::stol(orig_input[j][i]));
        }
    }

    long part1 = prelude::zip(operands, nums) | rv::transform([](const auto &p) {
                     auto &[op, num] = p;
                     return compute(op, num);
                 })
                 | prelude::sum;

    fmt::print("part 1: {}\n", part1);
}

void part2(std::vector<std::string> &orig) {
    auto operands = prelude::split_ws(prelude::chomp(orig.back()))
                    | rv::transform(operandFromString) | prelude::collect<std::vector>;
    orig.pop_back();

    std::vector<std::string> reformed;
    const size_t R = orig.size();
    const size_t C = orig[0].size();

    for (size_t i = 0; i < C; ++i) {
        std::string r;
        for (size_t j = 0; j < R; ++j) {
            r.push_back(orig[j][i]);
        }
        reformed.push_back(prelude::chomp(r));
    }

    long part2 = 0;
    std::vector<long> accum;
    size_t op = 0;
    for (auto &s : reformed) {
        if (0 == s.size()) {
            part2 += compute(operands[op++], accum);
            accum.clear();
        } else {
            accum.push_back(std::stol(s));
        }
    }
    part2 += compute(operands[op++], accum);

    fmt::print("part 2: {}\n", part2);
}

int main(int, char **) {
    std::vector<std::string> raw_input
        = prelude::line_view(std::cin) | prelude::collect<std::vector>;

    part1(raw_input);
    part2(raw_input);
    return 0;
}