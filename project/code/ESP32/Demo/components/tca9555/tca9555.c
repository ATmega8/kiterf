#include "tca9555.h"
#include "driver/i2c.h"

#define TCA9555_SCL_PIN 25
#define TCA9555_SDA_PIN 26

#define I2C_MASTER_NUM     I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */


#define TCA9555_ADDRESS 0x20

#define TCA9555_INPUT0_REG  0x00
#define TCA9555_INPUT1_REG  0x01
#define TCA9555_OUTPUT0_REG 0x02
#define TCA9555_OUTPUT1_REG 0x03
#define TCA9555_PI0_REG     0x04
#define TCA9555_PI1_REG     0x05
#define TCA9555_CONFIG0_REG 0x06
#define TCA9555_CONFIG1_REG 0x07

static int tca9555_write_register(uint8_t reg, uint8_t* pdata, int  len)
{
    uint8_t tmp = reg;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCA9555_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, &tmp, 1, ACK_CHECK_EN);
    i2c_master_write(cmd, pdata, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static int tca9555_read_register(uint8_t reg, uint8_t* pdata, int len)
{
    if (len == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCA9555_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCA9555_ADDRESS << 1) | READ_BIT, ACK_CHECK_EN);
    if (len > 1) {
        i2c_master_read(cmd, pdata, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, pdata+ len - 1, NACK_VAL);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void tac9555_i2c_init(void)
{
    /* i2c init */
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = TCA9555_SDA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = TCA9555_SCL_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    esp_err_t err = i2c_driver_install(i2c_master_port, conf.mode,0, 0, 0);
    ESP_ERROR_CHECK(err);
}

void tca9555_init(void)
{
    /* set io to output mode */
    uint8_t data[2] = {0x00, 0x00};
    tca9555_write_register(TCA9555_CONFIG0_REG, data, 2);
    /* reset all io */
    tca9555_write_register(TCA9555_OUTPUT0_REG, data, 2);
}

void tca9555_set_io_mode(tca9555_io_port_t port, uint8_t pin, tca9555_io_mode_t mode)
{
    uint8_t reg = TCA9555_CONFIG0_REG;

    switch(port) {
        case TCA9555_PORT_0:
            reg = TCA9555_CONFIG0_REG;
            break;

        case TCA9555_PORT_1:
            reg = TCA9555_CONFIG1_REG;
            break;

        default:
            break;
    }

    /* read current mode */
    uint8_t data;
    tca9555_read_register(reg, &data, 1);

    if(mode) {
        /* input mode */
        data |= pin;
    } else {
        /* output mode */
        data &= ~(pin);
    }

    tca9555_write_register(reg, &data, 1);
}

void tca9555_set_io_pin(tca9555_io_port_t port, uint16_t pin, tca9555_io_pin_t val)
{
    uint8_t reg = TCA9555_OUTPUT0_REG;

    switch(port) {
        case TCA9555_PORT_0:
            reg = TCA9555_OUTPUT0_REG;
            break;

        case TCA9555_PORT_1:
            reg = TCA9555_OUTPUT1_REG;
            break;

        default:
            break;
    }

    /* read current mode */
    uint8_t data;
    tca9555_read_register(reg, &data, 1);

    if(val) {
        /* io set*/
        data |=  pin;
    } else {
        /* io reset */
        data &= ~pin;
    }

    tca9555_write_register(reg, &data, 1);
}