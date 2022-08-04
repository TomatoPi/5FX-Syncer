# 5FX-Syncer

A JACK Musical synchronisation tool, usable as a tracker, sequencer, arpeggiator, etc.

## Principles

This program is based on the following concepts comming from traditionnal music sequencer / trackers :

- An **Event** can represent either a MIDI event, and OSC event, an internal event like 'stop pattern n°5', etc. They are primary objects manipulated by the syncer.
- A **Timestamp** is a score related time position given as number of bars, beats and MIDI ticks since the begining of a pattern (MIDI ticks are send at a rate of 24 ticks per quarter note). Timestamps can also have a floating value for higher precision.
- A **Channel** is a bus where events are launched. They represents the outputs of the syncer. A special channel named 'loopback' is given to send events to the syncer itself.
- A **Pattern** is a recursive structure which contains timestamped events or other patterns (aliased as pattern begin/end events).
- A **Binding** is a link between an event recieved on input or loopback port to an internal event.