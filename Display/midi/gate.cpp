#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "vars.h"
#include "LinkedList.h"
#include "MovingAvg.hpp"
#include "MultiClick.h"

movingAvg myTrigLength(DATAPOINTS);
movingAvg param(DATAPOINTS);

void loop()
{
#ifdef DEBUG_LOOP  
    unsigned long starttime = micros();
#endif  
    adc_select_input(NOTELENGTH_ADC_IN);
    myTrigLength.reading(adc_read() >> 3);
    adc_select_input(PARAM_ADC_IN);
    param.reading(adc_read() >> 5);

    new_rand = map(param.getAvg(), 0, 31, 0, 50);

    switch (checkButton())
    {
    case btn_event::BTCLICK:
        if (menu_mode)
        {
            menu_item++;
            if (menu_item > MAXMENU) menu_item = 0;
            parequal = false;
            needsUpdate = true;
            OptionName(menu_item);
            OptionValue(menu_item, param.getAvg());
        }
        break;
    case btn_event::BTDBLC:
        menu_mode = !menu_mode;
        if (!menu_mode)
        {
            arpdir = false;
            ShowDisplay();
            ShowNotes();
            ShowLength();
            needsUpdate = true;
        }
        else {
            menu_item = 0;
            parequal = false;
            needsUpdate = true;
            Options();
            OptionName(menu_item);
            OptionValue(menu_item, param.getAvg());
        }
        break;
    case btn_event::BTHOLD:
        if (menu_mode)
        {
            display.clearDisplay();
            DoText(1, 36, true, "Saving");
            display.display();
            SaveOptions();
            delay(500);
            parequal = false;
            needsUpdate = true;
            Options();
            OptionName(menu_item);
            OptionValue(menu_item, param.getAvg());
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
                ShowLength();
                needsUpdate = true;
            }
        }
    }

    if (ListLength() != oldnotecount && !menu_mode)
    {
        ShowNotes();
        needsUpdate = true;
        oldnotecount = ListLength();
    }

    if (param.getCount() == DATAPOINTS && param.getAvg() != oldparam)
    {
        oldparam = param.getAvg();
        if (menu_mode)
        {
            OptionValue(menu_item, param.getAvg());
        }
        else {
            if (rand_length != new_rand)
            {
                rand_length = new_rand;
                ShowRand();
                needsUpdate = true;
            }
        }
    }

    // Limit display updates
    if (UpdateDisp)
    {
        // Set contrast back to normal
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(1);
        if (needsUpdate)
        {
            display.display();
            needsUpdate = false;
        }
        UpdateDisp = false;
    }

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

    // Check for trigger
    if (state)
    {
        // Increase brightness to indicate a trigger has been received
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(0xFF);

        if (ListLength() > 0)
        {
            if (arp_mode != mode::OFF)
            {
                Arp();
            }
            else
            {
                ListSendNoteOn();
            }
        }
        state = LOW;
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