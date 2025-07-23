#include <stdbool.h>
#ifndef IMU_C
#define IMU_C

struct imu_data {
    int x;
    int y;
    int z;
};

void read_store_imu_data();

int imu_get_x();
int imu_get_y();
int imu_get_z();

#endif