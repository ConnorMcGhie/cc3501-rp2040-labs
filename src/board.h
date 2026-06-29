#pragma once

// Board-level pin definitions for the RP2040.
// IC-specific values (register addresses, I2C device addresses, etc.) stay
// in their respective driver headers - only RP2040 pin/peripheral
// assignments belong here.

// LEDs (WS2812 via PIO)
#define LED_PIN 14

// Accelerometer (I2C)
#define I2C_SCL 17
#define I2C_SDA 16

// Microphone (ADC)
#define MIC_ADC_GPIO    26
#define MIC_ADC_CHANNEL  0

// Mode switch
// C318884 tactile button: 3V3 -> SW -> GPIO15, with a 100k pulldown to
// ground and a decoupling cap on the GPIO node. Idle = LOW, pressed = HIGH.
#define MODE_SWITCH_PIN 15