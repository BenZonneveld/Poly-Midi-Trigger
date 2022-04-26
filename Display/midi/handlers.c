#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
//#include "vars.h"
#include"handlers.h"
#include "LinkedList.h"

#include <string.h>
#if DEBUG
#include "cdc.h"
#endif

uint8_t tx_channel = 1;
uint8_t rx_channel = 0;
uint8_t trigger_channel = 2;

bool state = false;

void MidiHandler(uint8_t DataType, uint8_t Note, uint8_t Velo, uint8_t Channel)
{
    switch (DataType)
    {
    case 0x90:  // Note on
        if (Velo > 0)
        {
            if (Channel == rx_channel) ListInsert(Note, Velo);
            if (Channel == trigger_channel) state = true;
        }
        if (Velo == 0 && Channel == rx_channel)
        {
            ListDeleteNote(Note);
            ListSort();
        }
        break;
    case 0x80: // Note Off
        if (Channel == rx_channel)
        {
            ListDeleteNote(Note);
            ListSort();
        }
        break;
    case 0xB0: // Controller
    default:
        //send back the received MIDI command
        if (Channel == rx_channel)
        {
 /*           midiEventPacket_t tx;
            tx.byte1 = DataType | tx_channel;
            tx.byte2 = Note;
            tx.byte3 = Velo;
            MidiUSB.sendMIDI(tx);
            MidiUSB.flush();
            //        MIDI.send(DataType, Note, Velo, tx_channel + 1);
            midiB.send(DataType, Note, Velo, tx_channel + 1);
*/
        }
        break;
    }
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
/*    midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOn);
    MidiUSB.flush();
    //  MIDI.sendNoteOn(pitch, velocity,channel + 1);
    midiB.sendNoteOn(pitch, velocity, channel + 1);
*/
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
/*    midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOff);
    MidiUSB.flush();
    //  MIDI.sendNoteOff(pitch,velocity,channel + 1);
    midiB.sendNoteOff(pitch, velocity, channel + 1);
*/
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

//void controlChange(byte channel, byte control, byte value) {
//  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
//  MidiUSB.sendMIDI(event);
//}