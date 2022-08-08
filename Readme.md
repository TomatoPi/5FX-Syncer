# 5FX-Syncer

A JACK Musical synchronisation tool, usable as a tracker, sequencer, arpeggiator, etc.

## Principles

This program is based on the following concepts comming from traditionnal music sequencer / trackers :

- An **Event** can represent either a MIDI event, and OSC event, an internal event like 'stop pattern n°5', etc. They are primary objects manipulated by the syncer.
- A **Timestamp** is a score related time position given as number of MIDI ticks since the begining of a pattern (MIDI ticks are send at a rate of 24 ticks per quarter note). Timestamps can also have a floating value for higher precision. Note that unlike JACK's BBT which are 1 based (first bar is bar 1, first beat is beat 1, which is very odd to define an arithmetic), timestamps are zero based and represents an offset since the begining of a pattern.
Main con representing timestamps this way is the dependency on score's metric and timeline, ie tick n° 120 represents the first beat of the second bar in 4/4 but the third beat in 3/4. We can baypass this issue using small patterns and relative positionning between patterns.
- A **Channel** is a bus where events are launched. They represents the outputs of the syncer. A special channel named 'loopback' is given to send events to the syncer itself.
- A **Pattern** is a recursive structure which contains timestamped events or other patterns (aliased as pattern begin/end events).
- A **Binding** is a link between an event recieved on input or loopback port to an internal event.

## Design

The Syncer is designed with simplicity and interoperability as main goal. Instead of a an uber program with a bunch of useless features, it provides a simple synchronous event engine form which we can build tools we need.

