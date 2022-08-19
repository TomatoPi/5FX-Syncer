#include "../src/core/time.hpp"

#include <iostream>

int main(int argc, char * const argv[])
{
    using namespace sfx;

    /*
        So we will simulate the player behaviour.
        let compute the time until next tick and look if it happens during this block (spoiler: yes)
        so we compute frametime for this new tick
    */

    constexpr time::bpm bpm{60_bpm};
    constexpr time::samplerate sr{48_kHz};
    constexpr time::frame block_size{64};

    std::size_t block_index = 0;

    /* by default we start from this state */
    
    time::syncer anchor{{0, bpm}, {0, sr}};

    time::tick current_tick = anchor.rtime.repr;
    time::frame current_frame = anchor.atime.repr;

    time::tick next_tick{current_tick +1};
    time::frame next_tick_frame = anchor(next_tick);
    assert(next_tick == 1);
    assert(next_tick_frame == 50);

    time::frame block_end_frame{current_frame + block_size};
    assert(block_end_frame == 64);

    auto _step = [&](bool should_pass = true) {
        /* If next tick is in this block let it happens */
        if (next_tick_frame < block_end_frame)
        {
            current_tick = time::tick{current_tick +1};
            next_tick = time::tick{current_tick +1};
            next_tick_frame = anchor(next_tick);
            if (!should_pass)
            {
                std::cout << current_tick << std::endl;
                assert(false);
            }
        }
        else
        {
            if (should_pass)
            {
                std::cout << current_tick << std::endl;
                assert(false);
            }
        }
    };

    auto _step_block = [&]() {
        current_frame = block_end_frame;
        block_end_frame = time::frame{current_frame + block_size};
        block_index += 1;
    };

    // BLOCK 0 [0, 64[
    assert(current_frame == 0);
    assert(block_end_frame == 64);

    _step(); // Next tick (f50) is in the bloc
    assert(current_tick == 1);
    _step(false); // Next tick (f100) is not in the block
    assert(current_tick == 1);

    _step_block();
    // BLOCK 1 [64, 128[
    assert(current_frame == 64);
    assert(block_end_frame == 128);

    _step(); // Next tick (f100) is in the block
    _step(false); // f150 is not in the block
    assert(current_tick == 2);
    assert(next_tick_frame == 150);

    /*
        Let suppose that real BPM wasn't 60 but 23.4375 (which corresponds to exactly 1 tick per 2 block)
        So we recieve an hard_sync at frame 128 and we must compensate the fact we had launched two bad ticks
    */
//    std::cout << time::to_base<time::tick>(time::framestamp{block_size, sr}) << std::endl;

    _step_block();
    // BLOCK 2 [128, 192[
    assert(current_frame == 128);
    assert(block_end_frame == 192);

    /* First let compute a new syncpoint */
    time::syncer hard_sync{{anchor.rtime + time::tick{1}}, {128, sr}};
    assert(hard_sync.rtime.repr == 1);
    /* Then compute and fill the real bpm */
    time::tick deltat{hard_sync.rtime.repr - anchor.rtime.repr};
    auto deltaf = hard_sync.atime - anchor.atime;
    assert(deltat == 1);
    assert(deltaf.repr == 128);
    hard_sync.rtime.base = time::to_base<time::tick>(deltaf, deltat);
    assert(hard_sync.rtime.base == 23.4375_bpm);
    anchor = hard_sync;

    /* So we now must recompute the next tick time */
    next_tick_frame = anchor(next_tick);
    assert(next_tick == 3);
    assert(next_tick_frame == 384);
    _step(false); // Not reached yet

    /* Nothing will happens until block 5 */
    assert(block_index == 2);
    _step_block(); _step(false); // -> 3
    _step_block(); _step(false);
    _step_block(); _step(false);
    _step_block(); // -> 6
    assert(block_index == 6);
    assert(current_frame == 384);
    assert(block_end_frame == 448);

    _step(); // Now we will go to tick 3
    assert(current_tick == 3);
    assert(next_tick == 4);
    assert(next_tick_frame == 512);
    _step(false); // Next tick is too far

    /* 
        TWIST : BPM has raised to a beautiful 30, therefore an hardsync is received at frame 484 (384 + 100)
    */

    _step_block();
    assert(block_index == 7);
    assert(current_frame == 448);
    assert(block_end_frame == 512);

    hard_sync = {{current_tick + time::tick{1}}, {484, sr}};
    deltat = time::tick{hard_sync.rtime.repr - current_tick};
    deltaf = hard_sync.atime - time::frame{384};
    assert(deltat == 1);
    assert(deltaf.repr == 100);

    hard_sync.rtime.base = time::to_base<time::tick>(deltaf, deltat);
    assert(hard_sync.rtime.base == 30_bpm);
    anchor = hard_sync;

    next_tick_frame = anchor(next_tick);
    assert(next_tick == 4);
    assert(next_tick_frame == 484);
    _step(); // Next tick will be reached sooner
    _step(false); // next tick at 584 (not this block)

    return 0;
}