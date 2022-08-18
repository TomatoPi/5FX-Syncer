#include "../time.hpp"

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

    BIASSERT(one_sec_f_48);
    BIASSERT(one_sec_f_96);
    BIASSERT(one_sec_t_60);
    BIASSERT(one_sec_t_120);
    BIASSERT(!framestamp{});
    BIASSERT(!tickstamp{});
    BIASSERT(!tickstamp{16});

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

    /* Test arithmetical operators */

    static_assert(tickstamp{0, 60_bpm} + tickstamp{15, 60_bpm} == tickstamp{15, 60_bpm});
    static_assert(tickstamp{30, 60_bpm} + tickstamp{60, 60_bpm} == tickstamp{90, 60_bpm});
    static_assert(tickstamp{30, 60_bpm} + tickstamp{60, 120_bpm} == tickstamp{30, 30_bpm});

    static_assert(tickstamp{60, 60_bpm} - tickstamp{60, 60_bpm} == tickstamp{0, 60_bpm});
    static_assert(tickstamp{60, 60_bpm} - tickstamp{120, 120_bpm} == tickstamp{0, 60_bpm});
    static_assert(tickstamp{0, 60_bpm} - tickstamp{960, 120_bpm} == tickstamp{-240, 30_bpm});

    /* Test syncronisation utilities */

    using sync_t = syncpoint<tick, frame>;
    constexpr sync_t sync0{tickstamp{0, 60_bpm}, framestamp{0, 48_kHz}};
        /**< start bar 0 at frame 0, 60bpm */
    static_assert(sync0(tick{0}) == frame{0});
    static_assert(sync0(tick{1}) == frame{50});
    static_assert(sync0(tick{240}) == frame{12'000});
    static_assert(sync0(tick{960}) == frame{48'000});
        /** frame to tick */
    static_assert(sync0(frame{50}) == tick{1});
    static_assert(sync0(frame{64}) == tick{1});
    static_assert(sync0(frame{12'000}) == tick{240});
    static_assert(sync0(frame{12'049}) == tick{240});
    static_assert(sync0(frame{12'050}) == tick{241});

    constexpr sync_t syncbarm1{tickstamp{ppqn * 4, 60_bpm}, framestamp{0, 48_kHz}};
        /**< start bar 1 at frame 0, 60bpm */
    static_assert(syncbarm1(tick{0}) == frame{-192'000});
    static_assert(syncbarm1(tick{ppqn * 5}) == frame{48'000});
    static_assert(syncbarm1(tick{ppqn * 4}) == frame{0});

    constexpr sync_t syncpat60{tickstamp{0, 60_bpm}, framestamp{48'000, 48_kHz}};
        /**< start bar 0 at sec 1, 60bpm */
    static_assert(syncpat60(tick{0}) == frame{48'000});
    static_assert(syncpat60(tick{1}) == frame{48'050});
    static_assert(syncpat60(tick{ppqn}) == frame{96'000});
        /** frame to tick */
    static_assert(syncpat60(frame{0}) == tick{-960});
    static_assert(syncpat60(frame{64}) == tick{-958});
    static_assert(syncpat60(frame{47'950}) == tick{-1});
    static_assert(syncpat60(frame{47'951}) == tick{0});
    static_assert(syncpat60(frame{47'999}) == tick{0});
    static_assert(syncpat60(frame{48'000}) == tick{0});

    constexpr sync_t syncpat120{tickstamp{0, 120_bpm}, framestamp{48'000, 48_kHz}};
        /**< start bar 0 at sec 1, 120bpm */
    static_assert(syncpat120(tick{0}) == frame{48'000});
    static_assert(syncpat120(tick{1}) == frame{48'025});
    static_assert(syncpat120(tick{ppqn}) == frame{72'000});

    return 0;
}
