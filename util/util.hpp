#pragma once

#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {

struct line_view {
    std::istream &is;

    enum Done { NO, YES };

    // Input iterator for lines (single-pass)
    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using reference = const std::string &;

        std::istream *is_ptr = nullptr;
        std::string line;
        Done done = Done::NO;

        iterator() = default; // end iterator

        explicit iterator(std::istream &s, Done d) : is_ptr(&s), done(d) {
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

        // equality with another iterator is only meaningful for end iterators
        // in input iterators
        bool operator==(const iterator &other) const noexcept {
            // Two iterators compare equal only if both are "done" (end)
            return done == other.done && (done ? true : is_ptr == other.is_ptr);
        }
    };

    // members
    line_view(std::istream &s) : is(s) {}

    // member begin/end (non-const)
    iterator begin() { return iterator{is, Done::NO}; }
    iterator end() { return iterator{is, Done::YES}; }
};

template <std::ranges::input_range R> auto enumerate(R &&r) {
    struct view {
        R base;

        struct iterator {
            std::ranges::iterator_t<R> current;
            std::size_t index = 0;

            iterator() = default;
            explicit iterator(std::ranges::iterator_t<R> it) : current(it) {}

            auto operator*() const {
                // return pair of index and reference to element
                return std::pair{index, *current};
            }

            iterator &operator++() {
                ++current;
                ++index;
                return *this;
            }

            bool operator==(const iterator &other) const { return current == other.current; }
            bool operator!=(const iterator &other) const { return !(*this == other); }
        };

        auto begin() { return iterator{std::ranges::begin(base)}; }
        auto end() { return iterator{std::ranges::end(base)}; }
    };

    return view{std::forward<R>(r)};
}

//
// collect<C>(range) or range | collect<C>
//
template <template <class...> class C> struct collect_t {
    template <std::ranges::input_range R> auto operator()(R &&r) const {
        using T = std::ranges::range_value_t<R>;

        if constexpr (requires { C<T>{std::ranges::begin(r), std::ranges::end(r)}; }) {
            return C<T>(std::ranges::begin(r), std::ranges::end(r));
        } else if constexpr (requires(C<T> c) { c.assign(std::ranges::begin(r), std::ranges::end(r)); }) {
            C<T> out;
            out.assign(std::ranges::begin(r), std::ranges::end(r));
            return out;
        } else {
            C<T> out;

            if constexpr (std::ranges::sized_range<R> && requires { out.reserve(std::ranges::size(r)); }) {
                out.reserve(std::ranges::size(r));
            }

            for (auto &&v : r)
                out.emplace_back(v);

            return out;
        }
    }

    template <std::ranges::input_range R> friend auto operator|(R &&r, collect_t self) {
        return self(std::forward<R>(r));
    }
};

// Factory: collect<std::vector>
template <template <class...> class C> inline constexpr collect_t<C> collect{};

} // namespace cp
