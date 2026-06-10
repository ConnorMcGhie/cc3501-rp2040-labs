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

// ALSO FOR WEEK 3: ACCELEROMETER
// Update a row of 4 LEDs based on an acceleration value in g
// LED layout per row:
//   [base+0] [base+1] [base+2] [base+3]
//   extreme  tilt-low  level   tilt-high  extreme
//
// Level:          both base+1 and base+2 lit green
// Tilt low:       both base+0 and base+1 lit green
// Tilt high:      both base+2 and base+3 lit green
// Extreme low:    only base+0 lit red
// Extreme high:   only base+3 lit red
// void update_row(LEDDriver& leds, int base, float value)
// {
//     // Clear the row
//     leds.set(base + 0, Colours::OFF);
//     leds.set(base + 1, Colours::OFF);
//     leds.set(base + 2, Colours::OFF);
//     leds.set(base + 3, Colours::OFF);
 
//     if (value >= -0.2f && value <= 0.2f) {
//         // Level
//         leds.set(base + 1, Colours::GREEN);
//         leds.set(base + 2, Colours::GREEN);
//     } else if (value > 0.2f && value <= 0.6f) {
//         // Tilting high
//         leds.set(base + 2, Colours::GREEN);
//         leds.set(base + 3, Colours::GREEN);
//     } else if (value < -0.2f && value >= -0.6f) {
//         // Tilting low
//         leds.set(base + 0, Colours::GREEN);
//         leds.set(base + 1, Colours::GREEN);
//     } else if (value > 0.6f) {
//         // Extreme high
//         leds.set(base + 3, Colours::RED);
//     } else {
//         // Extreme low
//         leds.set(base + 0, Colours::RED);
//     }
// }


int main()
{
    // WEEK 3: ACCELEROMETER
    // stdio_init_all();
 
    // // Initialise PIO for LEDs
    // uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    // ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    // LEDDriver leds(pio0, 0);
 
    // // Initialise I2C for accelerometer
    // i2c_init(i2c0, 400000);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
 
    // // Initialise and configure accelerometer
    // Accelerometer accel(i2c0);
    // if (!accel.init()) {
    //     log(LogLevel::ERROR, "Accelerometer init failed");
    //     leds.set(0, Colours::RED);
    //     leds.show();
    //     while(true);
    // }
 
    // if (!accel.configure(AccelSampleRate::HZ_100, AccelRange::G_2)) {
    //     log(LogLevel::ERROR, "Accelerometer configure failed");
    //     leds.set(0, Colours::RED);
    //     leds.show();
    //     while(true);
    // }
 
    // log(LogLevel::INFORMATION, "Accelerometer ready");
    // leds.set(0, Colours::GREEN);
    // leds.show();
 
    // for (;;) {
    //     AccelDataFloat data;
    //     if (accel.read_g(&data)) {
    //         update_row(leds, 0, data.x);   // X axis: LEDs 0-3
    //         update_row(leds, 4, data.y);   // Y axis: LEDs 4-7
    //         update_row(leds, 8, data.z);   // Z axis: LEDs 8-11
    //         leds.show();
    //     }
 
    //     sleep_ms(40);
    // }
 
    // return 0;



    // WEEK 2: LEDS
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