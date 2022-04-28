//#include "vars.h"
#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <wchar.h>

#include "defines.h"
#include "midi.h"
#include "tools.h"
#include "MultiClick.h"
#include "MovingAvg.h"
#include "OptionsMenu.h"
#include <hagl_hal.h>
#include <hagl.h>
#include "disp_hagl.h"

cOptions Opt;

void LoadOptions()
{
    // Arp Mode
//  arp_mode = EEPROM.read(eeprom::ARP);

//    if ((int8_t)EEPROM.read(eeprom::ARP) >= mode::OFF && (int8_t)EEPROM.read(eeprom::ARP) <= mode::RANDOM)
//    {
//        arp_mode = EEPROM.read(eeprom::ARP);
//    }
//    else {
    setArpMode(ARP_OFF);
    //    }

        // RX Channel
    //    if ((int8_t)EEPROM.read(eeprom::RX) >= 0 && (int8_t)EEPROM.read(eeprom::RX) < 16)
    //    {
    //        rx_channel = EEPROM.read(eeprom::RX);
    //    }
    //    else {
    setMidiRX(0);
    //    }

        // TX Channel
    //    if ((int8_t)EEPROM.read(eeprom::TX) >= 0 && (int8_t)EEPROM.read(eeprom::TX) < 16)
    //    {
    //        tx_channel = EEPROM.read(eeprom::TX);
    //    }
    //    else {
    setMidiTX(0);
    //    }

        // Trigger channel
    //    if ((int8_t)EEPROM.read(eeprom::TRIG) >= 0 && (int8_t)EEPROM.read(eeprom::TRIG) < 16)
    //    {
    //        trigger_channel = EEPROM.read(eeprom::TRIG);
    //    }
    //    else {
    setMidiTrig(2);
    //    }
}

void SaveOptions()
{
    //    EEPROM.update(eeprom::ARP, (uint8_t)arp_mode);
    //    EEPROM.update(eeprom::RX, (uint8_t)rx_channel);
    //    EEPROM.update(eeprom::TX, (uint8_t)tx_channel);
    //    EEPROM.update(eeprom::TRIG, (uint8_t)trigger_channel);
}

void cOptions::Options()
{
    //    adc_select_input(NOTELENGTH_ADC_IN);
//    myTrigLength.reading(adc_read() >> 3);
//    adc_select_input(PARAM_ADC_IN);
//    paramVal.reading(adc_read() >> 5);

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
            midi.resetArp();
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
            strcpy(buffer, txt_options[0]);
            CenterText(64, 12, false, buffer);
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
            strcpy(buffer, txt_options[0]);
            CenterText(64, 12, false, buffer);
            OptionName(menu_item);
            OptionValue(menu_item, paramVal.getAvg());
            needsUpdate = true;
        }
        break;
    default:
        break;
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

    if (myTrigLength.getCount() == DATAPOINTS)
    {
        uint16_t lVal = min(myTrigLength.getAvg(), Opt.getTrigLength());
        uint16_t hVal = max(myTrigLength.getAvg(), Opt.getTrigLength());

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

    if (midi.ListLength() != oldnotecount && !menu_mode)
    {
#ifdef DISP_SSD1306
        ShowNotes();
#endif
        needsUpdate = true;
        oldnotecount = midi.ListLength();
    }

    // Limit display updates
    if (needsUpdate)
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
}

void cOptions::ShowLength()
{
    itoa(MINTRIG + getTrigLength(), buffer, 10);
    //    SetDisplayPart(64, 64, 4, 4, BLACK);
    CenterText(96, 59, true, buffer);
}

void cOptions::ShowNotes()
{
    //TODO    itoa(ListLength(), buffer, 10);
    //    SetDisplayPart(0, 64, 0, 4, BLACK);
    CenterText(32, 26, true, buffer);
}

void cOptions::ShowDisplay()
{
    //    display.clearDisplay();
    DoText(68, 11, false, (char*)"Active");
    strcpy(buffer, namearp[getArpMode()]);
    CenterText(96, 28, false, buffer);

    DoText(1, 44, false, (char*)"Length");
    DoText(1, 62, false, (char*)"Rnd:");
    ShowRand();
}

void cOptions::ShowRand()
{
    //    SetDisplayPart(41, 23, 6, 2, BLACK);
    itoa(getRandLength(), buffer, 10);
    CenterText(50, 62, false, buffer);
}

void cOptions::OptionName(uint8_t item)
{
    //    SetDisplayPart(0, 128, 2, 2, BLACK);
    strcpy(buffer, nameOpt[item]);
    CenterText(64, 28, false, buffer);
}

void cOptions::OptionValue(uint8_t item, uint8_t value)
{
    int val;
    uint8_t ypos = 59;
    bool fsize = true;

    if (item == 0)   // Arp Mode
    {
        fsize = false;
        ypos = 54;
        val = map(value, 0, 31, ARP_OFF, ARP_RANDOM);
        if (val == getArpMode() && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (val != (uint8_t)getArpMode())
            {
                setArpMode((e_mode)val);
                needsUpdate = true;
            }
        }
        strcpy(buffer, nameArp[getArpMode()]);
    }
    if (item == 1)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == getMidiRX() && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (getMidiRX() != val)
            {
                if (val != getMidiTrig())
                {
                    setMidiRX(val);
                    needsUpdate = true;
                }
            }
        }
        itoa(getMidiRX() + 1, buffer, 10);
    }
    if (item == 2)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == getMidiTX() && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (getMidiTX() != val)
            {
                setMidiTX(val);
                needsUpdate = true;
            }
        }
        itoa(getMidiTX() + 1, buffer, 10);
    }
    if (item == 3)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == getMidiTrig() && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (getMidiTrig() != val)
            {
                if (val != getMidiRX())
                {
                    setMidiTrig(val);
                    needsUpdate = true;
                }
            }
        }
        itoa(getMidiTrig() + 1, buffer, 10);
    }

    if (needsUpdate)
    {
        if (parequal)
        {
            //            SetDisplayPart(0, 128, 4, 4, BLACK);
        }
        else {
            //            SetDisplayPart(0, 128, 4, 4, WHITE);
            //            display.setTextColor(BLACK);
        }
        CenterText(64, ypos, fsize, buffer);
        //        display.setTextColor(WHITE);
    }
}

void cOptions::DoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text)
{
    charToWChar(text);
    if (FontSize)
    {
        hagl_put_text(wmessage, Xpos, Ypos, hagl_color(255, 255, 255), font9x18B_ISO8859_15);
    }
    else
    {
        hagl_put_text(wmessage, Xpos, Ypos, hagl_color(255, 255, 255), font6x13B_ISO8859_15);
    }
}

void cOptions::CenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text)
{
    charToWChar(text);

    int16_t x1, y1;
    uint16_t w, h;
    //    display.getTextBounds(text, 0, 64, &x1, &y1, &w, &h);
    //    display.setCursor(Xcenter - (w / 2), Ypos);
    //    display.print(text);
    if (FontSize)
    {
        hagl_put_text(wmessage, Xcenter, Ypos, hagl_color(255, 255, 255), font9x18B_ISO8859_15);
    }
    else
    {
        hagl_put_text(wmessage, Xcenter, Ypos, hagl_color(255, 255, 255), font6x13B_ISO8859_15);
    }
}

// a row indicates 8 display lines
void cOptions::SetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color)
{
    /*    uint8_t* buf = display.getBuffer();

        for (uint8_t r = row; r < (row + rows); r++)
        {
            if (r > 7) break;
            memset(buf + x + (128 * r), color ? 0xff : 0, cols);
        }
        */
}

uint8_t getTrigLength()
{
    return Opt.getTrigLength();
}

uint8_t getRandLength()
{
    return Opt.getRandLength();
}

void OptionsLoop(void* param)
{
    Opt.Options();
}