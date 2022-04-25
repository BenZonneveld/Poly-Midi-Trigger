//extern mutex_t* DataMutex;
#include <FreeRTOS.h>
#include <task.h>

extern datetime_t t;
extern TaskHandle_t xHandle;
extern TaskHandle_t usb_device_handle;
extern TaskHandle_t midi_handle;
extern TaskHandle_t display_handle;
