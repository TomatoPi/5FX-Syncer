#pragma once

#include "strong-types.hpp"
#include <chrono>
#include <cassert>

namespace sfx {
    namespace time {

        /* Metaprogramming utils */

        namespace meta {
            template <typename ...>
            struct traits {};
        }

        template <typename Repr>
        struct timestamp {
            typename meta::traits<Repr>::repr_type repr;
            typename meta::traits<Repr>::base_type base;
            explicit constexpr operator bool() const
                { return static_cast<bool>(base); }
        };

        namespace meta {
            template <typename Repr>
            struct traits<timestamp<Repr>> {
                /* TODO */
            };
            
            template <typename Repr, typename Base, typename Ratio>
            struct make_time_unit {
                using stamp_type = timestamp<Repr>;
                using repr_type  = Repr;
                using base_type  = Base;
                using base_ratio = Ratio;
            };
        }

        /* Hardcoded constants */

        static constexpr const int32_t ppqn = 960;

        /* Score dependant time */

        struct bpm  : strong_type<float, bpm> {
            explicit constexpr operator bool() const
                { return v != 0; }
        };
        struct tick : strong_type<std::int32_t, tick> {};

        namespace meta {
            /** 1BPM == *ppqn* ticks per 60 seconds */
            using score_time_traits = make_time_unit<tick, bpm, std::ratio<ppqn, 60>>;
            template <> struct traits<tick> : score_time_traits {};
            template <> struct traits<bpm>  : score_time_traits {};
        }

        /* Runtime dependant time */

        struct samplerate : strong_type<std::intmax_t, samplerate> {
            explicit constexpr operator bool() const
                { return v != 0; }
        };
        struct frame : strong_type<std::intmax_t, frame> {};

        namespace meta {
            /** 1Hz == 1 frame per 1 second */
            using play_time_traits = make_time_unit<frame, samplerate, std::ratio<1>>;
            template <> struct traits<frame>        : play_time_traits {};
            template <> struct traits<samplerate>   : play_time_traits {};
        }

        /* Generic timepoint */

        template <typename Repr>
        constexpr timestamp<Repr> sanitise(timestamp<Repr> t)
            { return t ? t : timestamp<Repr>{}; }

        template <
            typename To,
            typename From,
            typename MT = meta::traits<To>,
            typename MF = meta::traits<From>
            >
        constexpr typename MT::stamp_type convert(typename MF::stamp_type ftime, typename MT::base_type tbase)
        {
            assert(ftime); assert(tbase);
            using factor = std::ratio<
                MF::base_ratio::den * MT::base_ratio::num,
                MF::base_ratio::num * MT::base_ratio::den>;
            auto num = tbase.v * ftime.repr.v * factor::num;
            auto den = ftime.base.v * factor::den;
            typename To::type value = static_cast<typename To::type>(num / den);
            return {value, tbase};
        }
        
        // template <typename To, typename From>
        // constexpr typename To::timebase convert_delta(timestamp<From> deltaf, To deltat)
        // {
        //     assert(t); assert(tr != typename To::timebase{});
        //     using factor = std::ratio<
        //         From::timebase::ratio::den * To::timebase::ratio::num,
        //         From::timebase::ratio::num * To::timebase::ratio::den>;
        //     auto num = tr.v * t.repr.v * factor::num;
        //     auto den = t.base.v * factor::den;
        //     typename To::type value = static_cast<typename To::type>(num / den);
        //     return {value, tr};
        // }

        template <typename Repr>
        constexpr auto remap = convert<Repr, Repr>;

        /* Relational operators */

        /**
         * @brief Takes two timestamps and remap them to the gcd of rate a and b
         * @return a pair of timestamps (a', b') remaped to a rate of max(a.rate, b.rate)
         * @note It may exist some corner cases where two slightly different timestamps are aliased as equals
         */
        template <
            typename Repr,
            typename Timestamp = typename meta::traits<Repr>::stamp_type
            >
        constexpr std::pair<Timestamp, Timestamp> align_bases(Timestamp a, Timestamp b)
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
            auto [a_, b_] = align_bases<Repr>(a, b);
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
                { assert(*this); return (atime + convert<AbsRepr>(rs - rtime, atime.base)); }
            constexpr AbsRepr operator() (RelRepr r) const
                { assert(*this); return this->operator()(rstamp{r, rtime.base}).repr; }
            /*  */
            constexpr rstamp operator() (astamp as) const
                { return syncpoint<AbsRepr, RelRepr>{atime, rtime}(as); }
            constexpr RelRepr operator() (AbsRepr a) const
                { return syncpoint<AbsRepr, RelRepr>{atime, rtime}(a); }
            /*  */
            constexpr operator bool() const
                { return rtime && atime; }
        };

        /* Conveniant typedefs */

        using tickstamp = timestamp<tick>;
        using framestamp = timestamp<frame>;

        using syncer = syncpoint<tick, frame>;

        constexpr auto frame_to_tick = convert<tick, frame>;
        constexpr auto tick_to_frame = convert<frame, tick>;
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