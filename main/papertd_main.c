/* 
 * Paper towel dispenser
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "paper towel dispenser";

#define GPIO_OUTPUT_IO_0 15
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<GPIO_OUTPUT_IO_0))

static void adc_task()
{
    uint16_t adc_data;

    while (1) {
        if (ESP_OK == adc_read(&adc_data)) {
            ESP_LOGI(TAG, "adc read: %d\r\n", adc_data);
			if (adc_data < 1000) {
				gpio_set_level(GPIO_OUTPUT_IO_0, false);
			} else {
				gpio_set_level(GPIO_OUTPUT_IO_0, true);
			}
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main()
{
    // 1. init adc
    adc_config_t adc_config;

    // Depend on menuconfig->Component config->PHY->vdd33_const value
    // When measuring system voltage(ADC_READ_VDD_MODE), vdd33_const must be set to 255.
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8; // ADC sample collection clock = 80MHz/clk_div = 10MHz
    ESP_ERROR_CHECK(adc_init(&adc_config));

	// Init GPIO output
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);	

    // 2. Create a adc task to read adc value
    xTaskCreate(adc_task, "adc_task", 1024, NULL, 5, NULL);
}

