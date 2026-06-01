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

 
    return 0;
}