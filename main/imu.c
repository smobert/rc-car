//#include <stdbool.h>
//#include "esp_log.h"

#include "imu.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "hal/adc_types.h"

const int X_OUT = ADC1_CHANNEL_4;
const int Y_OUT = ADC1_CHANNEL_7;
const int Z_OUT = ADC1_CHANNEL_6;

struct imu_data current_imu_data = {0,0,0};

void adc_setup(){
    adc1_config_width(ADC_WIDTH_BIT_12); // 0-4095
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12); // GPIO34 Z_OUT
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_12); // GPIO35 Y_OUT
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12); // GPIO32 X_OUT
    esp_adc_cal_characterize();
}

void read_store_imu_data(){
    int x_voltage = adc1_get_raw(X_OUT);
    int y_voltage = adc1_get_raw(Y_OUT);
    int z_voltage = adc1_get_raw(Z_OUT);
    current_imu_data.x = map_adc_to_g(x_voltage);
    current_imu_data.y = map_adc_to_g(y_voltage);
    current_imu_data.z = map_adc_to_g(z_voltage);
}

int map_adc_to_g(int voltage){
    min_g = -3;
    max_g = 3;
    // with a 12 bit resolution we divide the volatage by 2**12
    float percent = voltage/4095;
    float map = ((max_g - min_g) * percent) + min_g; // Normalize to zero then scale back down.
    return map;
}

int get_imu_x(){
    int x_out = current_imu_data.x;
    printf("imu X_Out: %d g", x_out);
}

int get_imu_y(){
    int y_out = current_imu_data.y;
    printf("imu Y_Out: %d g", y_out);
}

int get_imu_z(){
    int z_out = current_imu_data.z;
    printf("imu Z_Out: %d g", z_out);
}

void main() {
    adc_setup();
    while(1){
       read_store_imu_data(); 
       get_imu_x();
       get_imu_y();
       get_imu_z();
    }
}