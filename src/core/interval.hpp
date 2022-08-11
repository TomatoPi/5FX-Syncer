#pragma once

#include <vector>

namespace sfx {
    namespace interval {

        template <typename T>
        struct interval
        {
            T begin;    /**< Included lower bound */
            T end;      /**< Excluded upper bound */

            explicit constexpr operator bool () const noexcept
                { return begin < end; }
        };

        template <typename T>
        struct shift
        {
            T diff;
            constexpr interval<T> operator() (interval<T> i) const noexcept
                { return interval<T>{i.begin + diff, i.end + diff}; }
        };

        template <typename T>
        struct contains
        {
            T val;
            constexpr bool operator() (interval<T> i) const noexcept
                { return i.begin <= val && val < i.end; }
        };

        template <typename T>
        struct intersect
        {
            constexpr interval<T> operator() (interval<T> a, interval<T> b) const noexcept
                { return interval<T>{std::max(a.begin, b.begin), std::min(a.end, b.end)}; }
        };
    }
}
