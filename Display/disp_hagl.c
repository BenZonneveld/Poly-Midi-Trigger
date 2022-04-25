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
//#include "rtos_hooks.h"
#include "main.h"

#include "midi/struct.h"
//#include <midi.h>

#include <sys/time.h>

#include"cdc.h"
#include <hagl_hal.h>
#include <hagl.h>
#include <font6x9.h>
#include <font8x13O.h>
#include <fps.h>
#include <aps.h>

volatile bool fps_flag = false;
static float effect_fps;
static float display_bps;

static bitmap_t *bb;

static const uint64_t US_PER_FRAME_60_FPS = 1000000 / 60;
static const uint64_t US_PER_FRAME_30_FPS = 1000000 / 30;
static const uint64_t US_PER_FRAME_25_FPS = 1000000 / 25;

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

    fps_flag = 0;

    /* Print the message on lower left corner.*/ 
    snprintf(mesg, sizeof(mesg), "%.*f FPS  ", 0, effect_fps);
    charToWChar(mesg);
//    swprintf(wmessage, sizeof(wmessage), L"%.*f FPS  ", 0, effect_fps);
    hagl_put_text(wmessage, 4, DISPLAY_HEIGHT - 14, green, font6x9);

    /* Print the message on lower right corner.*/ 
    snprintf(mesg, sizeof(mesg), "%.*f KBPS  ", 0, display_bps / 1000);
    charToWChar(mesg);
    hagl_put_text(wmessage, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT - 14, green, font6x9);
    
    rtc_get_datetime(&t);
    snprintf(mesg, sizeof(mesg), "%.2i:%.2i:%.2i", t.hour, t.min, t.sec);
    charToWChar(mesg);
    hagl_put_text(wmessage, 4, 0 , hagl_color(255, 255, 255), font8x13O);

//    uint32_t owner_out;
//    if (mutex_try_enter(DataMutex, &owner_out))
//    {
        /* Set clip window to full screen so we can display the messages. */
//        hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

//        struct s_midi_data mdata = get_midi_data();
 
        /* Print the message on lower left corner. */
    for (int i = lines; i >= 0; i--)
    {
        mdata.cn_cin = (uint8_t)(midilog[i] >> 24);
        mdata.command = (uint8_t)(midilog[i] >> 16);
        mdata.data1 = (uint8_t)(midilog[i] >> 8);
        mdata.data2 = (uint8_t)(midilog[i] & 0xff);

        snprintf(mesg, sizeof(mesg), "%.2X %.2X %.2X %.2X   ", mdata.cn_cin, mdata.command, mdata.data1, mdata.data2);
        charToWChar(mesg);
        hagl_put_text(wmessage, 4, DISPLAY_HEIGHT - 26 - (i * 9), hagl_color(255, 255, 255), font6x9);
    }
//       mutex_exit(DataMutex);
//    }

}

void screen_core_hagl(void *param) {
    size_t bytes = 0;
    struct repeating_timer show_timer;
    char mesg[128] = { 0 };
    uint32_t ulNotifiedValue;
//    hagl_init();
    hagl_clear_screen();
    uint8_t lines = 0;
    //    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
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
