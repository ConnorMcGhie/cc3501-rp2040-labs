#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
 
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
 
#define LED_PIN 14

int main()
{
    stdio_init_all();
 
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
 
    leds_init();
    log(LogLevel::INFORMATION, "LED driver ready");

     for (;;) {
        // Light each LED one at a time
        leds_set(0, Colours::RED);
        sleep_ms(500);

        leds_set(1, Colours::GREEN);
        sleep_ms(500);

        leds_set(2, Colours::BLUE);
        sleep_ms(500);

        // Clear all LEDs
        leds_clear();
        sleep_ms(500);
     }
 
    return 0;
}