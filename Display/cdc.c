#include <bsp/board.h>
#include <tusb.h>
//#include <hardware/uart.h>
//#include <hardware/gpio.h>

#include <FreeRTOS.h>
#include "semphr.h"
#include <queue.h>
#include <task.h>

#include "main.h"
#include "led_task.h"

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void tud_print(char* msg)
{
    tud_cdc_write(msg, sizeof(msg));
    tud_cdc_write_flush();
}

void cdc_task(void* params)
{
    (void)params;

    char msg[64] = { 0 };
    // RTOS forever loop
    while (1)
    {
        // connected() check for DTR bit
        // Most but not all terminal client set this when making connection
        // if ( tud_cdc_connected() )

        // There are data available
        if (tud_cdc_available())
        {
            snprintf(msg, sizeof(msg), "cdc task\r\n");
            tud_cdc_write(msg, sizeof(msg));
            tud_cdc_write_flush();

            led_usb_state = true;

            uint8_t buf[64];

            // read and echo back
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            (void)count;

            // Echo back
            // Note: Skip echo by commenting out write() and write_flush()
            // for throughput test e.g
            //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
        else {
            led_usb_state = false;
        }
        led_task();


        // For ESP32-S2 this delay is essential to allow idle how to run and reset wdt
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void)itf;
    (void)rts;

    // TODO set some indicator
    if (dtr)
    {
        // Terminal connected
    }
    else
    {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;
}
