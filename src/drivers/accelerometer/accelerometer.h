#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C address with SA0 low
#define ACCEL_I2C_ADDR 0x19

// WHO_AM_I register address and expected response
#define ACCEL_REG_WHO_AM_I  0x0F
#define ACCEL_WHO_AM_I_VAL  0x33

class Accelerometer {
public:
    Accelerometer(i2c_inst_t* i2c);

    // Initialise and verify communication via WHO_AM_I register.
    // Returns true if successful.
    bool init(void);

private:
    i2c_inst_t* _i2c;

    // Write a value to a register.
    bool write_register(uint8_t reg, uint8_t value);

    // Read a value from a register.
    bool read_register(uint8_t reg, uint8_t* value);
};