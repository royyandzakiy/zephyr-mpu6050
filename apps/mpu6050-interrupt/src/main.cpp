#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>

#define MPU_NODE DT_NODELABEL(mpu6050)

/* MPU6050 registers */
#define REG_ACCEL_CONFIG 0x1C
#define REG_MOT_THR 0x1F
#define REG_MOT_DUR 0x20
#define REG_INT_PIN_CFG 0x37
#define REG_INT_ENABLE 0x38
#define REG_INT_STATUS 0x3A
#define REG_PWR_MGMT_1 0x6B
#define REG_WHO_AM_I 0x75

#define INT_MOT_BIT BIT(6)

static const struct i2c_dt_spec mpu = I2C_DT_SPEC_GET(MPU_NODE);
static const struct gpio_dt_spec mpu_int = GPIO_DT_SPEC_GET(MPU_NODE, int_gpios);

static struct gpio_callback int_cb;
static struct k_work jolt_work;

/* your callback */
static void on_jolt(void)
{
    printk("jolt! t=%u ms\n", k_uptime_get_32());
}

/* runs in the system workqueue, so I2C is allowed here */
#define JOLT_DEBOUNCE_MS 2000

static void jolt_work_handler(struct k_work *work)
{
    static int64_t last_jolt = -JOLT_DEBOUNCE_MS;
    uint8_t status;

    /* always read INT_STATUS so the latched pin gets released */
    if (i2c_reg_read_byte_dt(&mpu, REG_INT_STATUS, &status) || !(status & INT_MOT_BIT)) {
        return;
    }

    int64_t now = k_uptime_get();

    if (now - last_jolt < JOLT_DEBOUNCE_MS) {
        return;
    }

    last_jolt = now;
    on_jolt();
}

/* ISR: no I2C here, just hand off */
static void int_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    k_work_submit(&jolt_work);
}

static int mpu_init(void)
{
    uint8_t id;
    int ret;

    ret = i2c_reg_read_byte_dt(&mpu, REG_WHO_AM_I, &id);
    if (ret || id != 0x68) {
        printk("MPU6050 not found (ret=%d id=0x%02x)\n", ret, id);
        return -ENODEV;
    }

    const uint8_t cfg[][2] = {
        {REG_PWR_MGMT_1, 0x00},   /* wake up */
        {REG_ACCEL_CONFIG, 0x01}, /* +-2g, high-pass 5 Hz (motion detect uses HPF data) */
        {REG_MOT_THR, 20},        /* threshold, tune this */
        {REG_MOT_DUR, 1},         /* 1 ms above threshold */
        {REG_INT_PIN_CFG, 0x20},  /* active high, push-pull, latched until INT_STATUS read */
        {REG_INT_ENABLE, 0x40},   /* motion interrupt only */
    };

    for (size_t i = 0; i < ARRAY_SIZE(cfg); i++) {
        ret = i2c_reg_write_byte_dt(&mpu, cfg[i][0], cfg[i][1]);
        if (ret) {
            printk("write 0x%02x failed: %d\n", cfg[i][0], ret);
            return ret;
        }
    }

    /* clear anything latched during setup */
    i2c_reg_read_byte_dt(&mpu, REG_INT_STATUS, &id);
    return 0;
}

int main(void)
{
    if (!i2c_is_ready_dt(&mpu) || !gpio_is_ready_dt(&mpu_int)) {
        printk("I2C or GPIO not ready\n");
        return 0;
    }

    if (mpu_init()) {
        return 0;
    }

    k_work_init(&jolt_work, jolt_work_handler);

    gpio_pin_configure_dt(&mpu_int, GPIO_INPUT);
    gpio_init_callback(&int_cb, int_isr, BIT(mpu_int.pin));
    gpio_add_callback(mpu_int.port, &int_cb);
    gpio_pin_interrupt_configure_dt(&mpu_int, GPIO_INT_EDGE_TO_ACTIVE);

    printk("MPU6050 ready, waiting for jolts\n");
    k_sleep(K_FOREVER);
    return 0;
}
