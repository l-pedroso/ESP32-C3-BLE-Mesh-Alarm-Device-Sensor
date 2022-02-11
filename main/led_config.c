#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "led_config.h"


static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO




static led_strip_t *pStrip_a;

void blink_led(uint8_t s_led_state)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 59, 0, 111);
        /* Refresh the strip to send data */
        pStrip_a->refresh(pStrip_a, 100);
        s_led_state = 0;
    } else {
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
        s_led_state = 1;
    }
}

 void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(1, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}






