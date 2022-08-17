#include "../src/core/time.hpp"

#include <iostream>
#include <cassert>

#define BIASSERT(expr) static_assert(expr); assert((expr));

int main(int argc, char * const argv[])
{
    using namespace sfx::time;

    /* string literals */

    BIASSERT(samplerate{48'000} == 48_kHz);
    BIASSERT(samplerate{48'000} == 48000_Hz);

    BIASSERT(bpm{60} == 60.0_bpm);
    BIASSERT(bpm{60} == 60_bpm);

    /* basic conversions */

    constexpr framestamp one_sec_f_48{48000, 48_kHz};  /* 48k samples at 48kHz == 1s */
    constexpr framestamp one_sec_f_96{96000, 96_kHz};  /* 48k samples at 48kHz == 1s */
    constexpr tickstamp one_sec_t_60 {ppqn, 60_bpm};   /* 24 ticks == 1beat == 1s at 60bpm */
    constexpr tickstamp one_sec_t_120{ppqn * 2, 120_bpm}; /* 48 ticks == 2beat == 1s at 120bpm */

    BIASSERT(tick_to_frame(one_sec_t_60, 48_kHz).repr == 48'000);
    BIASSERT(tick_to_frame(one_sec_t_120, 48_kHz).repr == 48'000);
    BIASSERT(frame_to_tick(one_sec_f_48, 60_bpm).repr == ppqn);
    BIASSERT(frame_to_tick(one_sec_f_48, 120_bpm).repr == ppqn * 2);

    /* basic remaping */

    BIASSERT(remap<frame>(one_sec_f_48, 96_kHz).repr == 96'000);
    BIASSERT(remap<frame>(one_sec_f_96, 48_kHz).repr == 48'000);
    BIASSERT(remap<tick>(one_sec_t_60, 120_bpm).repr == ppqn * 2);
    BIASSERT(remap<tick>(one_sec_t_120, 60_bpm).repr == ppqn);

    /* Check numeric stability at highest samplerate */

    BIASSERT(remap<frame>(framestamp{196'000, 196_kHz}, 196_kHz).repr == 196'000);
    BIASSERT(remap<frame>(framestamp{196'000, 196_kHz}, 48_kHz).repr == 48'000);
    BIASSERT(remap<frame>(one_sec_f_48, 196_kHz).repr == 196'000);

    /* The FINAL test, check stability for at least 72 hours at maximal samplerate */

    BIASSERT(remap<frame>(framestamp{50'803'200'000, 196_kHz}, 196_kHz).repr == 50'803'200'000);
    BIASSERT(remap<frame>(framestamp{24'883'200'000, 96_kHz},  196_kHz).repr == 50'803'200'000);
    BIASSERT(remap<frame>(framestamp{12'441'600'000, 48_kHz},  196_kHz).repr == 50'803'200'000);

    BIASSERT(tick_to_frame(tickstamp{248'832'000,  60_bpm}, 196_kHz).repr  == 50'803'200'000);
    BIASSERT(tick_to_frame(tickstamp{995'328'000, 240_bpm}, 196_kHz).repr == 50'803'200'000);

    BIASSERT(frame_to_tick(framestamp{50'803'200'000, 196_kHz}, 240_bpm).repr == 995'328'000);

    /* Test relational operators */

    BIASSERT((framestamp{0, 48_kHz} == framestamp{0, 196_kHz}) && true);
    BIASSERT((framestamp{196'000, 196_kHz} == framestamp{48'000, 48_kHz}) && true);
    BIASSERT((framestamp{44'100, 44'100_Hz} == framestamp{48'000, 48_kHz}) && true);
    BIASSERT((framestamp{24'000, 44'100_Hz} < framestamp{48'000, 48_kHz}) && true);
    BIASSERT((framestamp{96'000, 196'000_Hz} < framestamp{40'000, 48_kHz}) && true);

    BIASSERT(!(framestamp{95'999, 96_kHz} == framestamp{47'999, 48_kHz}) && true);
    BIASSERT(!(framestamp{95'999, 96_kHz} < framestamp{47'999, 48_kHz}) && true);

    BIASSERT((tickstamp{0, 120_bpm}  == tickstamp{0, 240_bpm}) && true);
    BIASSERT((tickstamp{60, 120_bpm} == tickstamp{120, 240_bpm}) && true);
    BIASSERT((tickstamp{90, 120_bpm} == tickstamp{180, 240_bpm}) && true);
    BIASSERT((tickstamp{90, 120_bpm} < tickstamp{200, 240_bpm}) && true);
    BIASSERT((tickstamp{66, 240_bpm} < tickstamp{55, 60_bpm}) && true);

    return 0;
}
