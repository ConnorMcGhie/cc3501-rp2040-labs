#pragma once
 
#include <stdint.h>
#include "hardware/pio.h"

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

// Array of LED indeces and colours for setting
struct LEDUpdate {
    uint8_t index;
    Colour colour;
};

class LEDDriver {
public:
    LEDDriver(PIO pio, uint sm);
 
    // Stage a colour change for a single LED (0-indexed). Does not update the hardware.
    void set(uint8_t index, Colour colour);

    // Stage colour changes for multiple LEDs at once. Does not update the hardware.
    void set_multiple(const LEDUpdate* updates, uint8_t count);
 
    // Stage all LEDs to be turned off. Does not update the hardware.
    void clear(void);
 
    // Commit all staged changes to the hardware.
    void show(void);

    // Get the current staged colour of an LED (0-indexed).
    Colour get(uint8_t index) const;

    // Returns true if staged values have not yet been committed with show().
    bool is_dirty(void) const;
 
private:
    PIO   _pio;
    uint  _sm;
    Colour _state[NUM_LEDS];
    bool   _dirty;
};