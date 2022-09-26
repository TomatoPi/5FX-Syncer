#include "../time.hpp"

#include <iostream>
#include <cassert>

#define BIASSERT(expr) static_assert(expr); assert((expr));

int main(int argc, char * const argv[])
{
    using namespace sfx::time;

    /* runtime rational arithmetic */

    BIASSERT(ratio(5, 7).num() == 5);
    BIASSERT(ratio(5, 7).den() == 7);
    BIASSERT(ratio(3, 15).num() == 1);
    BIASSERT(ratio(3, 15).den() == 5);

    BIASSERT(ratio(1, 2).inverse() == ratio(2));

    BIASSERT(ratio() == ratio(1, 1));
    BIASSERT(ratio() == ratio(2, 2));
    BIASSERT((ratio(1, 2) * 4) == ratio(8, 4));
    BIASSERT((ratio(1, 2) * 4) == ratio(2));

    BIASSERT(ratio(1, 2) * ratio(1, 2) == ratio(1, 4));
    BIASSERT(ratio(1, 2) * ratio(4, 3) == ratio(2, 3));
    BIASSERT(ratio(5, 7) * ratio(7, 5) == ratio());

    BIASSERT(ratio(1, 5) < ratio(1, 3));
    BIASSERT(ratio(1, 2) < ratio(3, 2));

    /* string literals */

    BIASSERT(timebase{ratio(48'000, 1)} == 48_kHz);
    BIASSERT(timebase{ratio(48'000, 1)} == 48'000_Hz);
    BIASSERT(timebase{ratio(1, 1)}      == 60_bpm);

    /* basic conversions */
    
    constexpr duration one_sec_f_48{48000, 48_kHz};  /* 48k samples at 48kHz == 1s */
    constexpr duration one_sec_f_96{96000, 96_kHz};  /* 48k samples at 48kHz == 1s */
    constexpr duration one_sec_t_60 {1, 60_bpm};   /* 24 ticks == 1beat == 1s at 60bpm */
    constexpr duration one_sec_t_120{2, 120_bpm}; /* 48 ticks == 2beat == 1s at 120bpm */

    BIASSERT(one_sec_f_48.is_valid());
    BIASSERT(one_sec_f_96.is_valid());
    BIASSERT(one_sec_t_60.is_valid());
    BIASSERT(one_sec_t_120.is_valid());

    BIASSERT(one_sec_t_60.value(48_kHz) == 48'000);
    BIASSERT(one_sec_t_120.value(48_kHz) == 48'000);
    BIASSERT(one_sec_f_48.value(60_bpm) == 1);
    BIASSERT(one_sec_f_48.value(120_bpm) == 2);

    /* basic remaping */

    BIASSERT(one_sec_f_48.value(96_kHz) == 96'000);
    BIASSERT(one_sec_f_96.value(48_kHz) == 48'000);
    BIASSERT(one_sec_t_60.value(120_bpm) == 2);
    BIASSERT(one_sec_t_120.value(60_bpm) == 1);

    /* advanced remaping */

    constexpr auto x = duration(48'000, 48_kHz);

    BIASSERT(duration(48'000, 48_kHz).as_base() == 60_bpm);
    BIASSERT(duration(96'000, 96_kHz).as_base()   == 60_bpm);
    BIASSERT(duration(192'000, 192_kHz).as_base() == 60_bpm);
        /** Twice more ticks in the same duration <=> double bpm */
    BIASSERT(duration(384'000, 192_kHz).as_base() == 120_bpm);
        /** Twice less ticks in the same duration <=> half bpm */
    BIASSERT(duration(96'000, 192_kHz).as_base() == 30_bpm);

    /* Check numeric stability at highest samplerate */

    BIASSERT(duration(192'000, 192_kHz).value(192_kHz) == 192'000);
    BIASSERT(duration(192'000, 192_kHz).value(48_kHz) == 48'000);
    BIASSERT(duration(48'000, 48_kHz).value(192_kHz) == 192'000);

    /* Check stability for at least 72 hours at maximal samplerate */

    constexpr uint64_t secsp72h          = 259'200; //*< 3600 * 72 */
    constexpr uint64_t framesp72h_192kHz = 49'766'400'000;
    constexpr uint64_t framesp72h_96kHz  = 24'883'200'000;
    constexpr uint64_t framesp72h_48kHz  = 12'441'600'000;

    BIASSERT(duration(secsp72h * 192'000, 192_kHz).value(192_kHz) 
        == framesp72h_192kHz);
    BIASSERT(duration(framesp72h_96kHz, 96_kHz).value(192_kHz)
        == framesp72h_192kHz);
    BIASSERT(duration(framesp72h_48kHz, 48_kHz).value(192_kHz)
        == framesp72h_192kHz);

    constexpr uint64_t ppqn = 960;

    BIASSERT(duration(ppqn * secsp72h, ppqn * 60_bpm).value(192_kHz)
        == framesp72h_192kHz);
    BIASSERT(duration(4 * ppqn * secsp72h, 4 * ppqn * 60_bpm).value(192_kHz)
        == framesp72h_192kHz);
    BIASSERT(duration(framesp72h_192kHz, 192_kHz).value(4 * ppqn * 60_bpm)
        == 4 * ppqn * secsp72h);

    /* Test relational operators */

    BIASSERT(duration(0, 48_kHz) == duration(0, 192_kHz));
    BIASSERT(duration(192'000, 192_kHz) == duration(48'000, 48_kHz));
    BIASSERT(duration(44'100, 44'100_Hz) == duration(48'000, 48_kHz));
    BIASSERT(duration(96'000, 192_kHz) == duration(24'000, 48_kHz));

    BIASSERT(duration(24'000, 44'100_Hz) < duration(48'000, 48_kHz));
    BIASSERT(duration(96'000, 192'000_Hz) < duration(40'000, 48_kHz));

    BIASSERT(!(duration(95'999, 96_kHz) == duration(47'999, 48_kHz)));
    BIASSERT(!(duration(95'999, 96_kHz) < duration(47'999, 48_kHz)));

    BIASSERT(duration(0, 120_bpm)  == duration(0, 240_bpm));
    BIASSERT(duration(60, 120_bpm) == duration(120, 240_bpm));
    BIASSERT(duration(90, 120_bpm) == duration(180, 240_bpm));
    BIASSERT(duration(90, 120_bpm) < duration(200, 240_bpm));
    BIASSERT(duration(66, 240_bpm) < duration(55, 60_bpm));

    /* Test arithmetical operators */

    BIASSERT(duration(0, 60_bpm) + duration(15, 60_bpm) == duration(15, 60_bpm));
    BIASSERT(duration(30, 60_bpm) + duration(60, 60_bpm) == duration(90, 60_bpm));
    BIASSERT(duration(30, 60_bpm) + duration(60, 120_bpm) == duration(30, 30_bpm));

    BIASSERT(duration(60, 60_bpm) - duration(60, 60_bpm) == duration(0, 60_bpm));
    BIASSERT(duration(60, 60_bpm) - duration(120, 120_bpm) == duration(0, 60_bpm));
    BIASSERT(duration(0, 60_bpm) - duration(960, 120_bpm) == duration(-240, 30_bpm));

    /* Test timepoint */

    BIASSERT(timepoint(0, 60_bpm) == timepoint(0, 48_kHz));

    constexpr auto _960ppqn_60bpm = 960 * 60_bpm;

    BIASSERT(timepoint(0, _960ppqn_60bpm) == timepoint(0, 48_kHz));
    BIASSERT(timepoint(1, _960ppqn_60bpm) == timepoint(50, 48_kHz));
    BIASSERT(timepoint(240, _960ppqn_60bpm) == timepoint(12'000, 48_kHz));
    BIASSERT(timepoint(960, _960ppqn_60bpm) == timepoint(48'000, 48_kHz));

    // /* Test syncronisation utilities */

    // using sync_t = syncpoint<tick, frame>;
    // constexpr sync_t sync0{duration(0, 60_bpm}, duration(0, 48_kHz}};
    //     /**< start bar 0 at frame 0, 60bpm */
    // static_assert(sync0(tick{0}) == frame{0});
    // static_assert(sync0(tick{1}) == frame{50});
    // static_assert(sync0(tick{240}) == frame{12'000});
    // static_assert(sync0(tick{960}) == frame{48'000});
    //     /** frame to tick */
    // static_assert(sync0(frame{50}) == tick{1});
    // static_assert(sync0(frame{64}) == tick{1});
    // static_assert(sync0(frame{12'000}) == tick{240});
    // static_assert(sync0(frame{12'049}) == tick{240});
    // static_assert(sync0(frame{12'050}) == tick{241});

    // constexpr sync_t syncbarm1{duration(ppqn * 4, 60_bpm}, duration(0, 48_kHz}};
    //     /**< start bar 1 at frame 0, 60bpm */
    // static_assert(syncbarm1(tick{0}) == frame{-192'000});
    // static_assert(syncbarm1(tick{ppqn * 5}) == frame{48'000});
    // static_assert(syncbarm1(tick{ppqn * 4}) == frame{0});

    // constexpr sync_t syncpat60{duration(0, 60_bpm}, duration(48'000, 48_kHz}};
    //     /**< start bar 0 at sec 1, 60bpm */
    // static_assert(syncpat60(tick{0}) == frame{48'000});
    // static_assert(syncpat60(tick{1}) == frame{48'050});
    // static_assert(syncpat60(tick{ppqn}) == frame{96'000});
    //     /** frame to tick */
    // static_assert(syncpat60(frame{0}) == tick{-960});
    // static_assert(syncpat60(frame{64}) == tick{-958});
    // static_assert(syncpat60(frame{47'950}) == tick{-1});
    // static_assert(syncpat60(frame{47'951}) == tick{0});
    // static_assert(syncpat60(frame{47'999}) == tick{0});
    // static_assert(syncpat60(frame{48'000}) == tick{0});

    // constexpr sync_t syncpat120{duration(0, 120_bpm}, duration(48'000, 48_kHz}};
    //     /**< start bar 0 at sec 1, 120bpm */
    // static_assert(syncpat120(tick{0}) == frame{48'000});
    // static_assert(syncpat120(tick{1}) == frame{48'025});
    // static_assert(syncpat120(tick{ppqn}) == frame{72'000});

    return 0;
}
