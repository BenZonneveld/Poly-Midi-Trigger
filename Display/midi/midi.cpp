#include <bsp/board.h>
#include <tusb.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
//#include "rtos_hooks.h"

#include "main.h"
//#if DEBUG
#include "cdc.h"
//#endif

#include "led_task.h"
#include "midi.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

//uint8_t tx_channel = 1;
//uint8_t rx_channel = 0;
//uint8_t trigger_channel = 2;

cMidi midi;

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
          size_t length = cin_to_length[cid];
          if (length < 3) packet[3] = 0;
          if (length < 2) packet[2] = 0;
          if (length < 1) packet[1] = 0;

          uint8_t DataType = packet[1] & 0xf0;
          uint8_t Channel = packet[1] & 0xf;
          uint8_t Note = packet[2];
          uint8_t Velo = packet[3];

          if (DataType == 0xF0)
          {
              if (packet[1] != 0xF0 && packet[1] != 0xF7 && packet[1] != 0xFE) // Ignore sysex and Active Sensing
              {
                  if (length)
                  {
                      uart_write_blocking(uart0, packet + 1, length);
                  }
              }
          } else {
              uint32_t dmesg = ((uint32_t)packet[0] << 24) | ((uint32_t)packet[1] << 16) | ((uint32_t)packet[2] << 8) | (uint32_t)packet[3];
              xTaskNotify(display_handle, dmesg, eSetValueWithOverwrite);
              midi.Handler(packet[0] << 4, packet[2], packet[3], packet[1] & 0xF);
          }
      }
      led_usb_state = true;
  }
  else
  {
      led_usb_state = false;
  }
  /*
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
  */
}

// CORE Task
//void midi_core(void)
//{
//    while (true)
//    {
//        led_task();
//        midi_task();
//    }
//}

void cMidi::setTX(uint8_t channel)
{
    if (channel < 16)
    {
        tx_channel = channel;
    }
}

void cMidi::setRX(uint8_t channel)
{
    if (channel < 16)
    {
        rx_channel = channel;
    }
}

void cMidi::setTRIG(uint8_t channel)
{
    if (channel < 16)
    {
        trigger_channel = channel;
    }
}

void cMidi::Handler(uint8_t DataType, uint8_t Note, uint8_t Velo, uint8_t Channel)
{
    char mymsg[64];
    switch (DataType)
    {
    case 0x90:  // Note on
        if (Velo > 0)
        {
            if (Channel == rx_channel) ListInsert(Note, Velo);
            if (Channel == trigger_channel) trigger_state = true;
        }
        if (Velo == 0 && Channel == rx_channel)
        {
            ListDeleteNote(Note);
            ListSort();
        }
        break;
    case 0x80: // Note Off
        if (Channel == rx_channel)
        {
            ListDeleteNote(Note);
            ListSort();
        }
        break;
    case 0xB0: // Controller
    default:
        //send back the received MIDI command
        if (Channel == rx_channel)
        {
            /*           midiEventPacket_t tx;
                       tx.byte1 = DataType | tx_channel;
                       tx.byte2 = Note;
                       tx.byte3 = Velo;
                       MidiUSB.sendMIDI(tx);
                       MidiUSB.flush();
                       //        MIDI.send(DataType, Note, Velo, tx_channel + 1);
                       midiB.send(DataType, Note, Velo, tx_channel + 1);
           */
        }
        break;
    }
}

void cMidi::noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    /*    midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
        MidiUSB.sendMIDI(noteOn);
        MidiUSB.flush();
        //  MIDI.sendNoteOn(pitch, velocity,channel + 1);
        midiB.sendNoteOn(pitch, velocity, channel + 1);
    */
}

void cMidi::noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    /*    midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
        MidiUSB.sendMIDI(noteOff);
        MidiUSB.flush();
        //  MIDI.sendNoteOff(pitch,velocity,channel + 1);
        midiB.sendNoteOff(pitch, velocity, channel + 1);
    */
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

//void controlChange(byte channel, byte control, byte value) {
//  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
//  MidiUSB.sendMIDI(event);
//}

uint8_t getMidiTX()
{
    return midi.getRX();
}

uint8_t getMidiRX()
{
    return midi.getRX();
}

uint8_t getMidiTrig()
{
    return midi.getTRIG();
}

void setMidiTX(uint8_t channel)
{
    midi.setTX(channel);
}

void setMidiRX(uint8_t channel)
{
    midi.setRX(channel);
}

void setMidiTrig(uint8_t channel)
{
    midi.setTRIG(channel);
}
