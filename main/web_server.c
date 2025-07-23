#include "web_server.h"
#include "motor_control.h"
#include "servo_control.h"
#include "ultrasonic.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "driver/ledc.h"
#include <string.h>
#include <stdbool.h>

#define WIFI_SSID "ESP_Car"
#define WIFI_PASS "drive1234"

static const char *TAG = "WEB";

int previousspeed = 50;

void wifi_init_softap(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi AP...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    if (strlen(WIFI_PASS) == 0) {
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "AP ready: SSID='%s', PASS='%s'", WIFI_SSID, WIFI_PASS);
}

// -- HTTP Handlers --

esp_err_t move_start_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "HTTP: move/start");
    char query[64], dir[16];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        if (httpd_query_key_value(query, "dir", dir, sizeof(dir)) == ESP_OK) {
            if (strcmp(dir, "forward") == 0) set_motor_direction(true, true);
            else if (strcmp(dir, "backward") == 0) set_motor_direction(false, false);
            else if (strcmp(dir, "left") == 0) {
		    set_motor_direction(true, true);
		    set_motor_speed(LEDC_CHANNEL_0, 0);
	    }
            else if (strcmp(dir, "right") == 0) {
		    set_motor_direction(true, true);
		    set_motor_speed(LEDC_CHANNEL_1, 0);
	    }
        }
    }
    return httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
}

esp_err_t move_stop_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "HTTP: move/stop");
    stop_motors();
    set_motor_speed(LEDC_CHANNEL_0, previousspeed);
    set_motor_speed(LEDC_CHANNEL_1, previousspeed);

    return httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
}

esp_err_t speed_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "HTTP: /speed");
    char buf[32], val[8];
    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        if (httpd_query_key_value(buf, "val", val, sizeof(val)) == ESP_OK) {
            int spd = atoi(val);
	    previousspeed = spd;
            set_motor_speed(LEDC_CHANNEL_0, spd);
            set_motor_speed(LEDC_CHANNEL_1, spd);
        }
    }
    return httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
}

esp_err_t servo_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "HTTP: /servo");
    char buf[32], val[8];
    int pos = 90;
    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        if (httpd_query_key_value(buf, "val", val, sizeof(val)) == ESP_OK) {
            pos = atoi(val);
            set_servo(pos);
        }
    }
    ESP_LOGI(TAG, "Servo pos set to %d", pos);
    return httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
}

esp_err_t sensor_handler(httpd_req_t *req) {
	ESP_LOGI(TAG, "HTTP: /sensor");

	int distance = (int)ultrasonic_measure();
	
	char resp[128];
	snprintf(resp, sizeof(resp), "{\"distance\":%d}", distance);

	httpd_resp_set_type(req, "application/json");

	ESP_LOGI(TAG, "Updating sensor data");
	return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}

// -- Start Server --
void start_webserver() {
    wifi_init_softap();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_start(&server, &config);

    httpd_uri_t mstart = { .uri = "/move/start", .method = HTTP_POST, .handler = move_start_handler };
    httpd_uri_t mstop = { .uri = "/move/stop", .method = HTTP_POST, .handler = move_stop_handler };
    httpd_uri_t speed = { .uri = "/speed", .method = HTTP_POST, .handler = speed_handler };
    httpd_uri_t servo = { .uri = "/servo", .method = HTTP_POST, .handler = servo_handler };
    httpd_uri_t sensor = { .uri = "/sensor", .method = HTTP_GET, .handler = sensor_handler };

    //httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &mstart);
    httpd_register_uri_handler(server, &mstop);
    httpd_register_uri_handler(server, &speed);
    httpd_register_uri_handler(server, &servo);
    httpd_register_uri_handler(server, &sensor);

    ESP_LOGI(TAG, "HTTP server running");
}

