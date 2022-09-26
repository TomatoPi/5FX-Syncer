#pragma once

#include <map>
#include <string>

namespace sfx {
  namespace event {

    template <typename Timepoint, typename Payload>
    struct event {
      using timepoint_type = Timepoint;
      using payload_type = Payload;

      enum flag : int8_t {
        Invalid = 0x00,
        Custom  = 0x40,
        /* Custom events between 0x40 and 0x4F */
        Builtin = 0x80,
        Begin   = 0x81,
        End     = 0x82
      };

      static constexpr event begin()
        { return {timepoint_type::zero(), payload_type()}; }
      static constexpr event end()
        { return {timepoint_type::max(), payload_type()}; }

      flag           flags;
      timepoint_type time;
      payload_type   payload;
    };

    namespace time {
      template <typename Event>
      struct equals {
        constexpr bool operator() (const Event& a, const Event& b) const
          { return a.time == b.time; }
      };
      template <typename Event>
      struct less {
        constexpr bool operator() (const Event& a, const Event& b) const
          { return a.time < b.time; }
      };
    }

    namespace payload {
      template <typename Event>
      struct equals {
        constexpr bool operator() (const Event& a, const Event& b) const
          { return a.payload == b.payload; }
      };
      template <typename Event>
      struct less {
        constexpr bool operator() (const Event& a, const Event& b) const
          { return a.payload < b.payload; }
      };
    }

    template <typename Event>
    class sequence {
    public:
      using event_list_type = std::multimap<Event, time::less<Event>>;



    private:
      std::string     _name;
      event_list_type _events;
    };
  }
}