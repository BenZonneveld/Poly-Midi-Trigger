#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"


void ssd1306DoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text)
{
    display.setTextSize(FONTSIZE * (FontSize + 1));
    display.setCursor(Xpos, Ypos);
    display.print(text);
}

void ssd1306CenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(FONTSIZE * (FontSize + 1));
    display.getTextBounds(text, 0, 64, &x1, &y1, &w, &h);
    display.setCursor(Xcenter - (w / 2), Ypos);
    display.print(text);
}

// a row indicates 8 display lines
void ssd1306SetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color)
{
    uint8_t* buf = display.getBuffer();

    for (uint8_t r = row; r < (row + rows); r++)
    {
        if (r > 7) break;
        memset(buf + x + (128 * r), color ? 0xff : 0, cols);
    }
}

void ssd1306ShowLength()
{
    itoa(MINTRIG + triglength, buffer, 10);
    SetDisplayPart(64, 64, 4, 4, BLACK);
    CenterText(96, 59, true, buffer);
}

void ssd1306ShowNotes()
{
    itoa(ListLength(), buffer, 10);
    SetDisplayPart(0, 64, 0, 4, BLACK);
    CenterText(32, 26, true, buffer);
}

void ssd1306ShowDisplay()
{
    display.clearDisplay();
    DoText(68, 11, false, "Active");
    strcpy_P(buffer, (char*)pgm_read_word(&(namearp[arp_mode])));
    CenterText(96, 28, false, buffer);

    DoText(1, 44, false, "Length");
    DoText(1, 62, false, "Rnd:");
    ShowRand();
}

void ssd1306ShowRand()
{
    SetDisplayPart(41, 23, 6, 2, BLACK);
    itoa(rand_length, buffer, 10);
    CenterText(50, 62, false, buffer);
}

void Options()
{
    display.clearDisplay();
    strcpy_P(buffer, (char*)pgm_read_word(&(txt_options[0])));
    CenterText(64, 12, false, buffer);
}

void OptionName(uint8_t item)
{
    SetDisplayPart(0, 128, 2, 2, BLACK);
    strcpy_P(buffer, (char*)pgm_read_word(&(nameOpt[item])));
    CenterText(64, 28, false, buffer);
}

void OptionValue(uint8_t item, uint8_t value)
{
    int val;
    uint8_t ypos = 59;
    bool fsize = true;

    if (item == 0)   // Arp Mode
    {
        fsize = false;
        ypos = 54;
        val = map(value, 0, 31, mode::OFF, mode::RANDOM);
        if (val == arp_mode && parequal == false)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (val != arp_mode)
            {
                arp_mode = val;
                needsUpdate = true;
            }
        }
        strcpy_P(buffer, (char*)pgm_read_word(&(nameArp[arp_mode])));
    }
    if (item == 1)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == rx_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (rx_channel != val)
            {
                if (val != trigger_channel)
                {
                    rx_channel = val;
                    needsUpdate = true;
                }
            }
        }
        itoa(rx_channel + 1, buffer, 10);
    }
    if (item == 2)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == tx_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (tx_channel != val)
            {
                tx_channel = val;
                needsUpdate = true;
            }
        }
        itoa(tx_channel + 1, buffer, 10);
    }
    if (item == 3)
    {
        val = map(value, 0, 31, 0, 15);
        if (val == trigger_channel && !parequal)
        {
            parequal = needsUpdate = true;
        }
        if (parequal)
        {
            if (trigger_channel != val)
            {
                if (val != rx_channel)
                {
                    trigger_channel = val;
                    needsUpdate = true;
                }
            }
        }
        itoa(trigger_channel + 1, buffer, 10);
    }

    if (needsUpdate)
    {
        if (parequal)
        {
            SetDisplayPart(0, 128, 4, 4, BLACK);
        }
        else {
            SetDisplayPart(0, 128, 4, 4, WHITE);
            display.setTextColor(BLACK);
        }
        CenterText(64, ypos, fsize, buffer);
        display.setTextColor(WHITE);
    }
}
