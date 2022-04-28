#include <pico.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <pico/stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "main.h"

#include "defines.h"

#include <sys/time.h>

#if DEBUG
#include"cdc.h"
#endif

#include <hagl_hal.h>
#include <hagl.h>
#include <font6x13B-ISO8859-15.h>
#include <font9x18B-ISO8859-15.h>

#include "defines.h"
#include "tools.h"
//#include "disp_hagl.h"
#include "midi.h"
#include "OptionsMenu.h"

#define MAXLINES 5
static uint32_t midilog[MAXLINES];

void static inline show_display_hagl(uint8_t lines) {
    color_t green = hagl_color(0, 255, 0);
    char mesg[128] = { 0 };
    hagl_set_clip_window(0, 64, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    for (int i = lines; i >= 0; i--)
    {
        uint8_t cn_cin = (uint8_t)(midilog[i] >> 24);
        uint8_t command = (uint8_t)(midilog[i] >> 16);
        uint8_t channel = (uint8_t)(midilog[i]);
        uint8_t data1 = (uint8_t)(midilog[i] >> 8);
        uint8_t data2 = (uint8_t)(midilog[i] & 0xff);

        snprintf(mesg, sizeof(mesg), "%.2X %.2X %.2X %.2X   ", cn_cin, command, data1, data2);
        charToWChar(mesg);
        hagl_put_text(wmessage, 4, DISPLAY_HEIGHT - 14 - (i * 13), hagl_color(255-(20*i), 255 - (20 * i), 255 - (20 * i)), font6x13B_ISO8859_15);
    }
    hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
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
        OptionsLoop(NULL);
 //       main_gater();
        /* Flush back buffer contents to display. NOP if single buffering. */
        bytes = hagl_flush();
    };
}

void init_display()
{
    hagl_init();
}
