#pragma once

#include "types.hpp"

#include <string>
#include <iostream>
#include <iomanip>

namespace sfx {
    namespace io {

        /**
         * @brief Object containing all methods to serialise and deserialise core objects in human readable format
         */
        namespace human_readable {

            namespace details {
                template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
                template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
            }

            template <typename S> struct writer
            {
                /* Members */
                using stream_type = S;
                S& os;

                /* Primitives writers */

                S& operator() (std::byte b)
                    { os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b); return os; }
                S& operator() (int32_t i)
                    { os << std::dec << i; return os; }
                S& operator() (float f)
                    { os << std::showpoint << std::setw(8) << f; return os; }
                S& operator() (const std::string& s)
                    { os << std::quoted(s, '"', '\\'); return os; }
                
                /* Array helper */

                template <typename T> S& operator() (const std::vector<T>& v)
                {
                    os << "[ ";
                    for (const auto& x : v)
                        (*this)(x) << ' ';
                    os << ']';
                    return os;
                }

                /* Events writers */

                S& operator() (const event::any& e)
                    { return std::visit(*this, e); }

                S& operator() (const event::midi& e)
                {
                    os << "M "; (*this)(e.datas);
                    return os;
                }
                S& operator() (const event::osc& e)
                {
                    os << "O " << e.path;
                    if (e.arguments.size() == 0)
                        return os;

                    os << ' ';
                    for (const auto& arg : e.arguments)
                        os << std::visit(details::overloaded{
                            [](int32_t) { return 'i'; },
                            [](float)   { return 'f'; },
                            [](const std::string&)      { return 's'; },
                            [](const event::osc::blob&) { return 'b'; }
                        }, arg);
                    for (const auto& arg : e.arguments)
                        { os << ' '; std::visit(*this, arg); }
                        
                    return os;
                }
                S& operator() (const event::internal::any& e)
                    { os << "I "; return std::visit(*this, e); }

                /* Internal Events */

                S& operator() (const event::internal::play& e)
                    { os << "play "; return (*this)(e.pattern); }
                S& operator() (const event::internal::stop& e)
                    { os << "stop "; return (*this)(e.pattern); }
            };

            struct reader {

            };
        };
    }
}