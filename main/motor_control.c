#include "motor_control.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <stdbool.h>

#define AIN1 GPIO_NUM_14
#define AIN2 GPIO_NUM_27
#define PWMA GPIO_NUM_33
#define BIN1 GPIO_NUM_26
#define BIN2 GPIO_NUM_25
#define PWMB GPIO_NUM_32
#define STBY GPIO_NUM_10

static const char *TAG = "MOTOR";

void setup_pwm_channels() {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch[2] = {
        { .channel = LEDC_CHANNEL_0, .duty = 0, .gpio_num = PWMA, .speed_mode = LEDC_HIGH_SPEED_MODE, .timer_sel = LEDC_TIMER_0 },
        { .channel = LEDC_CHANNEL_1, .duty = 0, .gpio_num = PWMB, .speed_mode = LEDC_HIGH_SPEED_MODE, .timer_sel = LEDC_TIMER_0 }
    };
    for (int i = 0; i < 2; i++) ledc_channel_config(&ch[i]);
}

void set_motor_speed(int channel, int percent) {
    uint32_t duty = percent * 1023 / 100;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
    ESP_LOGI(TAG, "Speed chan %d -> %d%%", channel, percent);
}

void set_motor_direction(bool left_forward, bool right_forward) {
    gpio_set_level(AIN1, left_forward);
    gpio_set_level(AIN2, !left_forward);
    gpio_set_level(BIN1, right_forward);
    gpio_set_level(BIN2, !right_forward);
    ESP_LOGI(TAG, "Dir L:%s R:%s", left_forward ? "F" : "B", right_forward ? "F" : "B");
}

void stop_motors() {
    gpio_set_level(AIN1, 0);
    gpio_set_level(AIN2, 0);
    gpio_set_level(BIN1, 0);
    gpio_set_level(BIN2, 0);
    ESP_LOGI(TAG, "Motors stopped via GPIO low");
}

void motor_control_init(void) {
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << AIN1) | (1ULL << AIN2) | (1ULL << BIN1) | (1ULL << BIN2) | (1ULL << STBY)
    };
    gpio_config(&io);
    gpio_set_level(STBY, 1);
    setup_pwm_channels();
}

