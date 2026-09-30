# Zephyr MPU6050

## Project Structure

```text
mpu6050-interrupt/
├── CMakeLists.txt
├── prj.conf
├── boards/
    └── nrf5340dk_nrf5340_cpuapp.overlay
└── src/
    └── main.cpp
```

## Hardware Connection

| nRF5340 DK | MPU6050 |
|------------|---------|
| P1.02      | SDA     |
| P1.03      | SCL     |
| VDD        | VCC     |
| GND        | GND     |