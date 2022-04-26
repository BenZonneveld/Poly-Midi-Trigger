#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "vars.h"
#include "handlers.h"
#include "LinkedList.h"
#include "MovingAvg.hpp"
#include "MultiClick.h"
#include "disp_hagl.h"
#include "OptionsMenu.h"

#include "Arp.h"

movingAvg myTrigLength(DATAPOINTS);
movingAvg paramVal(DATAPOINTS);


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int min(int a, int b)
{
    return (a > b) ? b : a;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void loop(void* param)
{
#ifdef DEBUG_LOOP  
    unsigned long starttime = micros();
#endif  
    adc_select_input(NOTELENGTH_ADC_IN);
    myTrigLength.reading(adc_read() >> 3);
    adc_select_input(PARAM_ADC_IN);
    paramVal.reading(adc_read() >> 5);

    new_rand = map(paramVal.getAvg(), 0, 31, 0, 50);

    switch (checkButton())
    {
    case BTCLICK:
        if (menu_mode)
        {
            menu_item++;
            if (menu_item > MAXMENU) menu_item = 0;
            parequal = false;
            needsUpdate = true;
            OptionName(menu_item);
            OptionValue(menu_item, paramVal.getAvg());
        }
        break;
    case BTDBLC:
        menu_mode = !menu_mode;
        if (!menu_mode)
        {
            arpdir = false;
#ifdef DISP_SSD1306
            ShowDisplay();
            ShowNotes();
            ShowLength();
#endif
            needsUpdate = true;
        }
        else {
            menu_item = 0;
            parequal = false;
            needsUpdate = true;
            Options();
            OptionName(menu_item);
            OptionValue(menu_item, paramVal.getAvg());
        }
        break;
    case BTHOLD:
        if (menu_mode)
        {
#ifdef DISP_SSD1306
            display.clearDisplay();
            DoText(1, 36, true, "Saving");
            display.display();
#endif
            SaveOptions();
            busy_wait_ms(500);
            parequal = false;
            needsUpdate = true;
            Options();
            OptionName(menu_item);
            OptionValue(menu_item, paramVal.getAvg());
            needsUpdate = true;
        }
        break;
    default:
        break;
    }

    if (myTrigLength.getCount() == DATAPOINTS)
    {
        uint16_t lVal = min(myTrigLength.getAvg(), triglength);
        uint16_t hVal = max(myTrigLength.getAvg(), triglength);

        if ((hVal - lVal) > 0)
        {
            triglength = myTrigLength.getAvg();
            if (!menu_mode)
            {
#ifdef DISP_SSD1306
                ShowLength();
#endif
                needsUpdate = true;
            }
        }
    }

    if (ListLength() != oldnotecount && !menu_mode)
    {
#ifdef DISP_SSD1306
        ShowNotes();
#endif
        needsUpdate = true;
        oldnotecount = ListLength();
    }

    if (paramVal.getCount() == DATAPOINTS && paramVal.getAvg() != oldparam)
    {
        oldparam = paramVal.getAvg();
        if (menu_mode)
        {
            OptionValue(menu_item, paramVal.getAvg());
        }
        else {
            if (rand_length != new_rand)
            {
                rand_length = new_rand;
#ifdef DISP_SSD1306
                ShowRand();
#endif
                needsUpdate = true;
            }
        }
    }

    // Limit display updates
    if (UpdateDisp)
    {
#ifdef DISP_SSD1306
        // Set contrast back to normal
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(1);
#endif
        if (needsUpdate)
        {
#ifdef DISP_SSD1306
            display.display();
#endif
            needsUpdate = false;
        }
        UpdateDisp = false;
    }

/*
*   This should be moved to midi.c
* 
    midiEventPacket_t rx;
    do {
        rx = MidiUSB.read();
        if (rx.header != 0) {
            uint8_t DataType = rx.byte1 & 0xf0;
            uint8_t Channel = rx.byte1 & 0xf;
            uint8_t Note = rx.byte2;
            uint8_t Velo = rx.byte3;
            if (DataType == 0xF0)
            {
                if (rx.byte1 != 0xF0 && rx.byte1 != 0xF7 && rx.byte1 != 0xFE) // Ignore Sysex and Active Sensing
                {
                    midiB.sendRealTime(rx.byte1);
                }
            }
            else {
                MidiHandler(DataType, Note, Velo, Channel);
            }
        }
    } while (rx.header != 0);

    if (midiB.read())
    {
        MidiHandler(midiB.getType(), midiB.getData1(), midiB.getData2(), midiB.getChannel() - 1);
    }
*/
    // Check for trigger
    if (state)
    {
        // Increase brightness to indicate a trigger has been received
#ifdef DISP_SSD1306
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(0xFF);
#endif
        if (ListLength() > 0)
        {
            if (arp_mode != ARP_OFF)
            {
                Arp();
            }
            else
            {
                ListSendNoteOn();
            }
        }
        state = false;
    }

#ifdef DEBUG_LOOP
    unsigned long endtime = micros();
    unsigned long total = endtime - starttime;

    if (total > 10000)
    {
        Serial.print("Looptime: ");
        Serial.print(total);
        Serial.print(" micros, ");
        Serial.print(total / 1000);
        Serial.println(" millis");

#ifdef DEBUG_DISP
        buffer2ascii();
#endif      
    }
#endif  
}

#ifdef DEBUG_DISP
void buffer2ascii()
{
    uint8_t* buf = display.getBuffer();
    for (uint8_t i = 0; i < 128; i++)
    {
        Serial.print("#");
    }
    Serial.println();
    for (uint8_t row = 0; row < 64; row++)
    {
        for (uint8_t col = 0; col < 128; col++)
        {
            uint8_t bitpos = row % 8;
            uint8_t r = row / 8;
            if (bitRead(buf[col + (128 * r)], bitpos))
            {
                Serial.print("*");
            }
            else {
                Serial.print(" ");
            }
        }
        Serial.println("|");
    }
    for (uint8_t i = 0; i < 128; i++)
    {
        Serial.print("#");
    }
    Serial.println();
}
#endif