//--------------------------------------------------------------------+
// LED Task
//--------------------------------------------------------------------+

#include <bsp/board.h>

static bool led_uart_state = false;
static bool led_usb_state = false;

void led_task(void)
{
    static uint32_t last_active_ms = 0;

    if (led_usb_state || led_uart_state)
    {
        board_led_write(true);

        last_active_ms = board_millis();
    }
    else if (board_millis() - last_active_ms > 10)
    {
        board_led_write(false);
    }
}
