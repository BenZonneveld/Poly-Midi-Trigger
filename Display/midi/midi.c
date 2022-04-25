#include <bsp/board.h>
#include <tusb.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
//#include "rtos_hooks.h"

#include "hardware/rtc.h"
#include <pico/util/datetime.h>
//#include <sys/time.h>

#include "main.h"
#include "cdc.h"
#include "led_task.h"
#include "midi.h"
#include "struct.h"
//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

static struct s_midi_data midi_data;

//mutex_t DataMutex;
//void midi_task(void);
//void led_task(void);

struct s_midi_data get_midi_data()
{
    return midi_data;
}
/*------------- MAIN -------------*/
int init_midi(void)
{
  // Initialise UART 0
  uart_init(uart0, 31250);
  // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
  gpio_set_function(0, GPIO_FUNC_UART);
  gpio_set_function(1, GPIO_FUNC_UART);

  return 0;
}


//--------------------------------------------------------------------+
// UART Helper
//--------------------------------------------------------------------+

static size_t
__time_critical_func (uart_read) (uart_inst_t *uart, uint8_t *dst, size_t maxlen)
{
  size_t result = 0;

  while (maxlen && uart_is_readable(uart))
  {
    *dst++ = uart_getc(uart);

    result++;
    maxlen--;
  }
  
  return result;
}


//--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+

void midi_task(void)
{
  if (!tud_midi_mounted())
  {
    return;
  }

  // Handle USB to UART direction
  uint8_t packet[4];
  if (tud_midi_available() && uart_is_writable(uart0))
  {
      tud_midi_packet_read(packet);
      char msg[64] = { 0 };

      static const size_t cin_to_length[] =
      { 0, 0, 2, 3, 3, 1, 2, 3, 3, 3, 3, 3, 2, 2, 3, 1 };

      uint8_t cid = packet[0] & 0xF;
#ifdef IGNORE_MIDI_CC
      if (cid != 0x0B && cid != 0x0C)
#endif
      {
          uint32_t dmesg = ((uint32_t)packet[0] << 24) | ((uint32_t)packet[1] << 16) | ((uint32_t)packet[2] << 8) | (uint32_t)packet[3];
          xTaskNotify(display_handle, dmesg, eSetValueWithOverwrite);
          size_t length = cin_to_length[cid];
          if (length)
          {
              uart_write_blocking(uart0, packet + 1, length);
          }
      }

      led_usb_state = true;
  }
  else
  {
      led_usb_state = false;
  }

  // Handle UART to USB direction
  static uint8_t buffer[CFG_TUD_MIDI_TX_BUFSIZE/4*3];
  static uint8_t buf_pos = 0;
  static uint8_t buf_valid = 0;

  size_t length = buf_valid - buf_pos;
  if (length)
  {
      buf_pos += tud_midi_stream_write(0, buffer + buf_pos, length);
      if (buf_pos < buf_valid)
      {
          return;
      }
  }
  
  buf_valid = uart_read(uart0, buffer, sizeof buffer);
  if (buf_valid)
  {
    buf_pos = tud_midi_stream_write(0, buffer, buf_valid);

    led_uart_state = true;
  }
  else
  {
    buf_pos = 0;

    led_uart_state = false;
  }
}


// CORE Task
void midi_core(void)
{
    //    multicore_fifo_push_blocking(FLAG_VALUE);
    //    uint32_t g = multicore_fifo_pop_blocking();
    
    // Tiny USB init
//    tusb_init();

//    init_midi();

    while (true)
    {
//        tud_task();   // tinyusb device task
        led_task();
        midi_task();
    }
}
