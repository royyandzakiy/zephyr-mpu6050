#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

void read_loop()
{
    printk("MPU6050 ready, reading every 5 seconds...\n");

    while (1) {
        struct sensor_value accel[3];
        struct sensor_value gyro[3];
        struct sensor_value temp;

        if (sensor_sample_fetch(mpu) < 0) {
            printk("Failed to fetch sensor data\n");
            k_sleep(K_SECONDS(5));
            continue;
        }

        sensor_channel_get(mpu, SENSOR_CHAN_ACCEL_XYZ, accel);
        sensor_channel_get(mpu, SENSOR_CHAN_GYRO_XYZ, gyro);
        sensor_channel_get(mpu, SENSOR_CHAN_AMBIENT_TEMP, &temp);

        printk("Accel: X=%d.%06d, Y=%d.%06d, Z=%d.%06d\n", accel[0].val1, accel[0].val2,
               accel[1].val1, accel[1].val2, accel[2].val1, accel[2].val2);

        printk("Gyro:  X=%d.%06d, Y=%d.%06d, Z=%d.%06d\n", gyro[0].val1, gyro[0].val2, gyro[1].val1,
               gyro[1].val2, gyro[2].val1, gyro[2].val2);

        printk("Temp:  %d.%06d C\n\n", temp.val1, temp.val2);

        k_sleep(K_SECONDS(5));
    }
}

int main()
{
    const struct device *mpu = DEVICE_DT_GET(DT_NODELABEL(mpu6050));

    if (!device_is_ready(mpu)) {
        printk("MPU6050 device not ready\n");
        return 0;
    }

    // read_loop();

    return 0;
}
