#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
 
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds/leds.h"
#include "drivers/accelerometer/accelerometer.h"
#include "main.h"


int main()
{
    stdio_init_all();
 
    // Initialise PIO for LEDs
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    LEDDriver leds(pio0, 0);
 
    // Initialise I2C for accelerometer
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
 
    // Initialise and configure accelerometer
    Accelerometer accel(i2c0);
    if (!accel.init()) {
        log(LogLevel::ERROR, "Accelerometer init failed");
        leds.set(0, Colours::RED);
        leds.show();
        while(true);
    }
 
    if (!accel.configure(AccelSampleRate::HZ_100, AccelRange::G_2)) {
        log(LogLevel::ERROR, "Accelerometer configure failed");
        leds.set(0, Colours::RED);
        leds.show();
        while(true);
    }
 
    log(LogLevel::INFORMATION, "Accelerometer ready");
    leds.set(0, Colours::GREEN);
    leds.show();
 
    for (;;) {
        sleep_ms(10);
    }
 
    return 0;

    // for (;;) {
    //     //DEMO 1: Setting LEDs one at a time, then committing changes
    //     // Stage multiple changes committing after each, with a delay in between to see changes
    //     leds.set(0, Colours::RED);
    //     leds.show();
    //     sleep_ms(500);
    //     leds.set(1, Colours::GREEN);
    //     leds.show();
    //     sleep_ms(500);
    //     leds.set(2, Colours::BLUE);
    //     leds.show();
    //     sleep_ms(500);
    //     sleep_ms(1000);
 
    //     // Stage a different pattern and commit at once
    //     leds.set(0, Colours::BLUE);
    //     leds.set(1, Colours::RED);
    //     leds.set(2, Colours::GREEN);
    //     leds.show();
    //     sleep_ms(1000);
 
    //     // Clear everything and commit
    //     leds.clear();
    //     leds.show();
    //     sleep_ms(500);

    //     // DEMO 2: Setting multiple LEDs at once, then committing changes
    //     // Create array of changes to LEDs
    //     LEDUpdate pattern[] = {
    //         {0, Colours::RED},
    //         {5, Colours::GREEN},
    //         {11, Colours::BLUE},
    //     };
        
    //     // Parse changes into set_multiple method, then commit
    //     leds.set_multiple(pattern, 3);
    //     leds.show();
    //     sleep_ms(1000);
 
    //     // Clear and commit
    //     leds.clear();
    //     leds.show();
    //     sleep_ms(500);

    //     // DEMO 3: Querying the colour of an LED (with visual feedback
    //     // by setting to the same colour)
    //     leds.set(0, Colours::RED);
    //     Colour c = leds.get(0);
    //     leds.set(1, c);
    //     leds.show();
    //     sleep_ms(1000);

    //     leds.clear();
    //     sleep_ms(1000);}
    // }

}