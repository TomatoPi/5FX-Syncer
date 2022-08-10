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
                stream_type& os;

                /* Primitives writers */

                stream_type& operator() (std::byte b)
                    { os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b); return os; }
                stream_type& operator() (int32_t i)
                    { os << std::dec << i; return os; }
                stream_type& operator() (float f)
                    { os << std::showpoint << std::setw(8) << f; return os; }
                stream_type& operator() (const std::string& s)
                    { os << std::quoted(s, '"', '\\'); return os; }
                
                /* Array helper */

                template <typename T> stream_type& operator() (const std::vector<T>& v)
                {
                    os << "[ ";
                    for (const auto& x : v)
                        (*this)(x) << ' ';
                    os << ']';
                    return os;
                }

                /* Events writers */

                stream_type& operator() (const event::any& e)
                    { return std::visit(*this, e); }

                stream_type& operator() (const event::midi& e)
                {
                    os << "M "; (*this)(e.datas);
                    return os;
                }
                stream_type& operator() (const event::osc& e)
                {
                    os << "O " << e.path;
                    if (e.arguments.size() == 0)
                        { os << " {}"; return os; }

                    os << " { ";
                    for (const auto& arg : e.arguments)
                        os << std::visit(details::overloaded{
                            [](int32_t) { return 'i'; },
                            [](float)   { return 'f'; },
                            [](const std::string&)      { return 's'; },
                            [](const event::osc::blob&) { return 'b'; }
                        }, arg);
                    for (const auto& arg : e.arguments)
                        { os << ' '; std::visit(*this, arg); }
                    
                    os << " }";
                    return os;
                }
                stream_type& operator() (const event::internal::any& e)
                    { os << "I "; return std::visit(*this, e); }

                /* Internal Events */

                stream_type& operator() (const event::internal::play& e)
                    { os << "play "; return (*this)(e.pattern); }
                stream_type& operator() (const event::internal::stop& e)
                    { os << "stop "; return (*this)(e.pattern); }
            };

            template <typename S> struct reader
            {
                /* Members */

                using stream_type = S;
                stream_type& is;

                /* Primitives readers */

                explicit operator std::byte ()
                    { int x; is >> std::hex >> x; return static_cast<std::byte>(x); }
                explicit operator int32_t ()
                    { int32_t i; is >> std::dec >> i; return i; }
                explicit operator float ()
                    { float f; is >> std::showpoint >> f; return f; }
                explicit operator std::string ()
                    { std::string s; is >> std::quoted(s, '"', '\\'); return s; }
                
                /* Array helper */
                
                template <typename T> explicit operator std::vector<T> ()
                {
                    char c;
                    is >> std::ws >> c;
                    if (c != '[') throw std::runtime_error("Invalid vector start character");

                    std::vector<T> v;
                    for (std::string token; is && std::getline(is >> std::ws, token, ' '); )
                    {
                        if (token == "]") break;
                        std::istringstream iss{token};
                        v.emplace_back(static_cast<T>(reader{iss}));
                    }
                    if (!is) throw std::runtime_error("Error while parsing array");

                    return v;
                }

                /* Events readers */

                explicit operator event::any ()
                {
                    char type_c;
                    is >> std::ws >> type_c;
                    switch (type_c)
                    {
                        case 'M' : return static_cast<event::midi>(*this);
                        case 'O' : return static_cast<event::osc>(*this);
                        case 'I' : return static_cast<event::internal::any>(*this);
                        default : throw std::runtime_error("Invalid message type identifier");
                    }
                }

                explicit operator event::midi ()
                    { return event::midi{static_cast<event::midi::data_type>(*this)}; }
                explicit operator event::osc ()
                {
                    event::osc e;
                    is >> std::ws >> e.path;

                    std::string args_t;
                    is >> std::ws >> args_t;
                    if (args_t == "{}") return e;

                    is >> std::ws >> args_t;
                    for (size_t i=0 ; is && i<args_t.size() ; ++i)
                    {
                        switch (args_t[i])
                        {
                            case 'i' : e.arguments.emplace_back(static_cast<int32_t>(*this)); break;
                            case 'f' : e.arguments.emplace_back(static_cast<float>(*this)); break;
                            case 's' : e.arguments.emplace_back(static_cast<std::string>(*this)); break;
                            case 'b' : e.arguments.emplace_back(static_cast<event::osc::blob>(*this)); break;
                            default : throw std::runtime_error("Invalid osc argument type identifier");
                        }
                    }
                    if (!is) throw std::runtime_error("Error while parsing osc arguments");

                    char c;
                    is >> std::ws >> c;
                    if (c != '}') throw std::runtime_error("Invalid OSC stop character");

                    return e;
                }
                explicit operator event::internal::any ()
                {
                    std::string type;
                    is >> std::ws >> type;

                    if (type == "play") return static_cast<event::internal::play>(*this);
                    if (type == "stop") return static_cast<event::internal::stop>(*this);
                    throw std::runtime_error("Invalid internal event type");
                }

                /* Internal Events */

                explicit operator event::internal::play ()
                    { return event::internal::play{static_cast<std::string>(*this)}; }
                explicit operator event::internal::stop ()
                    { return event::internal::stop{static_cast<std::string>(*this)}; }
            };
        };
    }
}