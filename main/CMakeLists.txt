
idf_component_register(SRCS 
    "app_main.c"
    "motor_control.c"
    "servo_control.c"
    "ultrasonic.c"
    "web_server.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_ledc esp_driver_gpio nvs_flash esp_wifi esp_http_server driver esp_timer
)

