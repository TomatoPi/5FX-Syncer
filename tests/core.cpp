#include "../src/core/types.hpp"

#include <iostream>
#include <cassert>

int main(int argc, char * const argv[])
{
    using namespace sfx;

    event::any noteon   = event::midi{{std::byte{0x80}, std::byte{0x67}, std::byte{0x34}}};
    event::any noteoff  = event::midi{{std::byte{0x90}, std::byte{0x67}, std::byte{0x0}}};
    event::any cc       = event::midi{{std::byte{0xb8}, std::byte{0x40}, std::byte{0x7f}}};

    assert(noteon != noteoff);
    assert(cc != noteoff);

    event::any nsmkill  = event::osc{"/nsm/server/kill", {}};
    event::any nsmkill2 = event::osc{"/nsm/server/kill", {}};
    event::any mixgain  = event::osc{"/mixer/set/gain", {17, 0.5f}};

    assert(nsmkill == nsmkill2);
    assert(nsmkill != mixgain);
    assert(mixgain == mixgain);
    assert(cc != nsmkill);

    event::any playloopback     = event::internal::play{"loopback"};
    event::any playloopback2    = event::internal::play{"loopback"};
    event::any stoploopback     = event::internal::stop{"loopback"};

    assert(playloopback == playloopback2);
    assert(playloopback != stoploopback);
    assert(nsmkill != stoploopback);

    timestamp a{8}, b{8}, c{16}, x{-1};

    assert(a == b);
    assert(a <  c);
    assert(x <  b);

    assert(a + b == c);
    assert(a - b == timestamp{});
    assert(a * 2 == c);
    assert(c / 2 == b);

    return 0;
}