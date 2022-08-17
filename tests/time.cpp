#include "../src/core/time.hpp"

#include <iostream>
#include <cassert>

int main(int argc, char * const argv[])
{
    using namespace sfx::time;

    constexpr samplerate _48kHz{48000};
    constexpr samplerate _92kHz{92000};
    constexpr bpm _60bpm{60};
    constexpr bpm _120bpm{120};

    constexpr frametime one_sec_f{48000, _48kHz};  /* 48k samples at 48kHz == 1s */
    constexpr ticktime one_sec_t_60{24, _60bpm};   /* 24 ticks == 1beat == 1s at 60bpm */
    constexpr ticktime one_sec_t_120{48, _120bpm}; /* 48 ticks == 2beat == 1s at 120bpm */

    static_assert(_48kHz == 48_kHz);
    static_assert(_48kHz == 48000_Hz);

    static_assert(_60bpm == 60.0_bpm);
    static_assert(_60bpm == 60_bpm);

    static_assert(tick_to_frame(one_sec_t_60, _48kHz).value == one_sec_f.value);
    static_assert(tick_to_frame(one_sec_t_120, _48kHz).value == one_sec_f.value);
    static_assert(frame_to_tick(one_sec_f, _60bpm).value == one_sec_t_60.value);
    static_assert(frame_to_tick(one_sec_f, _120bpm).value == one_sec_t_120.value);

    return 0;
}