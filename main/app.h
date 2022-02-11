#ifndef _APP_H_
#define _APP_H_
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#define BUTTON_PRESS (1ULL << 0)
#define BUTTON_RELEASED (1ULL << 1)
#define PROV_INIT (1ULL << 2)
#define PROV_END (1ULL << 3)
#define BUTTON_GPIO 9ULL
#define BUTTON_TIMER_ID 100ULL
#define LED_TIMER_ID 101ULL
#define PROV_TIMER_ID 101ULL

EventGroupHandle_t xCreatedEventGroup;
TimerHandle_t xButtonTimer;
TimerHandle_t xLedTimer;
TimerHandle_t xProvTimer;

void gpio_handler_task(void *args);
void app_task(void *args);
void IRAM_ATTR gpio_isr_handler(void *arg);
void vTimerButtonCallback(TimerHandle_t pxTimer);
void vTimerLedCallback(TimerHandle_t pxTimer);
void vTimerProvCallback(TimerHandle_t pxTimer);

#endif