#pragma once
#include "time.hpp"

#include <type_traits>

namespace sfx {

  template <typename EventList, typename TimeRepr>
  class player {
  public:
    using time_repr = TimeRepr;
    using duration = time::duration<time_repr>;
    using timepoint = time::timepoint<time_repr>;

    using event_list = EventList;
    using event_citr = typename std::decay_t<event_list>::const_iterator;

    player(
      timepoint t = timepoint(),
      event_list l = event_list())
      : _anchor(t), _events(l)
      {}

    constexpr timepoint anchor() const
      { return _anchor; }
    constexpr duration rel_position() const
      { return _position; }
    constexpr timepoint abs_position() const
      { return _anchor + _position; }
    
    std::pair<event_citr, event_citr> events(
      timepoint begin,
      timepoint end) const
    {
      return {_events.lower_bound(_position), _events.upper_bound(end - _anchor)};
    }

  private:
    timepoint   _anchor;
    duration    _position;
    event_list  _events;
  };
}