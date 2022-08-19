#pragma once

#include "strong-types.hpp"
#include <chrono>
#include <cassert>

namespace sfx {
    namespace time {

        /* Hardcoded constants */

        static constexpr const int32_t ppqn = 960;

        /* Score dependant time */

        struct bpm  : strong_type<float, bpm> {
            /** 1BPM == *ppqn* ticks per 60 seconds */
            using ratio = std::ratio<ppqn, 60>;
            explicit constexpr operator bool() const
                { return *this > 0.f; }
        };
        struct tick : strong_type<std::int32_t, tick> {
            using timebase = bpm;
            explicit constexpr operator bool() const
                { return *this != 0; }
        };

        /* Runtime dependant time */

        struct samplerate : strong_type<std::intmax_t, samplerate> {
            /**< 1Hz == 1 frame per 1 second */
            using ratio = std::ratio<1,1>;
            explicit constexpr operator bool() const
                { return *this > 0; }
        };
        struct frame : strong_type<std::intmax_t, frame> {
            using timebase = samplerate;
            explicit constexpr operator bool() const
                { return *this != 0; }
        };

        /* Generic timepoint */

        template <typename Repr>
        struct timestamp {
            using repr_type = Repr;
            using timebase = typename Repr::timebase;
            repr_type repr;
            timebase base;
            explicit constexpr operator bool() const
                { return static_cast<bool>(base); }
        };

        template <typename Repr>
        constexpr timestamp<Repr> sanitise(timestamp<Repr> t)
            { return t ? t : timestamp<Repr>{}; }

        template <typename To, typename From>
        constexpr timestamp<To> to_stamp(timestamp<From> ftime, typename To::timebase tbase)
        {
            assert(ftime); assert(tbase);
            using factor = std::ratio<
                From::timebase::ratio::den * To::timebase::ratio::num,
                From::timebase::ratio::num * To::timebase::ratio::den>;
            auto num = tbase.v * ftime.repr.v * factor::num;
            auto den = ftime.base.v * factor::den;
            auto value = static_cast<typename To::type>(num / den);
            return {value, tbase};
        }

        template <typename Repr>
        constexpr auto remap = to_stamp<Repr, Repr>;

        template <typename ToRepr, typename From>
        constexpr typename ToRepr::timebase to_base(timestamp<From> deltaf, ToRepr deltat = ToRepr{1})
        {
            assert(deltaf); assert(deltat);
            using factor = std::ratio<
                From::timebase::ratio::den * ToRepr::timebase::ratio::num,
                From::timebase::ratio::num * ToRepr::timebase::ratio::den>;
            auto num = deltaf.base.v * factor::den * deltat.v;
            auto den = deltaf.repr.v * factor::num;
            return {static_cast<typename ToRepr::timebase::type>(num) / den};
        }

        /* Relational operators */

        /**
         * @brief Takes two timestamps and remap them to the gcd of rate a and b
         * @return a pair of timestamps (a', b') remaped to a rate of max(a.rate, b.rate)
         * @note It may exist some corner cases where two slightly different timestamps are aliased as equals
         */
        template <typename Repr>
        constexpr std::pair<timestamp<Repr>, timestamp<Repr>> align_bases(timestamp<Repr> a, timestamp<Repr> b)
        {
            assert(a); assert(b);
            if (a.base == b.base)
                return {a, b};
            if (a.base < b.base)
                return {remap<Repr>(a, b.base), b};
            else
                return {a, remap<Repr>(b, a.base)};
        }

        template <typename Repr>
        constexpr bool operator== (timestamp<Repr> a, timestamp<Repr> b)
        {
            // if (!a) return static_cast<bool>(b);
            // if (!b) return a.repr == Repr{};
            if (!a || !b) return false;
            auto [a_, b_] = align_bases(a, b);
            return a_.repr == b_.repr;
        }

        template <typename Repr>
        constexpr bool operator< (timestamp<Repr> a, timestamp<Repr> b)
        {
            // if (!a) return b && Repr{} < b.repr;
            // if (!b) return a.repr < Repr{};
            if (!a || !b) return false;
            auto [a_, b_] = align_bases(a, b);
            return a_.repr < b_.repr;
        }

        /* Arithmetical operator */

        template <typename Repr>
        constexpr timestamp<Repr> operator+ (timestamp<Repr> a, timestamp<Repr> b)
        {
            // if (!a) return sanitise(b);
            // if (!b) return sanitise(a);
            assert(a); assert(b);
            auto [a_, b_] = align_bases(a, b);
            return {a_.repr + b_.repr, a_.base};
        }
        template <typename Repr>
        constexpr timestamp<Repr> operator+ (timestamp<Repr> a, Repr t)
            { assert(a); return {a.repr + t, a.base}; }

        template <typename Repr>
        constexpr timestamp<Repr> operator- (timestamp<Repr> a, timestamp<Repr> b)
        {
            // if (!a) return sanitise(b);
            // if (!b) return sanitise(a);
            assert(a); assert(b);
            auto [a_, b_] = align_bases(a, b);
            return {a_.repr - b_.repr, a_.base};
        }
        template <typename Repr>
        constexpr timestamp<Repr> operator- (timestamp<Repr> a, Repr t)
            { assert(a); return {a.repr - t, a.base}; }

        /* Sync utilities */

        template <typename RelRepr, typename AbsRepr>
        struct syncpoint {
            using rstamp = timestamp<RelRepr>;
            using astamp = timestamp<AbsRepr>;
            rstamp rtime; /**< Relative timestamp offset */
            astamp atime; /**< Absolute timestamp offset */
            /*  */
            constexpr astamp operator() (rstamp rs) const
                { assert(*this); return (atime + to_stamp<AbsRepr>(rs - rtime, atime.base)); }
            constexpr AbsRepr operator() (RelRepr r) const
                { assert(*this); return this->operator()(rstamp{r, rtime.base}).repr; }
            /*  */
            constexpr rstamp operator() (astamp as) const
                { assert(*this); return syncpoint<AbsRepr, RelRepr>{atime, rtime}(as); }
            constexpr RelRepr operator() (AbsRepr a) const
                { assert(*this); return syncpoint<AbsRepr, RelRepr>{atime, rtime}(a); }
            /*  */
            constexpr operator bool() const
                { return rtime && atime; }
        };

        /* Conveniant typedefs */

        using tickstamp = timestamp<tick>;
        using framestamp = timestamp<frame>;

        using syncer = syncpoint<tick, frame>;

        constexpr auto frame_to_tick = to_stamp<tick, frame>;
        constexpr auto tick_to_frame = to_stamp<frame, tick>;
    }
}

constexpr sfx::time::bpm operator"" _bpm(long double b)
    { return {static_cast<sfx::time::bpm::type>(b)}; }
constexpr sfx::time::bpm operator"" _bpm(unsigned long long int b)
    { return {static_cast<sfx::time::bpm::type>(b)}; }

constexpr sfx::time::samplerate operator"" _Hz(unsigned long long int sr)
    { return {static_cast<sfx::time::samplerate::type>(sr)}; }
constexpr sfx::time::samplerate operator"" _kHz(unsigned long long int sr)
    { return {static_cast<sfx::time::samplerate::type>(sr) * 1000}; }