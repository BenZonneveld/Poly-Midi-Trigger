#pragma once
#ifndef PICO_MIDI
#define PICO_MIDI

#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif

//#define IGNORE_MIDI_CC

#define FLAG_VALUE 123

int init_midi(void);
void midi_core(void);
void tud_mount_cb(void);
void tud_umount_cb(void);
void tud_suspend_cb(bool remote_wakeup_en);
void tud_resume_cb(void);
static size_t __time_critical_func (uart_read) (uart_inst_t *uart, uint8_t *dst, size_t maxlen);
void midi_task(void);
struct s_midi_data get_midi_data();
#endif

