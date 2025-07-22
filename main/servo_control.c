#include "servo_control.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "SERVO";

#define SERVO_MIN_PULSEWIDTH_US 500    // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500   // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE        180    // Maximum angle

#define LEDC_TIMER              LEDC_TIMER_2
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO          (18) // Set to your GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_2
#define LEDC_DUTY_RES           LEDC_TIMER_16_BIT
#define LEDC_FREQUENCY          (50) // Frequency in Hertz. 50Hz for servo

void servo_control_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void set_servo(int pos)
{
    if (pos > SERVO_MAX_DEGREE) pos = SERVO_MAX_DEGREE;

    // Calculate pulse width in microseconds
    uint32_t pulsewidth = SERVO_MIN_PULSEWIDTH_US + (((SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) * pos) / SERVO_MAX_DEGREE);

    // Convert pulse width to duty based on resolution and frequency
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    uint32_t duty = (pulsewidth * max_duty) / (1000000 / LEDC_FREQUENCY);
    
    ESP_LOGI(TAG, "Servo actually moved (?) to %d", pos);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

