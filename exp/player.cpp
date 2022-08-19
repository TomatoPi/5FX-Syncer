#include "../src/core/time.hpp"

#include <iostream>

int main(int argc, char * const argv[])
{
    using namespace sfx;
    
    time::bpm bpm{60_bpm};
    time::samplerate sr{48_kHz};
    time::frame blocksize{64};

    /* by default we start from this state */
    time::syncer begin{{0, bpm}, {0, sr}};
    
    /* assume we recieve a midi beat clock at frame 50, let's compute the real bpm */
    time::framestamp mbc{50, sr};
    auto deltaf = mbc - begin.atime;

    /*
        using factor = std::ratio<
            From::timebase::ratio::den * To::timebase::ratio::num,
            From::timebase::ratio::num * To::timebase::ratio::den>;
        auto num = tr.v * t.repr.v * factor::num;
        auto den = t.base.v * factor::den;
        typename To::type value = static_cast<typename To::type>(num / den);

        Here value = 1 tick, so num == den
        with t == deltaf, tr == our new bpm :

            nbpm * deltaf.repr * num == deltaf.base * den
            <=> nbpm = (deltaf.base * den) / (deltaf.repr * num)
    */

    assert(time::to_base(deltaf, time::tick{1}) == 60_bpm);
    assert(time::to_base(deltaf, time::tick{2}) == 120_bpm);
    assert(time::to_base(time::framestamp{48'000, 48_kHz}, time::tick{960}) == 60_bpm);

    return 0;
}