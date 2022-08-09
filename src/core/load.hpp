#pragma once

#include "types.hpp"

#include <string>

namespace sfx {

    std::string to_string(const event::midi& e);
    std::string to_string(const event::osc& e);
    std::string to_string(const event::internal::any& e);

    inline std::string to_string(const event::any& e)
        { return std::visit([](auto &&e){ return to_string(e); }, e); }

    event::midi midi_from_string(const std::string& s);
    event::osc osc_from_string(const std::string& s);
    event::internal::any internal_from_string(const std::string& s);

    event::any from_string(const std::string& s);
}