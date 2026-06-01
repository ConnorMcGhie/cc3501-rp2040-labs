#include "leds.h"

#include "hardware/pio.h"
#include "WS2812.pio.h"


LEDDriver::LEDDriver(PIO pio, uint sm) : _pio(pio), _sm(sm)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        _state[i] = Colours::OFF;
    }
    show();
}
 
void LEDDriver::set(uint8_t index, Colour colour)
{
    if (index >= NUM_LEDS) {
        return;
    }
    _state[index] = colour;
}

void LEDDriver::set_multiple(const LEDUpdate* updates, uint8_t count)
{
    for (int i = 0; i < count; i++) {
        set(updates[i].index, updates[i].colour);
    }
}
 
void LEDDriver::clear(void)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        _state[i] = Colours::OFF;
    }
}
 
void LEDDriver::show(void)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t word = ((uint32_t)_state[i].red   << 24)
                      | ((uint32_t)_state[i].green << 16)
                      | ((uint32_t)_state[i].blue  <<  8);
        pio_sm_put_blocking(_pio, _sm, word);
    }
}