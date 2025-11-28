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
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Rangy utilities that make C++ feel more at home.
// I think a lot of these things are in c++23? I should just get a new compiler.

namespace prelude {

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

struct enumerate_fn {
    template <typename Index = size_t, std::ranges::input_range R> auto operator()(R &&r) const {
        struct view : std::ranges::view_base {
            std::ranges::views::all_t<R> base;

            view(R &&r) : base(std::forward<R>(r)) {}

            struct iterator {
                using iterator_category = std::input_iterator_tag;
                using value_type = std::pair<Index, std::ranges::range_value_t<R>>;
                using difference_type = std::ranges::range_difference_t<R>;
                using reference = std::pair<Index, std::ranges::range_reference_t<R>>;

                using BaseIt = std::ranges::iterator_t<decltype(base)>;
                BaseIt current;
                Index index = 0;

                iterator() = default;
                explicit iterator(BaseIt it) : current(it) {}

                auto operator*() const { return std::pair{index, *current}; }

                iterator &operator++() {
                    ++current;
                    ++index;
                    return *this;
                }
                iterator operator++(int) {
                    iterator ret = *this;
                    ++*this;
                    return ret;
                }

                bool operator==(const iterator &other) const { return current == other.current; }
            };

            iterator begin() const { return iterator{std::ranges::begin(base)}; }
            iterator end() const { return iterator{std::ranges::end(base)}; }
        };

        return view{std::forward<R>(r)};
    }

    // Support pipe syntax: enumerate | range
    template <std::ranges::input_range R> friend auto operator|(R &&r, const enumerate_fn &e) {
        return e(std::forward<R>(r));
    }
};

inline constexpr enumerate_fn enumerate;

//
// collect<C>(range) or range | collect<C>
//
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

    template <std::ranges::input_range R> friend auto operator|(R &&r, collect_t self) {
        return self(std::forward<R>(r));
    }
};

template <template <class...> class C> inline constexpr collect_fn<C> collect{};

// zip

struct zip2_fn {
    template <std::ranges::input_range R1, std::ranges::input_range R2>
    auto operator()(R1 &&r1, R2 &&r2) const {
        struct view : std::ranges::view_base {
            std::ranges::views::all_t<R1> _r1;
            std::ranges::views::all_t<R2> _r2;

            view(R1 &&r1, R2 &&r2) : _r1(std::forward<R1>(r1)), _r2(std::forward<R2>(r2)) {}

            struct iterator {
                using R1It = std::ranges::iterator_t<decltype(_r1)>;
                using R2It = std::ranges::iterator_t<decltype(_r2)>;

                R1It _it1;
                R2It _it2;

                iterator() = default;
                iterator(R1It it1, R2It it2) : _it1(it1), _it2(it2) {}

                using iterator_category = std::input_iterator_tag;
                using value_type
                    = std::pair<std::ranges::range_value_t<R1>, std::ranges::range_value_t<R2>>;
                using difference_type = std::ptrdiff_t;
                using reference = std::pair<std::ranges::range_reference_t<R1>,
                                            std::ranges::range_reference_t<R2>>;

                reference operator*() const { return {*_it1, *_it2}; }

                iterator &operator++() {
                    ++_it1;
                    ++_it2;
                    return *this;
                }
                iterator operator++(int) {
                    iterator tmp = *this;
                    ++*this;
                    return tmp;
                }

                bool operator==(const iterator &other) const {
                    return _it1 == other._it1 || _it2 == other._it2;
                }
                bool operator!=(const iterator &other) const { return !(*this == other); }
            };

            iterator begin() { return iterator{std::ranges::begin(_r1), std::ranges::begin(_r2)}; }
            iterator end() { return iterator{std::ranges::end(_r1), std::ranges::end(_r2)}; }
        };

        return view{std::forward<R1>(r1), std::forward<R2>(r2)};
    }
};

inline constexpr zip2_fn zip2;

// pipey reduce
struct reduce_fn {
    template <typename T, typename BinaryOp> struct pipeable {
        T init;
        BinaryOp op;

        // no member operator| here
        template <std::ranges::input_range R> friend auto operator|(R &&r, const pipeable &self) {
            return std::accumulate(r.begin(), r.end(), self.init, self.op);
        }
    };

    template <typename T, typename BinaryOp> auto operator()(T init, BinaryOp op) const {
        return pipeable<T, BinaryOp>{init, op};
    }
};

inline constexpr reduce_fn reduce;

struct sum_fn {
    template <std::ranges::input_range R> friend auto operator|(R &&r, sum_fn) {
        using T = std::ranges::range_value_t<std::decay_t<decltype(r)>>;
        return r | reduce(T{0}, std::plus<>{});
    }
};

inline constexpr sum_fn sum;

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

inline constexpr for_each_fn for_each;

template <std::ranges::view R, class Pred>
    requires std::regular_invocable<Pred &, std::ranges::range_reference_t<R>,
                                    std::ranges::range_reference_t<R>>
class chunk_by_view : public std::ranges::view_base {
    R base_;
    Pred pred_;

    using Iter = std::ranges::iterator_t<R>;

  public:
    chunk_by_view() = default;
    chunk_by_view(R base, Pred pred) : base_(std::move(base)), pred_(std::move(pred)) {}

    class iterator {
        Iter cur_;
        Iter end_;
        Pred *pred_;

      public:
        using value_type = std::ranges::subrange<Iter>;
        using difference_type = std::ptrdiff_t;

        iterator() = default;
        iterator(Iter cur, Iter end, Pred *pred) : cur_(cur), end_(end), pred_(pred) {}

        value_type operator*() const {
            Iter first = cur_;
            Iter it = cur_;
            if (it != end_) {
                Iter next = it;
                ++next;
                while (next != end_ && std::invoke(*pred_, *it, *next)) {
                    ++it;
                    ++next;
                }
                ++it;
            }
            return {first, it};
        }

        iterator &operator++() {
            if (cur_ == end_)
                return *this;
            Iter it = cur_;
            Iter next = it;
            ++next;
            // scan this chunk
            while (next != end_ && std::invoke(*pred_, *it, *next)) {
                ++it;
                ++next;
            }
            // step to next chunk
            cur_ = ++it;
            return *this;
        }

        void operator++(int) { ++(*this); }

        bool operator==(const iterator &other) const { return cur_ == other.cur_; }
    };

    iterator begin() {
        return iterator{std::ranges::begin(base_), std::ranges::end(base_), &pred_};
    }

    iterator end() { return iterator{std::ranges::end(base_), std::ranges::end(base_), &pred_}; }
};

template <class R, class P>
chunk_by_view(R &&, P) -> chunk_by_view<std::ranges::views::all_t<R>, P>;

struct chunk_by_fn {
    template <std::ranges::viewable_range R, class P> auto operator()(R &&r, P pred) const {
        return chunk_by_view(std::forward<R>(r), std::move(pred));
    }

    template <std::ranges::viewable_range R, class P>
    friend auto operator|(R &&r, const std::pair<chunk_by_fn, P> &self) {
        return chunk_by_view(std::forward<R>(r), self.second);
    }

    template <class P> auto operator()(P pred) const { return std::pair{chunk_by_fn{}, pred}; }
};

inline constexpr chunk_by_fn chunk_by;

struct run_length_fn {
    template <std::ranges::viewable_range R> auto operator()(R &&r) const {
        return std::forward<R>(r) | chunk_by(std::equal_to{}) // group equal consecutive elements
               | std::views::transform([](auto group) {       // convert group → (value,count)
                     return std::pair{*group.begin(),         // the value
                                      static_cast<std::size_t>(std::ranges::distance(group))};
                 });
    }

    // pipeable adaptor
    template <std::ranges::viewable_range R>
    friend auto operator|(R &&r, const run_length_fn &self) {
        return self(std::forward<R>(r));
    }
};

inline constexpr run_length_fn run_length;

} // namespace prelude
