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

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "midi/struct.h"
#include <midi.h>

#include <sys/time.h>

#include <hagl_hal.h>
#include <hagl.h>
#include <font6x9.h>
#include <fps.h>
#include <aps.h>


mutex_t *DataMutex;
//mutex_t DataMutex;

volatile bool fps_flag = false;
static float effect_fps;
static float display_bps;

static bitmap_t *bb;
wchar_t message[64];

static const uint64_t US_PER_FRAME_60_FPS = 1000000 / 60;
static const uint64_t US_PER_FRAME_30_FPS = 1000000 / 30;
static const uint64_t US_PER_FRAME_25_FPS = 1000000 / 25;

bool show_timer_callback(struct repeating_timer *t) {
    fps_flag = true;
    return true;
}

void static inline show_fps() {
    color_t green = hagl_color(0, 255, 0);

    fps_flag = 0;

    /* Set clip window to full screen so we can display the messages. */
    hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    /* Print the message on lower left corner. */
    swprintf(message, sizeof(message), L"%.*f FPS  ", 0, effect_fps);
    hagl_put_text(message, 4, DISPLAY_HEIGHT - 14, green, font6x9);

    /* Print the message on lower right corner. */
    swprintf(message, sizeof(message), L"%.*f KBPS  ", 0, display_bps / 1000);
    hagl_put_text(message, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT - 14, green, font6x9);

    uint32_t owner_out;
    if (mutex_try_enter(DataMutex, &owner_out))
    {
        /* Set clip window to full screen so we can display the messages. */
//        hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

        struct s_midi_data mdata = get_midi_data();
 
        /* Print the message on lower left corner. */
        swprintf(message, sizeof(message), L"%.2X %.2X %.2X %.2X   ", mdata.cn_cin, mdata.command, mdata.data1, mdata.data2);
        hagl_put_text(message, 4, DISPLAY_HEIGHT - 28, hagl_color(255,255,255), font6x9);
        
        mutex_exit(DataMutex);
    }

    /* Set clip window back to smaller so effects do not mess the messages. */
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
}

void screen_core(void){
    size_t bytes = 0;
    struct repeating_timer show_timer;

    hagl_init();
    hagl_clear_screen();
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

    /* Update displayed FPS counter every 250 ms. */
    add_repeating_timer_ms(250, show_timer_callback, NULL, &show_timer);

    while (1) {
        /* Update the displayed fps if requested. */
        if (fps_flag) {
            show_fps();
        }


        /* Flush back buffer contents to display. NOP if single buffering. */
        bytes = hagl_flush();

        display_bps = aps(bytes);
        effect_fps = fps();

        /* Cap the demos to 60 fps. This is mostly to accommodate to smaller */
        /* screens where plasma will run too fast. */
    };

}


int main()
{
    set_sys_clock_khz(133000, true); /// Seems to conflict with tinyusb ?!
    stdio_init_all();
    mutex_init(DataMutex);

    multicore_launch_core1(midi_core);

    // Wait for it to start up

//    uint32_t g = multicore_fifo_pop_blocking();

       /* Sleep so that we have time top open serial console. */
    //   sleep_ms(5000);
    screen_core();

    while (1)
    { }

    return 0;
}
