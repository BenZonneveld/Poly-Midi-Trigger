#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <pico/stdlib.h>
#include <hardware/rtc.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include <pico/util/datetime.h>
#include "pico/stdio/driver.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "rtos_hooks.h"

#include <bsp/board.h>
#include <tusb.h>
#include <sys/time.h>

#include "main.h"
#include "vars.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "disp_hagl.h"

#include "midi/struct.h"
#include "midi.h"
#include "cdc.h"

// static task for usbd
// Increase stack size when debug log is enabled
#if CFG_TUSB_DEBUG
#define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
#else
#define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
#endif

#define CDC_STACK_SIZE      configMINIMAL_STACK_SIZE

// static task for midi
#define MIDI_STACK_SIZE      configMINIMAL_STACK_SIZE

#define SCREEN_STACK_SIZE      configMINIMAL_STACK_SIZE

TaskHandle_t xHandle = NULL;
TaskHandle_t usb_device_handle = NULL;
TaskHandle_t midi_handle = NULL;
TaskHandle_t display_handle = NULL;

datetime_t t = {
        .year = 2020,
        .month = 06,
        .day = 05,
        .dotw = 5, // 0 is Sunday, so 5 is Friday
        .hour = 15,
        .min = 45,
        .sec = 00
};

uint8_t menu_item = 0;
uint8_t UpdateDisp = false;
int triglength = MINTRIG;
uint8_t oldnotecount = 0;
uint8_t oldparam = 0;
bool needsUpdate = false;
bool parequal = false;
uint8_t rand_length = 0;
uint8_t new_rand = 255;
bool menu_mode = false;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void usb_device_task(void* param)
{
    (void)param;

    // This should be called after scheduler/kernel is started.
    // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
    tusb_init();

    // RTOS forever loop
    while (1)
    {
        // tinyusb device task
        tud_task();
    }
}

void miditsk(void* param)
{
    while (1)
    {
        midi_task();
    }
}

int main()
{
//    set_sys_clock_khz(133000, true); /// Seems to conflict with tinyusb ?!
    board_init();
    init_midi();
    hagl_init();
    adc_init();
    adc_gpio_init(NOTELENGTH);
    adc_gpio_init(PARAM);
//TODO
// Initialize GPIO for the Trigger input

    // Create a task for tinyusb device stack
    (void)xTaskCreate(usb_device_task, "usbd", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES - 1,  &usb_device_handle);
    // Create CDC task
    (void)xTaskCreate(cdc_task, "cdc", CDC_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xHandle);

    // Create MIDI Task
    (void)xTaskCreate(miditsk, "midi", MIDI_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &midi_handle);

    // Display Task
    (void)xTaskCreate(screen_core_hagl, "screen", SCREEN_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &display_handle);

    char datetime_buf[256];
    char* datetime_str = &datetime_buf[0];

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    vTaskStartScheduler();

    return 0;
}
