#include "prelude/prelude.hpp"
namespace rv = std::ranges::views;

enum Operand { SUM, PRODUCT };

Operand operandFromString(const std::string &s) {
    if (s == "+") {
        return Operand::SUM;
    } else {
        return Operand::PRODUCT;
    }
}

int main(int, char **) {
    std::vector<std::vector<std::string>> orig_input
        = prelude::line_view(std::cin) | rv::transform(prelude::chomp)
          | rv::transform(prelude::split_ws) | prelude::collect<std::vector>;

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
        }
    }

    fmt::print("part 1: {}\n", part1);

    return 0;
}