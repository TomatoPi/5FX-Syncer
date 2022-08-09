#include "../src/core/load.hpp"

#include <iostream>
#include <cassert>

int main(int argc, char * const argv[])
{
    using namespace sfx;

    /* Serialisation */

    event::any noteon   = event::midi{std::byte{0x80}, std::byte{0x67}, std::byte{0x34}};
    event::any noteoff  = event::midi{std::byte{0x90}, std::byte{0x67}, std::byte{0x0}};
    event::any cc       = event::midi{std::byte{0xb8}, std::byte{0x40}, std::byte{0x7f}};

    assert("M [0x80 0x67 0x34]" == to_string(noteon));
    assert("M [0x90 0x67 0x00]" == to_string(noteoff));
    assert("M [0xb8 0x40 0x7f]" == to_string(cc));

    event::any nsmkill  = event::osc{"/nsm/server/kill", {}};
    event::any nsmkill2 = event::osc{"/nsm/server/kill", {"pouet haha", event::osc::blob({std::byte{0x10}, std::byte{0x20}})}};
    event::any mixgain  = event::osc{"/mixer/set/gain", {17, 0.5f}};

    assert("O /nsm/server/kill" == to_string(nsmkill));
    assert("O /nsm/server/kill \"pouet haha\" [0x10 0x20]" == to_string(nsmkill2));
    assert("O /mixer/set/gain 17 0.500000" == to_string(mixgain));

    event::any playloopback = event::internal::play{"loopback"};
    event::any playsynth    = event::internal::play{"synth"};
    event::any stoploopback = event::internal::stop{"loopback"};

    assert("I play loopback" == to_string(playloopback));
    assert("I play synth"    == to_string(playsynth));
    assert("I stop loopback" == to_string(stoploopback));

    /* De-serialisation */

    assert(noteon == from_string(to_string(noteon)));
    assert(noteoff == from_string(to_string(noteoff)));
    assert(cc == from_string(to_string(cc)));

    from_string(to_string(nsmkill2));

    return 0;
}