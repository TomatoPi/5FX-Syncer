#include "../src/core/time.hpp"

#include <iostream>
#include <cassert>

#define BIASSERT(expr) static_assert(expr); assert((expr));

int main(int argc, char * const argv[])
{
    using namespace sfx::time;

    constexpr samplerate _48kHz{48000};
    constexpr samplerate _96kHz{96000};
    constexpr bpm _60bpm{60};
    constexpr bpm _120bpm{120};

    constexpr framestamp one_sec_f_48{48000, _48kHz};  /* 48k samples at 48kHz == 1s */
    constexpr framestamp one_sec_f_96{96000, _96kHz};  /* 48k samples at 48kHz == 1s */
    constexpr tickstamp one_sec_t_60{24, _60bpm};   /* 24 ticks == 1beat == 1s at 60bpm */
    constexpr tickstamp one_sec_t_120{48, _120bpm}; /* 48 ticks == 2beat == 1s at 120bpm */

    BIASSERT(_48kHz == 48_kHz);
    BIASSERT(_48kHz == 48000_Hz);

    BIASSERT(_60bpm == 60.0_bpm);
    BIASSERT(_60bpm == 60_bpm);

    BIASSERT(tick_to_frame(one_sec_t_60, _48kHz).value == 48'000);
    BIASSERT(tick_to_frame(one_sec_t_120, _48kHz).value == 48'000);
    BIASSERT(frame_to_tick(one_sec_f_48, _60bpm).value == 24);
    BIASSERT(frame_to_tick(one_sec_f_48, _120bpm).value == 48);

    BIASSERT(remap<framestamp>(one_sec_f_48, _96kHz).value == 96'000);
    BIASSERT(remap<framestamp>(one_sec_f_96, _48kHz).value == 48'000);
    BIASSERT(remap<tickstamp>(one_sec_t_60, _120bpm).value == 48);
    BIASSERT(remap<tickstamp>(one_sec_t_120, _60bpm).value == 24);

    /* Check numeric stability at highest samplerate */

    BIASSERT(remap<framestamp>(framestamp{196'000, 196_kHz}, 196_kHz).value == 196'000);
    BIASSERT(remap<framestamp>(framestamp{196'000, 196_kHz}, _48kHz).value == 48'000);
    BIASSERT(remap<framestamp>(one_sec_f_48, 196_kHz).value == 196'000);

    /* The FINAL test, check stability for at least 72 hours at maximal samplerate */

    BIASSERT(remap<framestamp>(framestamp{50'803'200'000, 196_kHz}, 196_kHz).value == 50'803'200'000);
    BIASSERT(remap<framestamp>(framestamp{24'883'200'000, 96_kHz},  196_kHz).value == 50'803'200'000);
    BIASSERT(remap<framestamp>(framestamp{12'441'600'000, 48_kHz},  196_kHz).value == 50'803'200'000);

    BIASSERT(tick_to_frame(tickstamp{6'220'800,  60_bpm}, 196_kHz).value  == 50'803'200'000);
    BIASSERT(tick_to_frame(tickstamp{24'883'200, 240_bpm}, 196_kHz).value == 50'803'200'000);

    BIASSERT(frame_to_tick(framestamp{50'803'200'000, 196_kHz}, 240_bpm).value == 24'883'200);

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

    /* Test sync utils */

    constexpr syncpoint zero{tickstamp{0, 60_bpm}, framestamp{0, 48_kHz}};
    constexpr syncpoint one{tickstamp{24, 60_bpm}, framestamp{48'000, 48_kHz}};
    constexpr syncinterval in{zero, one};



    return 0;
}
