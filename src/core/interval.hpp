#pragma once

#include <vector>

namespace sfx {
    namespace interval {

        template <typename T>
        struct interval
        {
            T begin;    /**< Included lower bound */
            T end;      /**< Excluded upper bound */

            explicit constexpr operator bool () const { return begin < end; }
        };

        template <typename T>
        constexpr interval<T> operator>> (interval<T> i, T t) 
            { return interval<T>{i.begin + t, i.end + t}; }
        template <typename T>
        constexpr interval<T> operator<< (interval<T> i, T t) 
            { return interval<T>{i.begin - t, i.end - t}; }
        
        template <typename T, typename F>
        constexpr interval<F> convert (interval<T> i, F f) 
            { return interval<F>{static_cast<F>(i.begin * f), static_cast<F>(i.end * f)}; }
            
        template <typename T>
        constexpr bool contains(interval<T> i, T t) { return i.begin <= t && t < i.end; }
        template <typename T, typename ...Ts>
        constexpr bool contains(interval<T> i, T t, Ts... ts) { return contains(i, t) && contains(i, ts...); }

        template <typename T>
        constexpr interval<T> intersection (interval<T> a, interval<T> b)
            { return interval<T>{std::max(a.begin, b.begin), std::min(a.end, b.end)}; }
        template <typename T, typename ...Ts>
        constexpr interval<T> intersection (interval<T> a, interval<T> b, interval<Ts>... v)
            { return intersection(intersection(a, b), v...); }
    }
}
