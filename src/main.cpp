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
        // Stage multiple changes, then commit them all at once
        leds_set(0, Colours::RED);
        leds_set(1, Colours::GREEN);
        leds_set(2, Colours::BLUE);
        write_to_hardware();
        sleep_ms(1000);
 
        // Stage a different pattern and commit
        leds_set(0, Colours::BLUE);
        leds_set(1, Colours::RED);
        leds_set(2, Colours::GREEN);
        write_to_hardware();
        sleep_ms(1000);
 
        // Clear everything and commit
        leds_clear();
        write_to_hardware();
        sleep_ms(500);
     }
 
    return 0;
}