#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "motor_control.h"
#include "servo_control.h"
#include "ultrasonic.h"
#include "web_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP32 RC Car");
    ESP_ERROR_CHECK(nvs_flash_init());

    motor_control_init();
    servo_control_init();
    ultrasonic_init();
    start_webserver();

   }

