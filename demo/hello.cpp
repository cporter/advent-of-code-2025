#include <iostream>

#include "prelude/prelude.hpp"

int main(int, char **) {

    std::vector<int> a = {1, 2, 3, 4, 5};
    std::vector<int> b = a | std::ranges::views::transform([](const int &x) { return x + x; })
                         | cp::collect<std::vector>;

    auto z = cp::zip2(a, b);

    static_assert(std::ranges::range<decltype(z)>);

    auto tx = z | std::ranges::views::transform([](const auto &p) {
                  const auto &[x, y] = p;
                  return std::pair(x * 3, y * 3);
              });

    auto answer = tx
                  | std::ranges::views::transform([](const auto &p) { return p.first + p.second; })
                  | cp::sum;

    std::cout << answer << "\n";

    return 0;
}