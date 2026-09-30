## ESP32

Build & Run

```bash
west build -s apps/emul-shell-gpio -p always -b esp32s3_devkitc/esp32s3/procpu --no-sysbuild \
&& west flash --runner esp32 --esp-device /dev/ttyUSB0 \
&& python3 -m serial.tools.miniterm --raw /dev/ttyUSB0 115200
```

## Native Sim

Build & Run

```bash
west build -s apps/emul-shell-gpio -p always -b native_sim/native \
&& ./apps/emul-shell-gpio/build/zephyr/zephyr.exe
```

Build & Run tests manually

```bash
west build -d apps/emul-shell-gpio/build_test -s apps/emul-shell-gpio/tests/emul_button_toggle -p always -b native_sim/native \
&& ./apps/emul-shell-gpio/build_test/zephyr/zephyr.exe
```

Build & Run tests via Twister

```bash
west twister -T apps/emul-shell-gpio/tests/emul_button_toggle -p native_sim/native
```

## nRF

Build & Run

```bash
west build -s apps/emul-shell-gpio -p always -b nrf52840dk/nrf52840 \
&& west flash --runner nrfutil \
&& python3 -m serial.tools.miniterm --raw /dev/ttyACM0 115200
```