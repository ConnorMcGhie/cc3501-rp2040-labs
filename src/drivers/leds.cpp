#include "leds.h"

#include "hardware/pio.h"
#include "WS2812.pio.h"

static Colour led_state[NUM_LEDS];

// Write the current led_state array to the hardware with blocking.
static void write_to_hardware(void)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        // WS2812 expects GRB order, packed into the top 24 bits
        uint32_t word = ((uint32_t)led_state[i].red << 24)
                      | ((uint32_t)led_state[i].green   << 16)
                      | ((uint32_t)led_state[i].blue  <<  8);
        pio_sm_put_blocking(pio0, 0, word);
    }
}

void leds_init(void)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        led_state[i] = Colours::OFF;
    }
    write_to_hardware();
}

void leds_set(uint8_t index, Colour colour)
{
    if (index >= NUM_LEDS) {
        return;
    }
    led_state[index] = colour;
    write_to_hardware();
}
 
void leds_clear(void)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        led_state[i] = Colours::OFF;
    }
    write_to_hardware();
}