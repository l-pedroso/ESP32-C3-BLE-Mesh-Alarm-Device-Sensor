/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "ble_mesh_example_init.h"
#include "led_config.h"
#include "board.h"
#include "app.h"
#include "bluetooth_app.h"

#define TAG "EXAMPLE"

static void gpio_configuration(void)
{
    gpio_config_t button_config = {};

    button_config.intr_type = GPIO_INTR_ANYEDGE;
    button_config.mode = GPIO_MODE_INPUT;
    button_config.pin_bit_mask = (1ULL << BUTTON_GPIO);
    button_config.pull_down_en = 0;
    button_config.pull_up_en = 1;
    gpio_config(&button_config);
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, BUTTON_GPIO);
}

static void app_init(void)
{
    TaskHandle_t xHandle = NULL;
    uint8_t Button_Timer_ID = BUTTON_TIMER_ID;
    uint8_t Led_Timer_ID = LED_TIMER_ID;
    uint8_t Prov_Timer_ID = PROV_TIMER_ID;

    //////EVENT GROUPS CREATION******************************************************************
    xCreatedEventGroup = xEventGroupCreate();
    configASSERT(xCreatedEventGroup);
    if (xCreatedEventGroup == NULL)
    {
        ESP_LOGE(TAG, "Event Group Error");
    }
    //******************************************************************************************

    // TIMERS CREATION***********************************************************************

    //->BUTTON TIMER
    xButtonTimer = xTimerCreate("Button Timer",      // Just a text name, not used by the kernel.
                                300,                 // The timer period in ticks.
                                pdFALSE,             // The timers will auto-reload themselves when they expire.
                                &Button_Timer_ID,    // Assign each timer a unique id equal to its array index.
                                vTimerButtonCallback // Each timer calls the same callback when it expires.
    );
    configASSERT(xButtonTimer);
    if (xButtonTimer == NULL)
    {
        ESP_LOGE(TAG, "Button Timer Creation Error");
    }

    //->LED TIMER
    xLedTimer = xTimerCreate("Led Timer",      // Just a text name, not used by the kernel.
                             30,               // The timer period in ticks.
                             pdTRUE,           // The timers will auto-reload themselves when they expire.
                             &Led_Timer_ID,    // Assign each timer a unique id equal to its array index.
                             vTimerLedCallback // Each timer calls the same callback when it expires.
    );
    configASSERT(xLedTimer);
    if (xLedTimer == NULL)
    {
        ESP_LOGE(TAG, "Led Timer Creation Error");
    }

    //->PROVISION TIMER
    xProvTimer = xTimerCreate("Prov Timer",      // Just a text name, not used by the kernel.
                             2000,               // The timer period in ticks.
                             pdFALSE,           // The timers will auto-reload themselves when they expire.
                             &Prov_Timer_ID,    // Assign each timer a unique id equal to its array index.
                             vTimerProvCallback // Each timer calls the same callback when it expires.
    );
    configASSERT(xProvTimer);
    if (xProvTimer == NULL)
    {
        ESP_LOGE(TAG, "Provisioner Timer Creation Error");
    }

    //******************************************************************************************

    //////TASKS CREATION************************************************************************

    //->GPIO TASK
    xTaskCreate(gpio_handler_task, "gpio_task_example", 2048, NULL, 10, &xHandle);
    configASSERT(xHandle);
    if (xHandle == NULL)
    {
        ESP_LOGE(TAG, "Task Creation Error");
    }

    //->APP TASK
    xTaskCreate(app_task, "gpio_task_example", 2048, NULL, 10, &xHandle);
    configASSERT(xHandle);
    if (xHandle == NULL)
    {
        ESP_LOGE(TAG, "Task Creation Error");
    }
    //*****************************************************************************************
}






void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    configure_led();
    gpio_configuration();
    app_init();

    err = bluetooth_init();
    if (err)
    {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err)
    {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
