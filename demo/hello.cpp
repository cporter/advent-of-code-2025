#include <iostream>

#include <fmt/core.h>

#include "prelude/prelude.hpp"

int main(int, char **) {

    std::vector<int> v = {1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 5};

    // for (auto group : v | prelude::chunk_by(std::equal_to{})) {
    //     std::cout << "[ ";
    //     for (int x : group)
    //         std::cout << x << " ";
    //     std::cout << "]\n";
    // }

    for (auto rl : v | prelude::run_length) {
        fmt::print("{} -> {}\n", rl.first, rl.second);
    }

    return 0;
}