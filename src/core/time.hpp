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
        
        struct frame        : strong_type<uint32_t, frame> {};
        struct samplerate   : strong_type<uint32_t, samplerate> {
            /**< 1Hz == 1 frame per 1 second */
            using base = std::ratio<1,1>;
        };

        template <typename Repr, typename Rate>
        struct timestamp {
            using repr = Repr;
            using rate = Rate;
            repr value;
            rate base;
        };

        using ticktime = timestamp<tick, bpm>;
        using frametime = timestamp<frame, samplerate>;
        
        template <typename From, typename FromBase, typename To, typename ToBase>
        constexpr timestamp<To, ToBase> convert(timestamp<From, FromBase> t, ToBase tr)
        {
            using factor = std::ratio<FromBase::base::den * ToBase::base::num, FromBase::base::num * ToBase::base::den>;
            typename To::type value = static_cast<typename To::type>((tr.v * t.value.v * factor::num) / (t.base.v * factor::den));
            return timestamp<To, ToBase>{value, tr};
        }

        constexpr auto tick_to_frame = convert<tick, bpm, frame, samplerate>;
        constexpr auto frame_to_tick = convert<frame, samplerate, tick, bpm>;
    }
}