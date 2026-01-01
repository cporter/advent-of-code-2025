#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

// yep, we're polluting the global namespace. It's for-fun coding. Sue me.
using std::operator""sv;
namespace rv = std::ranges::views;

// Rangy utilities that make C++ feel more at home.
// I think a lot of these things are in c++23? I should just get a new compiler.

namespace prelude {

template <typename R> auto front(R &&r) {
    for (auto &x : r) {
        return x;
    }
    throw std::logic_error("this does not work with empty ranges");
}

struct line_view : std::ranges::view_base {
    std::istream *is;

    enum Done { NO, YES };

    line_view(std::istream &s) : is(&s) {}

    // Input iterator for lines (single-pass)
    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using reference = const std::string &;

        std::istream *is_ptr = nullptr;
        std::string line;
        Done done = Done::NO;

        iterator() = default;
        explicit iterator(std::istream *s, Done d) : is_ptr(s), done(d) {
            if (!done) {
                ++(*this);
            }
        }

        reference operator*() const noexcept { return line; }

        iterator &operator++() {
            if (!is_ptr) {
                done = Done::YES;
            } else if (!std::getline(*is_ptr, line)) {
                done = Done::YES;
                // leave line in a valid but unspecified state
            }
            return *this;
        }

        // post-increment for input iterator (returns void or iterator proxy;
        // simple void here)
        iterator operator++(int) {
            iterator ret = *this;
            ++*this;
            return ret;
        }

        friend bool operator==(const iterator &self, const iterator &other) noexcept {
            // Two iterators compare equal only if both are "done" (end)
            return self.done == other.done && (self.done ? true : self.is_ptr == other.is_ptr);
        }
        friend bool operator!=(const iterator &self, const iterator &other) noexcept {
            return !(self == other);
        }
    };

    iterator begin() const { return iterator{is, Done::NO}; }
    iterator end() const { return iterator{is, Done::YES}; }
};

//
// collect<C>(range) or range | collect<C>
//
namespace detail {
template <template <class...> class C> struct collect_fn {
    template <std::ranges::input_range R> auto operator()(R &&r) const {
        using T = std::ranges::range_value_t<R>;

        if constexpr (requires { C<T>{std::ranges::begin(r), std::ranges::end(r)}; }) {
            return C<T>(std::ranges::begin(r), std::ranges::end(r));
        } else if constexpr (requires(C<T> c) {
                                 c.assign(std::ranges::begin(r), std::ranges::end(r));
                             }) {
            C<T> out;
            out.assign(std::ranges::begin(r), std::ranges::end(r));
            return out;
        } else {
            C<T> out;

            if constexpr (std::ranges::sized_range<R>
                          && requires { out.reserve(std::ranges::size(r)); }) {
                out.reserve(std::ranges::size(r));
            }

            for (auto &&v : r)
                out.emplace_back(v);

            return out;
        }
    }

    template <std::ranges::input_range R> friend auto operator|(R &&r, collect_fn self) {
        return self(std::forward<R>(r));
    }
};
} // namespace detail
template <template <class...> class C> inline constexpr detail::collect_fn<C> collect{};

// collect optional types iff they are all there. If anything isn't,
// nullopt the whole thing.
template <std::ranges::input_range R> auto collect_optional(R &&r) {
    using Opt = std::ranges::range_value_t<R>;
    using T = typename Opt::value_type;
    using Result = std::optional<std::vector<T>>;

    std::vector<T> result;
    for (const auto &opt : r) {
        if (!opt)
            return Result{std::nullopt};
        result.push_back(*opt);
    }
    return Result(std::move(result));
}

// pipey reduce
namespace detail {
struct reduce_fn {
    template <typename T, typename BinaryOp> struct pipeable {
        T init;
        BinaryOp op;

        // no member operator| here
        template <std::ranges::input_range R> friend auto operator|(R &&r, const pipeable &self) {
            if constexpr (std::ranges::common_range<R>) {
                return std::accumulate(r.begin(), r.end(), self.init, self.op);
            } else {
                auto common = std::ranges::common_view(r);
                return std::accumulate(common.begin(), common.end(), self.init, self.op);
            }
        }
    };

    template <typename T, typename BinaryOp> auto operator()(T init, BinaryOp op) const {
        return pipeable<T, BinaryOp>{init, op};
    }
};
} // namespace detail

inline constexpr detail::reduce_fn reduce;

namespace detail {
struct sum_fn {
    template <std::ranges::input_range R> friend auto operator|(R &&r, sum_fn) {
        using T = std::ranges::range_value_t<std::decay_t<decltype(r)>>;
        return r | reduce(T{0}, std::plus<>{});
    }
};

struct product_fn {
    template <std::ranges::input_range R> friend auto operator|(R &&r, product_fn) {
        using T = std::ranges::range_value_t<std::decay_t<decltype(r)>>;
        return r | reduce(T{1}, std::multiplies<>{});
    }
};

} // namespace detail

inline constexpr detail::sum_fn sum;
inline constexpr detail::product_fn product;

namespace detail {
struct for_each_fn {
    template <typename F> struct pipeable {
        F f;
        template <std::ranges::input_range R>
            requires std::invocable<F &, std::ranges::range_reference_t<R>>
        friend void operator|(R &&r, const pipeable &p) {
            for (auto &&x : r)
                p.f(x);
        }
    };

    template <typename F> constexpr auto operator()(F f) const { return pipeable{f}; }
};
} // namespace detail

inline constexpr detail::for_each_fn for_each;

namespace detail {
struct run_length_fn {
    template <std::ranges::viewable_range R> auto operator()(R &&r) const {
        return std::forward<R>(r)
               | rv::chunk_by(std::equal_to{})          // group equal consecutive elements
               | std::views::transform([](auto group) { // convert group → (value,count)
                     return std::pair{*group.begin(),   // the value
                                      static_cast<std::size_t>(std::ranges::distance(group))};
                 });
    }

    // pipeable adaptor
    template <std::ranges::viewable_range R>
    friend auto operator|(R &&r, const run_length_fn &self) {
        return self(std::forward<R>(r));
    }
};
} // namespace detail
inline constexpr detail::run_length_fn run_length;

// string stuff

std::string chomp(std::string s) {
    auto is_space = [](char c) { return std::isspace((unsigned char)c); };

    auto first = std::ranges::find_if_not(s, is_space);
    auto last = std::ranges::find_if_not(s | std::views::reverse, is_space).base();

    return (first < last) ? std::string(first, last) : std::string{};
}

std::vector<std::string> split_ws(std::string_view sv) {
    std::vector<std::string> out;
    size_t i = 0, n = sv.size();

    while (i < n) {
        // skip whitespace
        while (i < n && std::isspace((unsigned char)sv[i]))
            ++i;

        size_t start = i;

        // consume non-whitespace
        while (i < n && !std::isspace((unsigned char)sv[i]))
            ++i;

        if (start < i)
            out.emplace_back(sv.substr(start, i - start));
    }

    return out;
}

template <typename Collection> Collection transpose(Collection &orig) {
    Collection reformed;
    const size_t R = orig.size();
    const size_t C = orig[0].size();

    for (size_t i = 0; i < C; ++i) {
        std::string r;
        for (size_t j = 0; j < R; ++j) {
            r.push_back(orig[j][i]);
        }
        reformed.push_back(prelude::chomp(r));
    }
    return reformed;
}

template <typename R> auto pairwise(R &&r) { return rv::zip(r, std::ranges::drop_view(r, 1)); }

} // namespace prelude
