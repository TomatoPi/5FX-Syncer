#pragma once

#include <algorithm>

namespace sfx {
    namespace interval {

        /* Interval definition */

        template <typename T>
        struct interval
        {
            T begin;    /**< Included lower bound */
            T end;      /**< Excluded upper bound */

            explicit constexpr operator bool () const noexcept
                { return begin < end; }
        };

        template <typename T>
        constexpr bool operator== (interval<T> a, interval<T> b) noexcept
            { 
                return (!static_cast<bool>(a) && !static_cast<bool>(b)) 
                    || (a.begin == b.begin && a.end == b.end);
            }

        /* Intervals modifiers */

        template <typename T>
        struct shift
        {
            T diff;
            constexpr interval<T> operator() (interval<T> i) const noexcept
                { return interval<T>{i.begin + diff, i.end + diff}; }
        };

        template <typename T>
        struct intersect
        {
            constexpr interval<T> operator() (interval<T> a, interval<T> b) const noexcept
                { return interval<T>{std::max(a.begin, b.begin), std::min(a.end, b.end)}; }
        };

        /* Intervals lookup */

        template <typename T>
        struct contains
        {
            interval<T> i;
            constexpr bool operator() (T t) const noexcept
                { return i.begin <= t && t < i.end; }
        };

        template <typename T>
        struct include
        {
            interval<T> i;
            constexpr bool operator() (interval<T> u) const noexcept
            {
                return !static_cast<bool>(u)
                    || (contains{i}(u.begin) && contains{i}(u.end));
            }
        };
    }
}
