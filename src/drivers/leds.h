#include <stdint.h>

#define NUM_LEDS 12

struct Colour {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

namespace Colours {
    constexpr Colour OFF   = {0,   0,   0};
    constexpr Colour RED   = {255, 0,   0};
    constexpr Colour GREEN = {0,   255, 0};
    constexpr Colour BLUE  = {0,   0,   255};
    constexpr Colour WHITE = {255, 255, 255};
}

// Initialise the LED driver. Must be called once before any other led_* functions.
void leds_init(void);

// Set a single LED to a colour and immediately update the hardware.
void leds_set(uint8_t index, Colour colour);

// Turn all LEDs off and immediately update the hardware.
void leds_clear(void);

// Write the current led_state array to the hardware with blocking.
void write_to_hardware(void);