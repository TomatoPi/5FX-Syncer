#pragma once

namespace sfx {

    struct timestamp { float tick; };
    constexpr timestamp operator+ (timestamp a, timestamp b) { return {a.tick + b.tick}; }
    constexpr timestamp operator- (timestamp a, timestamp b) { return {a.tick - b.tick}; }
    constexpr timestamp operator* (timestamp a, int x) { return {a.tick * x}; }
    constexpr timestamp operator/ (timestamp a, int x) { return {a.tick / x}; }
    constexpr bool operator== (timestamp a, timestamp b) { return a.tick == b.tick; }
    constexpr bool operator<  (timestamp a, timestamp b) { return a.tick <  b.tick; }
}