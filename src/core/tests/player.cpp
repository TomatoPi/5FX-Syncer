#include "../player.hpp"

#include <list>

int main(int argc, char * const argv[])
{
  using timerepr = uint64_t;
  using duration = sfx::time::duration<timerepr>;
  using timepoint = sfx::time::timepoint<timerepr>;

  using event = std::pair<int, duration>;
  using event_list = std::list<event>;

  using player = sfx::player<event_list&, timerepr>;

  event_list events = {
      {0, duration(0, 60_bpm)}, 
      {1, duration(1, 60_bpm)}, 
      {2, duration(2, 60_bpm)}, 
      {3, duration(3, 60_bpm)}, 
    };
  player p(timepoint(0), events);

  return 0;
}