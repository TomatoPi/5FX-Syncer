#include "../src/core/hio.hpp"

#include <iostream>
#include <sstream>
#include <cassert>

int main(int argc, char * const argv[])
{
    using namespace sfx;
    using namespace io::human_readable;

    /* Serialisation */

    event::any noteon   = event::midi{{std::byte{0x80}, std::byte{0x67}, std::byte{0x34}}};
    event::any noteoff  = event::midi{{std::byte{0x90}, std::byte{0x67}, std::byte{0x0}}};
    event::any cc       = event::midi{{std::byte{0xb8}, std::byte{0x40}, std::byte{0x7f}}};

    std::ostringstream oss;
    writer<std::ostringstream> w{oss};
    assert("M [ 0x80 0x67 0x34 ]" == w(noteon).str());  oss = std::ostringstream{};
    assert("M [ 0x90 0x67 0x00 ]" == w(noteoff).str()); oss = std::ostringstream{};
    assert("M [ 0xb8 0x40 0x7f ]" == w(cc).str());      oss = std::ostringstream{};

    event::any nsmkill  = event::osc{"/nsm/server/kill", {}};
    event::any nsmkill2 = event::osc{"/nsm/server/kill", {"pouet haha", event::osc::blob({std::byte{0x10}, std::byte{0x20}})}};
    event::any mixgain  = event::osc{"/mixer/set/gain", {17, 0.5f}};

    assert("O /nsm/server/kill" == w(nsmkill).str());                                   oss = std::ostringstream{};
    assert("O /nsm/server/kill sb \"pouet haha\" [ 0x10 0x20 ]" == w(nsmkill2).str());  oss = std::ostringstream{};
    assert("O /mixer/set/gain if 17 0.500000" == w(mixgain).str());                     oss = std::ostringstream{};

    event::any playloopback = event::internal::play{"loopback"};
    event::any playsynth    = event::internal::play{"synth"};
    event::any stoploopback = event::internal::stop{"loopback"};

    assert("I play \"loopback\"" == w(playloopback).str()); oss = std::ostringstream{};
    assert("I play \"synth\""    == w(playsynth).str());    oss = std::ostringstream{};
    assert("I stop \"loopback\"" == w(stoploopback).str()); oss = std::ostringstream{};

    // /* De-serialisation */

    // assert(noteon == from_string(to_string(noteon)));
    // assert(noteoff == from_string(to_string(noteoff)));
    // assert(cc == from_string(to_string(cc)));

    // from_string(to_string(nsmkill2));

    return 0;
}