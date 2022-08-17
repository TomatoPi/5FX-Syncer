#pragma once

#include "strong-types.hpp"
#include <chrono>

namespace sfx {
    namespace time {

        struct tick : strong_type<float, tick> {};
        struct bpm  : strong_type<float, bpm> {
            /** 1BPM == 24 ticks per 60 seconds (Hardcoded 24PPQN) */
            using base = std::ratio<24, 60>;
        };
        constexpr bpm operator"" _bpm(long double b)
            { return bpm{static_cast<bpm::type>(b)}; }
        constexpr bpm operator"" _bpm(unsigned long long int b)
            { return bpm{static_cast<bpm::type>(b)}; }
        
        struct frame        : strong_type<std::intmax_t, frame> {};
        struct samplerate   : strong_type<std::intmax_t, samplerate> {
            /**< 1Hz == 1 frame per 1 second */
            using base = std::ratio<1,1>;
        };
        constexpr samplerate operator"" _Hz(unsigned long long int sr)
            { return samplerate{static_cast<samplerate::type>(sr)}; }
        constexpr samplerate operator"" _kHz(unsigned long long int sr)
            { return samplerate{static_cast<samplerate::type>(sr) * 1000}; }

        template <typename Repr, typename Rate>
        struct timestamp {
            using repr = Repr;
            using rate = Rate;
            repr value;
            rate base;
        };

        using ticktime = timestamp<tick, bpm>;
        using frametime = timestamp<frame, samplerate>;
        
        template <typename To, typename ToBase, typename From, typename FromBase>
        constexpr timestamp<To, ToBase> convert(timestamp<From, FromBase> t, ToBase tr)
        {
            using factor = std::ratio<FromBase::base::den * ToBase::base::num, FromBase::base::num * ToBase::base::den>;
            typename To::type value = static_cast<typename To::type>((tr.v * t.value.v * factor::num) / (t.base.v * factor::den));
            return timestamp<To, ToBase>{value, tr};
        }

        constexpr auto frame_to_tick = convert<tick, bpm, frame, samplerate>;
        constexpr auto tick_to_frame = convert<frame, samplerate, tick, bpm>;

        template <typename T>
        constexpr auto remap = convert<typename T::repr, typename T::rate, typename T::repr, typename T::rate>;

        // constexpr bool operator<

        // struct syncpoint {
        //     ticktime    t;
        //     frametime   f;
        // };
    }
}
