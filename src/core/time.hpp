#pragma once

#include "strong-types.hpp"
#include <numeric>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <chrono>

namespace sfx {
    namespace time {

        /* Generic time measurement */

        class ratio {
        private :
            std::intmax_t n; /**< numerator */
            std::intmax_t d; /**< denominator */
        public :
            constexpr ratio(std::intmax_t num = 1, std::intmax_t den = 1) :
                    n{((den < 0 ? -1 : 1) * num) / std::gcd(num, den)},
                    d{(den * (den < 0 ? -1 : 1)) / std::gcd(num, den)}
                { assert(den != 0); }

            constexpr std::intmax_t num() const { return n; }
            constexpr std::intmax_t den() const { return d; }
            constexpr ratio inverse() const { return ratio(d, n); }

            constexpr friend ratio operator* (ratio a, ratio b)
                { return ratio(a.n * b.n, a.d * b.d); }
            constexpr friend bool operator== (ratio a, ratio b)
                { return (a.num() == b.num()) && (a.den() == b.den()); }
            constexpr friend bool operator< (ratio a, ratio b)
                { return (a.num() * b.den()) < (a.den() * b.num()); }

            constexpr friend ratio operator* (ratio a, std::intmax_t f)
                { return ratio(a.n * f, a.d); }
            constexpr friend ratio operator* (std::intmax_t f, ratio a)
                { return a * f; }
            constexpr friend ratio operator/ (ratio a, std::intmax_t f)
                { return ratio(a.n, a.d * f); }
            constexpr friend ratio operator/ (std::intmax_t f, ratio a)
                { return ratio(f * a.d, a.n); }

            explicit constexpr operator float() const 
                { return static_cast<float>(n) / static_cast<float>(d); }
            explicit constexpr operator double() const
                { return static_cast<double>(n) / static_cast<double>(d); }
            explicit constexpr operator std::intmax_t() const
                { return n / d; }
        };

        struct timebase : strong_type<ratio, timebase> {
            constexpr bool is_valid() const 
                { return v.num() != 0 && v.den() != 0; }
                
            constexpr friend timebase operator* (timebase a, std::intmax_t f)
                { return timebase{ratio(a.v.num() * f, a.v.den())}; }
            constexpr friend timebase operator* (std::intmax_t f, timebase a)
                { return a * f; }
        };

        template <typename Repr>
        class duration {
        public :
            using repr = Repr;

            /** Ctors **/

            constexpr duration(
                Repr v = static_cast<Repr>(0),
                timebase p = timebase(1, 1))
                : _value(v), _period(p)
                {}
            constexpr duration(duration other, timebase p)
                : duration(other.value(p), p)
                {}

            /** Accessors **/

            constexpr bool is_valid() const 
                { return _period.is_valid(); }

            constexpr duration sanitised() const
                { return is_valid() ? *this : duration(); }

            constexpr Repr value() const
                { return _value; }
            constexpr Repr value(timebase p) const
                { return rebase(_value, _period, p).value(); }

            constexpr timebase period() const
                { return _period; }
            constexpr timebase as_base() const
                { return duration_to_timebase(_value, _period); }

            /** Utility **/

            static constexpr std::pair<duration, duration> align_bases(
                duration a, duration b)
            {
                if (a.period() == b.period())
                    return {a, b};
                if (a.period() < b.period())
                    return {duration(a, b.period()), b};
                else
                    return {a, duration(b, a.period())};
            }

            /** Relational operators **/

            constexpr friend bool operator== (duration a, duration b)
            {
                if (!a.is_valid() || !b.is_valid()) return false;
                auto [_a, _b] = align_bases(a, b);
                return _a.value() == _b.value();
            }

            constexpr friend bool operator< (duration a, duration b)
            {
                if (!a.is_valid() || !b.is_valid()) return false;
                auto [_a, _b] = align_bases(a, b);
                return _a.value() < _b.value();
            }

            /** Arithmetic operators **/

            constexpr friend duration operator+ (duration a, duration b)
            {
                assert(a.is_valid());
                assert(b.is_valid());
                auto [_a, _b] = align_bases(a, b);
                return {_a.value() + _b.value(), _a.period()};
            }
            constexpr friend duration operator+ (duration a, repr t)
            {
                assert(a.is_valid());
                return {a.value() + t, a.period()};
            }

            constexpr friend duration operator- (duration a, duration b)
            {
                assert(a.is_valid());
                assert(b.is_valid());
                auto [_a, _b] = align_bases(a, b);
                return {_a.value() - _b.value(), _a.period()};
            }
            constexpr friend duration operator- (duration a, repr t)
            {
                assert(a.is_valid());
                return {a.value() - t, a.period()};
            }

        private :

            static constexpr duration rebase(
                repr val, timebase fromb, timebase tob)
            {
                assert(fromb.is_valid()); assert(tob.is_valid());
                const ratio factor(fromb.v.inverse() * tob.v);
                auto num = val * factor.num();
                auto den = factor.den();
                return {static_cast<repr>(num / den), tob};
            }
            
            static constexpr timebase duration_to_timebase(
                repr delta, timebase fromb)
            {
                assert(delta != static_cast<repr>(0));
                assert(fromb.is_valid());
                auto num = delta * fromb.v.den();
                auto den = fromb.v.num();
                return timebase{ratio(num, den)};
            }

            Repr     _value;
            timebase _period;
        };

        // /* Sync utilities */

        // template <typename RelRepr, typename AbsRepr>
        // struct syncpoint {
        //     using rstamp = timestamp<RelRepr>;
        //     using astamp = timestamp<AbsRepr>;
        //     rstamp rtime; /**< Relative timestamp offset */
        //     astamp atime; /**< Absolute timestamp offset */
        //     /*  */
        //     constexpr astamp operator() (rstamp rs) const
        //         { assert(*this); return (atime + to_stamp<AbsRepr>(rs - rtime, atime.base)); }
        //     constexpr AbsRepr operator() (RelRepr r) const
        //         { assert(*this); return this->operator()(rstamp{r, rtime.base}).repr; }
        //     /*  */
        //     constexpr rstamp operator() (astamp as) const
        //         { assert(*this); return syncpoint<AbsRepr, RelRepr>{atime, rtime}(as); }
        //     constexpr RelRepr operator() (AbsRepr a) const
        //         { assert(*this); return syncpoint<AbsRepr, RelRepr>{atime, rtime}(a); }
        //     /*  */
        //     constexpr operator bool() const
        //         { return rtime && atime; }
        // };

        // /* Conveniant typedefs */

        // using tickstamp = timestamp<tick>;
        // using framestamp = timestamp<frame>;

        // using syncer = syncpoint<tick, frame>;

        // constexpr auto frame_to_tick = to_stamp<tick, frame>;
        // constexpr auto tick_to_frame = to_stamp<frame, tick>;
    }
}

constexpr sfx::time::timebase operator"" _bpm(unsigned long long int b)
    { return {sfx::time::ratio{static_cast<std::intmax_t>(b), 60}}; }
constexpr sfx::time::timebase operator"" _Hz(unsigned long long int sr)
    { return {sfx::time::ratio{static_cast<std::intmax_t>(sr), 1}}; }
constexpr sfx::time::timebase operator"" _kHz(unsigned long long int sr)
    { return {sfx::time::ratio{static_cast<std::intmax_t>(sr) * 1000, 1}}; }