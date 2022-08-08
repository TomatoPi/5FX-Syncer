#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstddef>
#include <variant>
#include <functional>

namespace sfx {

    /**
     * @brief Event
     * 
     */
    namespace event {

        using midi = std::vector<std::byte>;

        struct osc {
            using blob = std::vector<std::byte>;
            using data = std::variant<int32_t, float, std::string, blob>;

            std::string         path;
            std::vector<data>   arguments;
        };
        /*constexpr*/ inline bool operator== (const osc& a, const osc& b) { return a.path == b.path && a.arguments == b.arguments; }
        /*constexpr*/ inline bool operator!= (const osc& a, const osc& b) { return a.path != b.path || a.arguments != b.arguments; }
        /*constexpr*/ inline bool operator<  (const osc& a, const osc& b) { return a.path <  b.path && a.arguments <  b.arguments; }

        namespace internal {

            struct play { std::string pattern; };
            /*constexpr*/ inline bool operator== (const play& a, const play& b) { return a.pattern == b.pattern; }
            /*constexpr*/ inline bool operator!= (const play& a, const play& b) { return a.pattern != b.pattern; }
            /*constexpr*/ inline bool operator<  (const play& a, const play& b) { return a.pattern <  b.pattern; }

            struct stop { std::string pattern; };
            /*constexpr*/ inline bool operator== (const stop& a, const stop& b) { return a.pattern == b.pattern; }
            /*constexpr*/ inline bool operator!= (const stop& a, const stop& b) { return a.pattern != b.pattern; }
            /*constexpr*/ inline bool operator<  (const stop& a, const stop& b) { return a.pattern <  b.pattern; }

            using any = std::variant<play, stop>;
        }

        using any = std::variant<midi, osc, internal::any>;
    }

    /**
     * @brief Timestamp
     * 
     */
    struct timestamp { float tick; };
    constexpr timestamp operator+ (timestamp a, timestamp b) { return {a.tick + b.tick}; }
    constexpr timestamp operator- (timestamp a, timestamp b) { return {a.tick - b.tick}; }
    constexpr timestamp operator* (timestamp a, int x) { return {a.tick * x}; }
    constexpr timestamp operator/ (timestamp a, int x) { return {a.tick / x}; }
    constexpr bool operator== (timestamp a, timestamp b) { return a.tick == b.tick; }
    constexpr bool operator<  (timestamp a, timestamp b) { return a.tick <  b.tick; }

    /**
     * @brief Channel
     * 
     */
    struct channel { std::string name; };

    /**
     * @brief Pattern
     * 
     */
    struct pattern {
        std::string                             name;
        std::multimap<timestamp, event::any>    events;
    };

    /**
     * @brief Binding
     * 
     */
    struct binding {
        using matcher = std::function<bool(const event::any&)>;
        using translator = std::function<std::vector<event::internal::any>(const event::any&)>;

        matcher     m;
        translator  t;
    };
}