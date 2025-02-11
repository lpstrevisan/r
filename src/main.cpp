#include <header.h>

MPU6050 mpu(PIN_SDA, PIN_SCL); // Assuming I2C is connected to PF_0 (SDA) and PF_1 (SCL)
Ticker gyroTicker;

int main() {
    mpu.initialize();
    calibrate_gyro();
    gyroTicker.attach(&flag_gyro, SAMPLE_TIME);
    
    while (true) {
        if(is_ready) {
            is_ready = false;
            read_gyro();
            filter_gyro();
        }
        // Integrate to get angular position (simple Euler integration)
        angle = angular_velocity * SAMPLE_TIME;

        printf("Angular Position (rad): %f\n", angle);
        ThisThread::sleep_for(4ms);
    }

    return 0;
}


void calibrate_gyro() {
    int16_t gyro_raw[3];
    double sum = 0;

    for(int i = 0; i < CALIBRATE_SAMPLES; i++) {
        mpu.readGyroRaw(gyro_raw);
        sum += gyro_raw[2];
    }

    gyro_offset_z = sum / CALIBRATE_SAMPLES;
}

void flag_gyro() {
    is_ready = true;
}

void read_gyro() {
    double gyro_data[3] = {0};
    mpu.readGyro(gyro_data);
    
    // Convert gyro data from degrees/sec to radians/sec
    gyro_z_rad = (gyro_data[2] - gyro_offset_z) * DEG_TO_RAD;
}

void filter_gyro() {
    // Apply simple low-pass filter
    angular_velocity = (ALPHA * angular_velocity) + ((1 - ALPHA) * gyro_z_rad);
}
