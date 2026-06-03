#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C address with SA0 high
#define ACCEL_I2C_ADDR 0x19

// Register addresses
#define ACCEL_REG_WHO_AM_I  0x0F
#define ACCEL_REG_CTRL1     0x20
#define ACCEL_REG_CTRL4     0x23

// WHO_AM_I expected response
#define ACCEL_WHO_AM_I_VAL  0x33

// Sample rates (ODR bits in CTRL_REG1)
enum class AccelSampleRate : uint8_t {
    HZ_1    = 0x10,
    HZ_10   = 0x20,
    HZ_25   = 0x30,
    HZ_50   = 0x40,
    HZ_100  = 0x50,
    HZ_200  = 0x60,
    HZ_400  = 0x70,
};

// Measurement ranges (FS bits in CTRL_REG4)
enum class AccelRange : uint8_t {
    G_2  = 0x00,
    G_4  = 0x10,
    G_8  = 0x20,
    G_16 = 0x30,
};

class Accelerometer {
public:
    Accelerometer(i2c_inst_t* i2c);

    // Initialise and verify communication via WHO_AM_I register.
    // Returns true if successful.
    bool init(void);

    // Configure the sample rate and measurement range.
    // Must be called after init().
    bool configure(AccelSampleRate rate, AccelRange range);

private:
    i2c_inst_t* _i2c;

    bool write_register(uint8_t reg, uint8_t value);
    bool read_register(uint8_t reg, uint8_t* value);
};