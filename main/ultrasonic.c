#include <stdio.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TRIGGER_GPIO 	GPIO_NUM_4
#define ECHO_GPIO    	GPIO_NUM_16

#define HC_SR04_TIMEOUT_US 25000 // 25ms timeout (~4m max range)

static const char *TAG = "ULTRA";

void ultrasonic_init(void)
{
    gpio_set_direction(TRIGGER_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(TRIGGER_GPIO, 0);  // Ensure LOW initially

    gpio_set_direction(ECHO_GPIO, GPIO_MODE_INPUT);
}

float ultrasonic_measure(void)
{
    // Send 10us HIGH pulse
    gpio_set_level(TRIGGER_GPIO, 0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIGGER_GPIO, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIGGER_GPIO, 0);

    // Wait for echo to go HIGH
    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(ECHO_GPIO) == 0) {
        if (esp_timer_get_time() - start_time > HC_SR04_TIMEOUT_US)
            return -1.0; // Timeout waiting for echo HIGH
    }

    // Measure HIGH pulse duration
    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(ECHO_GPIO) == 1) {
        if (esp_timer_get_time() - echo_start > HC_SR04_TIMEOUT_US)
            return -1.0; // Timeout waiting for echo LOW
    }
    int64_t echo_end = esp_timer_get_time();

    // Calculate distance in cm
    int64_t pulse_duration = echo_end - echo_start; // in microseconds
    float distance_cm = (pulse_duration / 2.0) * 0.0343; // Speed of sound
    
    ESP_LOGI(TAG, "Ultrasonic distance measured %f", distance_cm);
    
    return distance_cm;
}

