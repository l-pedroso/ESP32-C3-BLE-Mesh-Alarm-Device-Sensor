
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "app.h"
#include "led_config.h"
#include "ble_mesh_example_init.h"
#include "bluetooth_app.h"

#define TAG "example"

void IRAM_ATTR gpio_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
    uint8_t button_state = 0;
    uint32_t gpio_num = (uint32_t)arg;

    xHigherPriorityTaskWoken = pdFALSE;

    if (!gpio_get_level(BUTTON_GPIO))
    {
        button_state = BUTTON_PRESS;
    }
    else
    {
        button_state = BUTTON_RELEASED;
    }

    xResult = xEventGroupSetBitsFromISR(
        xCreatedEventGroup, // The event group being updated.
        button_state,       // The bits being set.
        &xHigherPriorityTaskWoken);

    // Was the message posted successfully?
    if (xResult == pdPASS)
    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

void vTimerButtonCallback(TimerHandle_t pxTimer)
{
    EventBits_t uxBits;

    configASSERT(pxTimer);

    ESP_LOGI(TAG, "long press");

    uxBits = xEventGroupSetBits(
        xCreatedEventGroup, // The event group being updated.
        PROV_INIT);         // The bits being set.
}

void vTimerLedCallback(TimerHandle_t pxTimer)
{

    static uint8_t led_status = 0;

    led_status = !led_status;

    blink_led(led_status);
}

void vTimerProvCallback(TimerHandle_t pxTimer)
{
    EventBits_t uxBits;

    configASSERT(pxTimer);

    uxBits = xEventGroupSetBits(
        xCreatedEventGroup, // The event group being updated.
        PROV_END);          // The bits being set.
}

void gpio_handler_task(void *args)
{
    EventBits_t uxBits;
    TickType_t expiration_time;

    for (;;)
    {
        uxBits = xEventGroupWaitBits(
            xCreatedEventGroup,             // The event group being tested.
            BUTTON_PRESS | BUTTON_RELEASED, // The bits within the event group to wait for.
            pdTRUE,                         // BIT_0 and BIT_4 should be cleared before returning.
            pdFALSE,                        // Don't wait for both bits, either bit will do.
            100);                           // Wait a maximum of 100ms for either bit to be set.

        if (uxBits & BUTTON_PRESS)
        {
            ESP_LOGI(TAG, "button press");
            if (xTimerReset(xButtonTimer, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "Timer Reset Error");
            }
        }
        else if (uxBits & BUTTON_RELEASED)
        {

            ESP_LOGI(TAG, "button released");
            if (xTimerStop(xButtonTimer, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "Timer Reset Error");
            }

            publish_trigger();
        }
    }
}

void app_task(void *args)
{
    EventBits_t uxBits;
    esp_err_t err;

    for (;;)
    {
        uxBits = xEventGroupWaitBits(
            xCreatedEventGroup,   // The event group being tested.
            PROV_INIT | PROV_END, // The bits within the event group to wait for.
            pdTRUE,               // BIT_0 and BIT_4 should be cleared before returning.
            pdFALSE,              // Don't wait for both bits, either bit will do.
            100);

        switch (uxBits)
        {

        case PROV_INIT:
            ESP_LOGI(TAG, "PROV INIT");
            if (xTimerStart(xLedTimer, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "Led Timer Start Error");
            }

            if (xTimerStart(xProvTimer, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "Prov Timer Start Error");
            }

            err = ble_mesh_reset();
            if (err)
            {
                ESP_LOGE(TAG, "Prov reset error");
            }
            break;

        case PROV_END:
            ESP_LOGI(TAG, "PROV END");
            if (xTimerStop(xLedTimer, 0) != pdPASS)
            {
                ESP_LOGE(TAG, "Let Timer Stop Error");
            }
            blink_led(false);
            break;
        }
    }
}