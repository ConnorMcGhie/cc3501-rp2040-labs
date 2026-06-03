#include "accelerometer.h"

Accelerometer::Accelerometer(i2c_inst_t* i2c) : _i2c(i2c)
{
}

bool Accelerometer::init(void)
{
    uint8_t who_am_i = 0;
    if (!read_register(ACCEL_REG_WHO_AM_I, &who_am_i)) {
        return false;
    }
    return who_am_i == ACCEL_WHO_AM_I_VAL;
}

bool Accelerometer::write_register(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    int result = i2c_write_blocking(_i2c, ACCEL_I2C_ADDR, buf, 2, false);
    return result != PICO_ERROR_GENERIC;
}

bool Accelerometer::read_register(uint8_t reg, uint8_t* value)
{
    // On I2C, a register read is always a write of the address followed by a read.
    // The nostop=true keeps the bus held between the two for a repeated start.
    int result = i2c_write_blocking(_i2c, ACCEL_I2C_ADDR, &reg, 1, true);
    if (result == PICO_ERROR_GENERIC) {
        return false;
    }

    result = i2c_read_blocking(_i2c, ACCEL_I2C_ADDR, value, 1, false);
    return result != PICO_ERROR_GENERIC;
}