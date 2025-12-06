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

    long part1 = 0;
    for (size_t i = 0; i < nums.size(); ++i) {
        switch (operands[i]) {
        case Operand::SUM:
            part1 += nums[i] | prelude::sum;
            break;
        case Operand::PRODUCT:
            part1 += nums[i] | prelude::product;
            break;
        default:
            break;
        }
    }

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
            switch (operands[op++]) {
            case Operand::SUM:
                part2 += accum | prelude::sum;
                break;
            case Operand::PRODUCT:
                part2 += accum | prelude::product;
                break;
            default:
                spdlog::error("Wat?");
                break;
            }
            accum.clear();
        } else {
            accum.push_back(std::stol(s));
        }
    }
    switch (operands[op++]) {
    case Operand::SUM:
        part2 += accum | prelude::sum;
        break;
    case Operand::PRODUCT:
        part2 += accum | prelude::product;
        break;
    default:
        spdlog::error("Wat?");
        break;
    }

    fmt::print("part 2: {}\n", part2);
}

int main(int, char **) {
    std::vector<std::string> raw_input
        = prelude::line_view(std::cin) | prelude::collect<std::vector>;

    part1(raw_input);
    part2(raw_input);
    return 0;
}