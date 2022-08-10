#pragma once

#include <map>
#include <limits>
#include <string>
#include <vector>
#include <cstddef> // std::byte
#include <variant>
#include <functional>

namespace sfx {

    /**
     * @brief Event
     * 
     */
    namespace event {

        struct midi {
            using data_type = std::vector<std::byte>;
            data_type datas;
        };
        /*constexpr*/ inline bool operator== (const midi& a, const midi& b) { return a.datas == b.datas; }
        /*constexpr*/ inline bool operator!= (const midi& a, const midi& b) { return a.datas != b.datas; }
        /*constexpr*/ inline bool operator<  (const midi& a, const midi& b) { return a.datas <  b.datas; }


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

            struct tick {};
            /*constexpr*/ inline bool operator== (const tick&, const tick&) { return true; }
            /*constexpr*/ inline bool operator!= (const tick&, const tick&) { return false; }
            /*constexpr*/ inline bool operator<  (const tick&, const tick&) { return false; }

            struct play { std::string pattern; };
            /*constexpr*/ inline bool operator== (const play& a, const play& b) { return a.pattern == b.pattern; }
            /*constexpr*/ inline bool operator!= (const play& a, const play& b) { return a.pattern != b.pattern; }
            /*constexpr*/ inline bool operator<  (const play& a, const play& b) { return a.pattern <  b.pattern; }

            struct resume {};
            /*constexpr*/ inline bool operator== (const resume&, const resume&) { return true; }
            /*constexpr*/ inline bool operator!= (const resume&, const resume&) { return false; }
            /*constexpr*/ inline bool operator<  (const resume&, const resume&) { return false; }

            struct stop { std::string pattern; };
            /*constexpr*/ inline bool operator== (const stop& a, const stop& b) { return a.pattern == b.pattern; }
            /*constexpr*/ inline bool operator!= (const stop& a, const stop& b) { return a.pattern != b.pattern; }
            /*constexpr*/ inline bool operator<  (const stop& a, const stop& b) { return a.pattern <  b.pattern; }

            struct end {};
            /*constexpr*/ inline bool operator== (const end&, const end&) { return true; }
            /*constexpr*/ inline bool operator!= (const end&, const end&) { return false; }
            /*constexpr*/ inline bool operator<  (const end&, const end&) { return false; }

            using any = std::variant<tick, play, resume, stop, end>;
        }

        using any = std::variant<midi, osc, internal::any>;
    }

    /**
     * @brief Timestamp
     * 
     */
    struct timestamp
    {
        float tick;
        static constexpr timestamp end() { return timestamp{std::numeric_limits<float>::max()}; }
    };
    constexpr timestamp operator+ (timestamp a, timestamp b) { return {a.tick + b.tick}; }
    constexpr timestamp operator- (timestamp a, timestamp b) { return {a.tick - b.tick}; }
    constexpr timestamp operator* (timestamp a, int x) { return {a.tick * x}; }
    constexpr timestamp operator/ (timestamp a, int x) { return {a.tick / x}; }
    constexpr timestamp& operator++ (timestamp& a) { a.tick += 1; return a; }
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
        using list = std::multimap<timestamp, event::any>;
        using iterator = list::iterator;

        std::string name;
        list        events;

        pattern(std::string name = std::string{}) : name{name}, events{{timestamp::end(), event::internal::end{}}} {}
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

/* STD Functors overload */

template <> struct std::hash<sfx::channel>
    { /*constexpr*/ std::size_t operator() (const sfx::channel& c) const { return std::hash<std::string>{}(c.name); } };
template <> struct std::hash<sfx::pattern>
    { /*constexpr*/ std::size_t operator() (const sfx::pattern& p) const { return std::hash<std::string>{}(p.name); } };