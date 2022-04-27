/*

MIT No Attribution

Copyright (c) 2021 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

#include <pico.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <pico/stdlib.h>
#include "hardware/rtc.h"
#include <pico/util/datetime.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "main.h"
#include "vars.h"

#include "midi/struct.h"

#include <sys/time.h>

#if DEBUG
#include"cdc.h"
#endif

#include <hagl_hal.h>
#include <hagl.h>
#include <font6x13B-ISO8859-15.h>
#include <font9x18B-ISO8859-15.h>
#include <fps.h>
#include <aps.h>

#include "vars.h"
#include "LinkedList.h"
#include "disp_hagl.h"
#include "maingate.h"   // For map/min/max functions
#include "arp.h"
#include "midi.h"

char buffer[10];
static float effect_fps;
static float display_bps;

static wchar_t wmessage[64];

#define MAXLINES 10
static uint32_t midilog[MAXLINES];

static struct s_midi_data mdata;

static void charToWChar(const char* text)
{
    const size_t size = strlen(text) + 1;    
    mbstowcs(wmessage, text, size);
}

void static inline show_display_hagl(uint8_t lines) {
    color_t green = hagl_color(0, 255, 0);
    char mesg[128] = { 0 };

    /* Print the message on lower left corner.*/ 
    snprintf(mesg, sizeof(mesg), "%.*f FPS  ", 0, effect_fps);
    charToWChar(mesg);
    hagl_put_text(wmessage, 4, DISPLAY_HEIGHT - 14, green, font6x13B_ISO8859_15);

    /* Print the message on lower right corner.*/ 
    snprintf(mesg, sizeof(mesg), "%.*f KBPS  ", 0, display_bps / 1000);
    charToWChar(mesg);
    hagl_put_text(wmessage, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT - 14, green, font6x13B_ISO8859_15);
    
    rtc_get_datetime(&t);
    snprintf(mesg, sizeof(mesg), "%.2i:%.2i:%.2i", t.hour, t.min, t.sec);
    charToWChar(mesg);
    hagl_put_text(wmessage, 4, 0 , hagl_color(255, 255, 255), font9x18B_ISO8859_15);

    for (int i = lines; i >= 0; i--)
    {
        mdata.cn_cin = (uint8_t)(midilog[i] >> 24);
        mdata.command = (uint8_t)(midilog[i] >> 16);
        mdata.data1 = (uint8_t)(midilog[i] >> 8);
        mdata.data2 = (uint8_t)(midilog[i] & 0xff);

        snprintf(mesg, sizeof(mesg), "%.2X %.2X %.2X %.2X   ", mdata.cn_cin, mdata.command, mdata.data1, mdata.data2);
        charToWChar(mesg);
        hagl_put_text(wmessage, 4, DISPLAY_HEIGHT - 26 - (i * 9), hagl_color(255-(20*i), 255 - (20 * i), 255 - (20 * i)), font6x13B_ISO8859_15);
    }

}

void screen_core_hagl(void *param) {
    size_t bytes = 0;
    struct repeating_timer show_timer;
    char mesg[128] = { 0 };
    uint32_t ulNotifiedValue;

    hagl_clear_screen();
    uint8_t lines = 0;
    
    /* Set clip window to full screen so we can display the messages. */
    hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    while (1) {
        /* Update the displayed fps if requested. */
        if (xTaskNotifyWait(0, 0, &ulNotifiedValue, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            for (int i = MAXLINES; i >= 1; i--)
            {
                midilog[i] = midilog[i - 1];
            }
            snprintf(mesg, sizeof(mesg), "%.8X\r\n", ulNotifiedValue);
            midilog[0] = ulNotifiedValue;
            lines++;
            if (lines >= MAXLINES)
            {
                lines = MAXLINES;
            }
        }

        show_display_hagl(lines);
        /* Flush back buffer contents to display. NOP if single buffering. */
        bytes = hagl_flush();
    };
}


void haglDoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text)
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

void haglCenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text)
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
void haglSetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color)
{
/*    uint8_t* buf = display.getBuffer();

    for (uint8_t r = row; r < (row + rows); r++)
    {
        if (r > 7) break;
        memset(buf + x + (128 * r), color ? 0xff : 0, cols);
    }
    */
}

void haglShowLength()
{
    itoa(MINTRIG + triglength, buffer, 10);
//    SetDisplayPart(64, 64, 4, 4, BLACK);
    haglCenterText(96, 59, true, buffer);
}

void haglShowNotes()
{
    itoa(ListLength(), buffer, 10);
//    SetDisplayPart(0, 64, 0, 4, BLACK);
    haglCenterText(32, 26, true, buffer);
}

void haglShowDisplay()
{
//    display.clearDisplay();
    haglDoText(68, 11, false, "Active");
    strcpy(buffer, namearp[arp_mode]);
    haglCenterText(96, 28, false, buffer);

    haglDoText(1, 44, false, "Length");
    haglDoText(1, 62, false, "Rnd:");
    haglShowRand();
}

void haglShowRand()
{
//    SetDisplayPart(41, 23, 6, 2, BLACK);
    itoa(rand_length, buffer, 10);
    haglCenterText(50, 62, false, buffer);
}

void Options()
{
//    display.clearDisplay();
    strcpy(buffer, txt_options[0]);
    haglCenterText(64, 12, false, buffer);
}

void OptionName(uint8_t item)
{
//    SetDisplayPart(0, 128, 2, 2, BLACK);
    strcpy(buffer, nameOpt[item]);
    haglCenterText(64, 28, false, buffer);
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
        val = map(value, 0, 31, ARP_OFF, ARP_RANDOM);
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
        strcpy(buffer, nameArp[arp_mode]);
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
//            SetDisplayPart(0, 128, 4, 4, BLACK);
        }
        else {
//            SetDisplayPart(0, 128, 4, 4, WHITE);
//            display.setTextColor(BLACK);
        }
        haglCenterText(64, ypos, fsize, buffer);
//        display.setTextColor(WHITE);
    }
}