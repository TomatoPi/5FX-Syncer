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
        };

        struct timebase : strong_type<ratio, timebase> {
            constexpr bool is_valid() const 
                { return v.num() != 0 && v.den() != 0; }
        };

        template <typename Repr>
        class duration {
        public :
            using repr = Repr;

            constexpr duration(
                Repr v = static_cast<Repr>(0),
                timebase p = timebase(1, 1))
                : _value(v), _period(p)
                {}
            constexpr duration(duration other, timebase p)
                : duration(rebase(other._value, other._period, p).value().v, p)
                {}

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

            static constexpr std::pair<duration, duration> align_bases(
                duration a, duration b)
            {

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

        /* Generic timestamp conversions */

        /* Usual Timestamps definitions */

        // struct bpm : timebase<float> {};
        // struct samplerate : timebase<std::intmax_t> {};

        // /* Score dependant time */

        // struct bpm : strong_type<float, bpm> {
        //     /** 1BPM == *ppqn* ticks per 60 seconds */
        //     using ratio = std::ratio<ppqn, 60>;
        //     explicit constexpr operator bool() const
        //         { return *this > 0.f; }
        // };
        // struct tick : strong_type<std::int32_t, tick> {
        //     using timebase = bpm;
        //     explicit constexpr operator bool() const
        //         { return *this != 0; }
        // };

        // /* Runtime dependant time */

        // struct samplerate : strong_type<std::intmax_t, samplerate> {
        //     /**< 1Hz == 1 frame per 1 second */
        //     using ratio = std::ratio<1,1>;
        //     explicit constexpr operator bool() const
        //         { return *this > 0; }
        // };
        // struct frame : strong_type<std::intmax_t, frame> {
        //     using timebase = samplerate;
        //     explicit constexpr operator bool() const
        //         { return *this != 0; }
        // };

        // /* Generic timepoint */

        // template <typename Repr>
        // struct timestamp {
        //     using repr_type = Repr;
        //     using timebase = typename Repr::timebase;
        //     repr_type repr;
        //     timebase base;
        //     explicit constexpr operator bool() const
        //         { return static_cast<bool>(base); }
        // };

        // template <typename Repr>
        // constexpr timestamp<Repr> sanitise(timestamp<Repr> t)
        //     { return t ? t : timestamp<Repr>{}; }

        // template <typename To, typename From>
        // constexpr timestamp<To> to_stamp(timestamp<From> ftime, typename To::timebase tbase)
        // {
        //     assert(ftime); assert(tbase);
        //     using factor = std::ratio<
        //         From::timebase::ratio::den * To::timebase::ratio::num,
        //         From::timebase::ratio::num * To::timebase::ratio::den>;
        //     auto num = tbase.v * ftime.repr.v * factor::num;
        //     auto den = ftime.base.v * factor::den;
        //     auto value = static_cast<typename To::type>(num / den);
        //     return {value, tbase};
        // }

        // template <typename Repr>
        // constexpr auto remap = to_stamp<Repr, Repr>;

        // template <typename ToRepr, typename From>
        // constexpr typename ToRepr::timebase to_base(timestamp<From> deltaf, ToRepr deltat = ToRepr{1})
        // {
        //     assert(deltaf); assert(deltat);
        //     using factor = std::ratio<
        //         From::timebase::ratio::den * ToRepr::timebase::ratio::num,
        //         From::timebase::ratio::num * ToRepr::timebase::ratio::den>;
        //     auto num = deltaf.base.v * factor::den * deltat.v;
        //     auto den = deltaf.repr.v * factor::num;
        //     return {static_cast<typename ToRepr::timebase::type>(num) / den};
        // }

        // /* Relational operators */

        // /**
        //  * @brief Takes two timestamps and remap them to the gcd of rate a and b
        //  * @return a pair of timestamps (a', b') remaped to a rate of max(a.rate, b.rate)
        //  * @note It may exist some corner cases where two slightly different timestamps are aliased as equals
        //  */
        // template <typename Repr>
        // constexpr std::pair<timestamp<Repr>, timestamp<Repr>> align_bases(timestamp<Repr> a, timestamp<Repr> b)
        // {
        //     assert(a); assert(b);
        //     if (a.base == b.base)
        //         return {a, b};
        //     if (a.base < b.base)
        //         return {remap<Repr>(a, b.base), b};
        //     else
        //         return {a, remap<Repr>(b, a.base)};
        // }

        // template <typename Repr>
        // constexpr bool operator== (timestamp<Repr> a, timestamp<Repr> b)
        // {
        //     // if (!a) return static_cast<bool>(b);
        //     // if (!b) return a.repr == Repr{};
        //     if (!a || !b) return false;
        //     auto [a_, b_] = align_bases(a, b);
        //     return a_.repr == b_.repr;
        // }

        // template <typename Repr>
        // constexpr bool operator< (timestamp<Repr> a, timestamp<Repr> b)
        // {
        //     // if (!a) return b && Repr{} < b.repr;
        //     // if (!b) return a.repr < Repr{};
        //     if (!a || !b) return false;
        //     auto [a_, b_] = align_bases(a, b);
        //     return a_.repr < b_.repr;
        // }

        // /* Arithmetical operator */

        // template <typename Repr>
        // constexpr timestamp<Repr> operator+ (timestamp<Repr> a, timestamp<Repr> b)
        // {
        //     // if (!a) return sanitise(b);
        //     // if (!b) return sanitise(a);
        //     assert(a); assert(b);
        //     auto [a_, b_] = align_bases(a, b);
        //     return {a_.repr + b_.repr, a_.base};
        // }
        // template <typename Repr>
        // constexpr timestamp<Repr> operator+ (timestamp<Repr> a, Repr t)
        //     { assert(a); return {a.repr + t, a.base}; }

        // template <typename Repr>
        // constexpr timestamp<Repr> operator- (timestamp<Repr> a, timestamp<Repr> b)
        // {
        //     // if (!a) return sanitise(b);
        //     // if (!b) return sanitise(a);
        //     assert(a); assert(b);
        //     auto [a_, b_] = align_bases(a, b);
        //     return {a_.repr - b_.repr, a_.base};
        // }
        // template <typename Repr>
        // constexpr timestamp<Repr> operator- (timestamp<Repr> a, Repr t)
        //     { assert(a); return {a.repr - t, a.base}; }

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