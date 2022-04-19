#include "bsp/board.h"
#include "tusb.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

//#define IGNORE_MIDI_CC

static bool led_usb_state = false;
static bool led_uart_state = false;

void midi_task(void);
void led_task(void);

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

void do_midi(void)
{
    tud_task();   // tinyusb device task
    led_task();
    midi_task();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  led_usb_state = false;
  led_uart_state = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
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

    static const size_t cin_to_length[] =
      {0, 0, 2, 3, 3, 1, 2, 3, 3, 3, 3, 3, 2, 2, 3, 1};

    uint8_t cid = packet[0] & 0xF;
#ifdef IGNORE_MIDI_CC
    if (cid != 0x0B && cid != 0x0C)
#endif
    {
      size_t length = cin_to_length[cid];
      if (length)
      {
        uart_write_blocking(uart0, packet+1, length);
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

//--------------------------------------------------------------------+
// LED Task
//--------------------------------------------------------------------+

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