## Polyphonic Midi Trigger

A long time ago when I first started making music I used the trigger output from a drum machine to gate the notes I pressed on the analog cv/gate synth I had.
This way I only had to press the correct keys as the cv information was still comming from the keyboard while the rhythm came from the drum machine.

I wanted to recreate this while adding polyphonic features.

## Features
* Adjustable gate duration with optionial randomness on the duration
* Responds to trigger/gate
* Keyboard and Trigger channels via MIDI. These can't be the same channel
* Optional Arppegiatior which plays the next note only after a trigger has been received.
* USB MIDI and standard MIDI

## Hardware needed
RaspberryPi Pico
2 potentiometers
Omrohn type button
2* 1N136 optocoupler
MIDI Jacks
6.3 mm jack for trigger input
128x64 SPI Display
