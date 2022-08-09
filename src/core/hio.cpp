#include "hio.hpp"

#include <regex>
#include <sstream>
#include <iomanip>
#include <iostream>

// namespace details {
//     template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//     template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

//     struct writer
//     {
//         std::ostringstream operator() (int32_t i) const
//             { return std::ostringstream() << std::dec << i; }

//         std::ostringstream operator() (float f) const
//             { return std::ostringstream() << std::showpoint << std::setw(8) << f; }

//         std::ostringstream operator() (const std::string& s) const
//             { return std::ostringstream() << '"' << s << '"'; }

//         std::ostringstream operator() (const std::vector<std::byte>& bs) const
//         {
//             std::ostringstream os;
//             os << '[';
//             for (auto x : bs) os << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(x) << ' ';
//             os.seekp(-1, std::ios_base::end);
//             os << ']';
//             return os;
//         }
//     };

//     std::vector<std::byte> read_array(std::istringstream& is)
//     {
//         std::vector<std::byte> v;

//         char c;
//         is >> c;
//         if (c != '[')
//             throw std::runtime_error("Invalid array start character");

//         while (!is.fail() && is.peek() != ']')
//         {
//             int x;
//             is >> std::hex >> x;
//             v.push_back(std::byte(x));
//         }
//         if (is.fail())
//             throw std::runtime_error("Failed parse array : '" + is.str() + "'");

//         return v;
//     }

//     std::string read_str(std::istringstream& is)
//     {
//         return "";
//     }
// }

// namespace sfx {
    
//     std::string to_string(const event::midi& e)
//     {
//         std::ostringstream ss;
//         ss << "M " << details::writer{}(e).str();
//         return ss.str();
//     }
//     std::string to_string(const event::osc& e)
//     {
//         std::ostringstream ss;
//         ss << "O " << e.path;
//         for (const auto& x : e.arguments)
//             ss << ' ' << std::visit(details::writer{}, x).str();
//         return ss.str();
//     }
//     std::string to_string(const event::internal::any& e)
//     {
//         std::ostringstream ss;
//         ss << "I ";
//         ss << std::visit(details::overloaded{
//             [](const event::internal::play& p) { return "play " + p.pattern; },
//             [](const event::internal::stop& p) { return "stop " + p.pattern; }
//         }, e);
//         return ss.str();
//     }


//     event::midi midi_from_string(const std::string& s)
//     {
//         std::istringstream is{s};
//         return details::read_array(is);
//     }
//     event::osc osc_from_string(const std::string& s)
//     {
//         event::osc o;
//         std::istringstream is{s};
//         is >> o.path;
//         for (std::string token; std::getline(is, token, ' '); )
//         {
//             std::cout << "'" << token << "'" << std::endl;
//             if (token.size() == 0) continue;
//             if (token[0] == '[')
//                 o.arguments.emplace_back(details::read_array(is));
//             else if (token[0] == '"')
//                 o.arguments.emplace_back(details::read_str(is));
//         }
//         return o;
//     }
//     event::internal::any internal_from_string(const std::string& s)
//     {

//     }

//     event::any from_string(const std::string& s)
//     {
//         char c;
//         std::istringstream is{s};
//         is >> c;

//         switch (c)
//         {
//             case 'M' : return midi_from_string(s.substr(2, s.size()-2));
//             case 'O' : return osc_from_string(s.substr(2, s.size()-2));
//             case 'I' : return internal_from_string(s.substr(2, s.size()-2));
//             default : throw std::runtime_error("Invalid Event : '" + s + "'");
//         }
//     }
// }