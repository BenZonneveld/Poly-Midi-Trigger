#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"


#include "defines.h"

//#include "MultiClick.h"
//#include "disp_hagl.h"
#include "OptionsMenu.h"

#include "midi.h"

void main_gater(void)
{
/*
*   This should be moved to midi.c
* 
    if (midiB.read())
    {
        MidiHandler(midiB.getType(), midiB.getData1(), midiB.getData2(), midiB.getChannel() - 1);
    }
*/
    // Check for trigger
    if (midi.getTrigState())
    {
        // Increase brightness to indicate a trigger has been received
#ifdef DISP_SSD1306
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(0xFF);
#endif
        if (midi.ListLength() > 0)
        {
            if (midi.getArpMode() != ARP_OFF)
            {
                midi.Arp();
            }
            else
            {
                midi.ListSendNoteOn();
            }
        }
        midi.resetTrigState();
    }
}
