#include <stdint.h>

typedef enum {
   TCA9555_PORT_0,
   TCA9555_PORT_1
} tca9555_io_port_t;

typedef enum {
    TCA9555_IO_OUTPUT = 0,
    TCA9555_IO_INPUT
} tca9555_io_mode_t;

typedef enum {
   TCA9555_IO_RESET,
   TCA9555_IO_SET
} tca9555_io_pin_t;

#define TCA9555_PORT0_PIN0   0x01
#define TCA9555_PORT0_PIN1   0x02
#define TCA9555_PORT0_PIN2   0x04
#define TCA9555_PORT0_PIN3   0x08
#define TCA9555_PORT0_PIN4   0x10
#define TCA9555_PORT0_PIN5   0x20
#define TCA9555_PORT0_PIN6   0x40
#define TCA9555_PORT0_PIN7   0x80

#define TCA9555_PORT1_PIN0   0x01
#define TCA9555_PORT1_PIN1   0x02
#define TCA9555_PORT1_PIN2   0x04
#define TCA9555_PORT1_PIN3   0x08
#define TCA9555_PORT1_PIN4   0x10
#define TCA9555_PORT1_PIN5   0x20
#define TCA9555_PORT1_PIN6   0x40
#define TCA9555_PORT1_PIN7   0x80

void tac9555_i2c_init(void);
void tca9555_init(void);
void tca9555_set_io_pin(tca9555_io_port_t port, uint16_t pin, tca9555_io_pin_t val);
