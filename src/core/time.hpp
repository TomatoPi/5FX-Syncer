#pragma once

#include "strong-types.hpp"
#include <chrono>

namespace sfx {
    namespace time {

        /* Hardcoded constants */

        static constexpr const int32_t ppqn = 960;

        /* Score dependant time */

        struct bpm  : strong_type<float, bpm> {
            /** 1BPM == *ppqn* ticks per 60 seconds */
            using ratio = std::ratio<ppqn, 60>;
        };
        struct tick : strong_type<std::int32_t, tick> {
            using timebase = bpm;
        };

        constexpr bpm operator"" _bpm(long double b)
            { return bpm{static_cast<bpm::type>(b)}; }
        constexpr bpm operator"" _bpm(unsigned long long int b)
            { return bpm{static_cast<bpm::type>(b)}; }
        
        /* Runtime dependant time */

        struct samplerate : strong_type<std::intmax_t, samplerate> {
            /**< 1Hz == 1 frame per 1 second */
            using ratio = std::ratio<1,1>;
        };
        struct frame : strong_type<std::intmax_t, frame> {
            using timebase = samplerate;
        };

        constexpr samplerate operator"" _Hz(unsigned long long int sr)
            { return samplerate{static_cast<samplerate::type>(sr)}; }
        constexpr samplerate operator"" _kHz(unsigned long long int sr)
            { return samplerate{static_cast<samplerate::type>(sr) * 1000}; }

        /* Generic timepoint */

        template <typename Repr>
        struct timestamp {
            using repr_type = Repr;
            using timebase = typename Repr::timebase;
            repr_type repr;
            timebase base;
        };

        
        template <typename To, typename From>
        constexpr timestamp<To> convert(timestamp<From> t, typename To::timebase tr)
        {
            using factor = std::ratio<
                From::timebase::ratio::den * To::timebase::ratio::num,
                From::timebase::ratio::num * To::timebase::ratio::den>;
            typename To::type value = static_cast<typename To::type>((tr.v * t.repr.v * factor::num) / (t.base.v * factor::den));
            return {value, tr};
        }

        template <typename Repr>
        constexpr auto remap = convert<Repr, Repr>;

        /* Relational operators */

        /**
         * @brief Takes two timestamps and remap them to the gcd of rate a and b
         * @return a pair of timestamps (a', b') remaped to a rate of max(a.rate, b.rate)
         * @note It may exist some corner cases where two slightly different timestamps are aliased as equals
         */
        template <typename Repr>
        constexpr std::pair<timestamp<Repr>, timestamp<Repr>> align_bases(timestamp<Repr> a, timestamp<Repr> b)
        {
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
            auto [a_, b_] = align_bases(a, b);
            return a_.repr == b_.repr;
        }

        template <typename Repr>
        constexpr bool operator< (timestamp<Repr> a, timestamp<Repr> b)
        {
            auto [a_, b_] = align_bases(a, b);
            return a_.repr < b_.repr;
        }

        /* Conveniant typedefs */

        using tickstamp = timestamp<tick>;
        using framestamp = timestamp<frame>;

        constexpr auto frame_to_tick = convert<tick, frame>;
        constexpr auto tick_to_frame = convert<frame, tick>;
    }
}
