#pragma once

#include "types.hpp"

#include <list>
#include <string>
#include <stdexcept>
#include <functional>
#include <map>
#include <unordered_map>

namespace sfx {
    
    class engine {
    private:

        using patterns_reg = std::list<pattern>;
        using reg_entry = patterns_reg::iterator;

        using patterns_table = std::unordered_map<std::string, reg_entry>;

        struct player {
            timestamp t;
            reg_entry p;
            /**
             * @brief return the range of events appening between player's t and given t
             * 
             * @param t 
             * @return std::multimap<timestamp, event::any> 
             */
            std::pair<pattern::iterator, pattern::iterator> operator() (timestamp end) const
                { return std::make_pair(p->events.lower_bound(t), p->events.upper_bound(end)); }
        };
        using active_patterns_list = std::unordered_map<std::string, player>;

        patterns_reg            reg;
        patterns_table          table;
        active_patterns_list    active_patterns;

        timestamp current_time;

    public:

        void add_pattern(const pattern& p)
        {
            if (table.find(p.name) != table.end())
                throw std::runtime_error("Duplicated key : " + p.name);
            reg.emplace_front(p);
            table.emplace_hint(table.end(), p.name, reg.begin());
        }
        void remove_pattern(const std::string& name)
        {
            auto itr = table.find(name);
            if (itr == table.end())
                return;
            
            active_patterns.erase(name); // no need to verify if it's there
            reg.erase(itr->second);
            table.erase(itr);
        }

        std::list<event::any> operator() (const event::internal::any& e)
            { return std::visit(*this, e); }

        std::list<event::any> operator() (const event::internal::tick& e)
        {
            ++current_time;
            for (auto& [_, pl] : active_patterns)
            {
                ++pl.t;
            }
        }
        std::list<event::any> operator() (const event::internal::play& e)
        {
            auto itr = table.find(e.pattern);
            if (itr == table.end()) throw std::runtime_error("Invalid pattern key : " + e.pattern);
            // active_patterns.insert_or_assign(itr->first, timestamp{}, itr->second);
        }
        std::list<event::any> operator() (const event::internal::resume& e)
        {

        }
        std::list<event::any> operator() (const event::internal::stop& e)
        {

        }
    };
}