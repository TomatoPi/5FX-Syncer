#include "../hio.hpp"

#include <iostream>
#include <sstream>
#include <cassert>

int main(int argc, char * const argv[])
{
    using namespace sfx;
    using namespace io::human_readable;

    /* Serialisation */

    std::ostringstream oss;
    writer<std::ostringstream> w{oss};

    event::any noteon   = event::midi{{std::byte{0x80}, std::byte{0x67}, std::byte{0x34}}};
    event::any noteoff  = event::midi{{std::byte{0x90}, std::byte{0x67}, std::byte{0x0}}};
    event::any cc       = event::midi{{std::byte{0xb8}, std::byte{0x40}, std::byte{0x7f}}};

    assert("M [ 0x80 0x67 0x34 ]" == w(noteon).str());  oss = std::ostringstream{};
    assert("M [ 0x90 0x67 0x00 ]" == w(noteoff).str()); oss = std::ostringstream{};
    assert("M [ 0xb8 0x40 0x7f ]" == w(cc).str());      oss = std::ostringstream{};

    event::any nsmkill  = event::osc{"/nsm/server/kill", {}};
    event::any nsmkill2 = event::osc{"/nsm/server/kill", {"pouet haha", event::osc::blob({std::byte{0x10}, std::byte{0x20}})}};
    event::any mixgain  = event::osc{"/mixer/set/gain", {17, 0.5f}};

    assert("O /nsm/server/kill {}" == w(nsmkill).str());                                   oss = std::ostringstream{};
    assert("O /nsm/server/kill { sb \"pouet haha\" [ 0x10 0x20 ] }" == w(nsmkill2).str());  oss = std::ostringstream{};
    assert("O /mixer/set/gain { if 17 0.500000 }" == w(mixgain).str());                     oss = std::ostringstream{};

    event::any playloopback = event::internal::play{"loopback"};
    event::any playsynth    = event::internal::play{"synth"};
    event::any stoploopback = event::internal::stop{"loopback"};
    event::any tick         = event::internal::tick{};
    event::any resume       = event::internal::resume{};

    assert("I play \"loopback\"" == w(playloopback).str()); oss = std::ostringstream{};
    assert("I play \"synth\""    == w(playsynth).str());    oss = std::ostringstream{};
    assert("I stop \"loopback\"" == w(stoploopback).str()); oss = std::ostringstream{};
    assert("I tick" == w(tick).str());      oss = std::ostringstream{};
    assert("I resume" == w(resume).str());  oss = std::ostringstream{};

    /* De-serialisation */

    std::istringstream iss;
    reader<std::istringstream> r{iss};

    iss = std::istringstream{"12"};         assert(12 == static_cast<int32_t>(r));
    iss = std::istringstream{"15.5"};       assert(15.5f == static_cast<float>(r));
    iss = std::istringstream{"\"Tomato\""}; assert("Tomato" == static_cast<std::string>(r));
    iss = std::istringstream{"0x80"};       assert(std::byte{0x80} == static_cast<std::byte>(r));

    iss = std::istringstream{"[ 1 2 3 ]"};  assert(std::vector<int32_t>({1, 2, 3}) == static_cast<std::vector<int32_t>>(r));

    iss = std::istringstream{"M [ 0x80 0x67 0x34 ]"};   assert(noteon == static_cast<event::any>(r));
    iss = std::istringstream{"M [ 0x90 0x67 0x00 ]"};   assert(noteoff == static_cast<event::any>(r));
    iss = std::istringstream{"M [ 0xb8 0x40 0x7f ]"};   assert(cc == static_cast<event::any>(r));

    iss = std::istringstream{"O /nsm/server/kill {}"};                                  assert(nsmkill == static_cast<event::any>(r));
    iss = std::istringstream{"O /nsm/server/kill { sb \"pouet haha\" [ 0x10 0x20 ] }"}; assert(nsmkill2 == static_cast<event::any>(r));
    iss = std::istringstream{"O /mixer/set/gain { if 17 0.500000 }"};                   assert(mixgain == static_cast<event::any>(r));

    iss = std::istringstream{"I play \"loopback\""};    assert(playloopback == static_cast<event::any>(r));
    iss = std::istringstream{"I play \"synth\""};       assert(playsynth == static_cast<event::any>(r));
    iss = std::istringstream{"I stop \"loopback\""};    assert(stoploopback == static_cast<event::any>(r));
    iss = std::istringstream{"I tick"};                 assert(tick == static_cast<event::any>(r));
    iss = std::istringstream{"I resume"};               assert(resume == static_cast<event::any>(r));

    /* De-serialisation robustness test */

    iss = std::istringstream{"   12"};          assert(12 == static_cast<int32_t>(r));
    iss = std::istringstream{"\t0x80"};         assert(std::byte{0x80} == static_cast<std::byte>(r));
    iss = std::istringstream{"\n[ 1 2 \t3 ]"};  assert(std::vector<int32_t>({1, 2, 3}) == static_cast<std::vector<int32_t>>(r));

    iss = std::istringstream{"\t O \t/nsm/server/kill     {\n sb \"pouet haha\" [ 0x10 \t\t0x20 ] }"}; assert(nsmkill2 == static_cast<event::any>(r));
    
    iss = std::istringstream{"I stop \"loopback\" I play \"loopback\" I stop \"loopback\" M [ 0xb8 0x40 0x7f ] O /mixer/set/gain { if 17 0.500000 }"};
    assert(stoploopback == static_cast<event::any>(r));
    assert(playloopback == static_cast<event::any>(r));
    assert(stoploopback == static_cast<event::any>(r));
    assert(cc == static_cast<event::any>(r));
    assert(mixgain == static_cast<event::any>(r));

    /* Spaces are mandatory */

    iss = std::istringstream{"M[0x80 0x67 0x34]"};
    bool thrown{false};
    try { static_cast<event::any>(r); } 
    catch (std::runtime_error&) { thrown = true; }
    assert(thrown);

    /* Seems ok for basic operations */

    return 0;
}