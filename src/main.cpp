#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds/leds.h"
#include "drivers/accelerometer/accelerometer.h"
#include "board.h"
#include "drivers/microphone/microphone.h"

// Demo modes, cycled in this order by the mode switch.
enum class Mode {
    LEDS = 0,
    ACCELEROMETER,
    MICROPHONE,
    MODE_COUNT  // not a real mode - used to wrap the cycle
};

// Minimum time between accepted mode switches. Acts as both a cooldown
// (so a deliberate press can't immediately trigger a second switch) and
// the debounce (contact bounce on a single press happens within
// microseconds-to-low-milliseconds, far inside this window).
static constexpr uint32_t SWITCH_COOLDOWN_MS = 1000;

// Set by the GPIO interrupt handler when a press is accepted. Cleared by
// mode_change_requested(). Marked volatile since it's written from an ISR
// and read from regular code.
static volatile bool mode_switch_flag = false;

// GPIO interrupt callback, fires on the rising edge of MODE_SWITCH_PIN
// (idle = LOW, pressed = HIGH: 3V3 -> SW -> GPIO15, 100k pulldown to
// ground). Kept short, as is good practice for an ISR - just a timestamp
// check and a flag set.
void mode_switch_isr(uint gpio, uint32_t events)
{
    static uint32_t last_accepted_ms = 0;

    if (gpio != MODE_SWITCH_PIN) {
        return;
    }

    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_accepted_ms >= SWITCH_COOLDOWN_MS) {
        last_accepted_ms = now;
        mode_switch_flag = true;
    }
    // Otherwise: within the cooldown window (bounce or a too-soon repeat
    // press) - ignore it.
}

// Returns true once if a mode switch has been requested since the last
// call, and clears the request. Safe to call frequently from anywhere -
// this is how demo loops check for a pending switch.
bool mode_change_requested(void)
{
    if (mode_switch_flag) {
        mode_switch_flag = false;
        return true;
    }
    return false;
}

// Sleeps for the requested duration, but returns early (without sleeping
// out the rest of the duration) if a mode switch is requested mid-sleep.
// Used in place of sleep_ms() anywhere a demo would otherwise block for
// long enough to make the button feel unresponsive.
void interruptible_sleep_ms(uint32_t ms)
{
    static constexpr uint32_t SLICE_MS = 10;

    while (ms > 0) {
        uint32_t slice = (ms < SLICE_MS) ? ms : SLICE_MS;
        sleep_ms(slice);
        ms -= slice;

        if (mode_switch_flag) {
            // Leave the flag set - the caller's own mode_change_requested()
            // check (in its while-loop condition) will pick it up.
            return;
        }
    }
}

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
void update_row(LEDDriver& leds, int base, float value)
{
    // Clear the row
    leds.set(base + 0, Colours::OFF);
    leds.set(base + 1, Colours::OFF);
    leds.set(base + 2, Colours::OFF);
    leds.set(base + 3, Colours::OFF);

    if (value >= -0.2f && value <= 0.2f) {
        // Level
        leds.set(base + 1, Colours::GREEN);
        leds.set(base + 2, Colours::GREEN);
    } else if (value > 0.2f && value <= 0.6f) {
        // Tilting high
        leds.set(base + 2, Colours::GREEN);
        leds.set(base + 3, Colours::GREEN);
    } else if (value < -0.2f && value >= -0.6f) {
        // Tilting low
        leds.set(base + 0, Colours::GREEN);
        leds.set(base + 1, Colours::GREEN);
    } else if (value > 0.6f) {
        // Extreme high
        leds.set(base + 3, Colours::RED);
    } else {
        // Extreme low
        leds.set(base + 0, Colours::RED);
    }
}

void update_leds_fft(LEDDriver& leds, q15_t *mag)
{
    // Logarithmically spaced bin boundaries from lab sheet:
    // ceil(logspace(log10(5), log10(512), 13))
    static const int bin_boundaries[13] = {
        6, 8, 11, 16, 24, 35, 51, 75, 110, 161, 237, 349, 400
    };

    leds.clear();

    for (int led = 0; led < NUM_LEDS; led++) {
        int bin_start = bin_boundaries[led];
        int bin_end   = bin_boundaries[led + 1];

        // Sum energy across all bins in this LED's frequency band.
        // Use int32_t to avoid overflow when summing multiple q15_t values.
        int32_t energy = 0;
        for (int b = bin_start; b < bin_end; b++) {
            energy += mag[b];
        }

        // Normalise energy to 0-255 brightness.
        // Threshold filters out noise floor; scale sets sensitivity.
        // These values will need tuning based on your environment.
        static constexpr int32_t THRESHOLD = 500;
        static constexpr int32_t SCALE     = 2000;

        if (energy < THRESHOLD) {
            leds.set(led, Colours::OFF);
        } else {
            // Clamp to 0-255
            int32_t brightness = ((energy - THRESHOLD) * 255) / SCALE;
            if (brightness > 255) brightness = 255;

            // Green at low energy, red at high energy
            Colour c;
            c.red   = (uint8_t)(brightness);
            c.green = (uint8_t)(255 - brightness);
            c.blue  = 0;
            leds.set(led, c);
        }
    }

    leds.show();
}

// WEEK 2: LEDS demo.
// Runs until the mode switch is pressed, then returns so main() can move
// on to the next mode. Uses interruptible_sleep_ms() throughout so a
// press is caught within one ~10ms slice, not just between steps.
void run_leds_demo(LEDDriver& leds)
{
    while (!mode_change_requested()) {
        // DEMO 1: Setting LEDs one at a time, then committing changes
        // Stage multiple changes committing after each, with a delay in between to see changes
        leds.set(0, Colours::RED);
        leds.show();
        interruptible_sleep_ms(500);
        if (mode_change_requested()) return;

        leds.set(1, Colours::GREEN);
        leds.show();
        interruptible_sleep_ms(500);
        if (mode_change_requested()) return;

        leds.set(2, Colours::BLUE);
        leds.show();
        interruptible_sleep_ms(500);
        interruptible_sleep_ms(1000);
        if (mode_change_requested()) return;

        // Stage a different pattern and commit at once
        leds.set(0, Colours::BLUE);
        leds.set(1, Colours::RED);
        leds.set(2, Colours::GREEN);
        leds.show();
        interruptible_sleep_ms(1000);
        if (mode_change_requested()) return;

        // Clear everything and commit
        leds.clear();
        leds.show();
        interruptible_sleep_ms(500);
        if (mode_change_requested()) return;

        // DEMO 2: Setting multiple LEDs at once, then committing changes
        // Create array of changes to LEDs
        LEDUpdate pattern[] = {
            {0, Colours::RED},
            {5, Colours::GREEN},
            {11, Colours::BLUE},
        };

        // Parse changes into set_multiple method, then commit
        leds.set_multiple(pattern, 3);
        leds.show();
        interruptible_sleep_ms(1000);
        if (mode_change_requested()) return;

        // Clear and commit
        leds.clear();
        leds.show();
        interruptible_sleep_ms(500);
        if (mode_change_requested()) return;

        // DEMO 3: Querying the colour of an LED (with visual feedback
        // by setting to the same colour)
        leds.set(0, Colours::RED);
        Colour c = leds.get(0);
        leds.set(1, c);
        leds.show();
        interruptible_sleep_ms(1000);
        if (mode_change_requested()) return;

        leds.clear();
        leds.show();
        interruptible_sleep_ms(1000);
    }
}

// WEEK 3: ACCELEROMETER demo.
// Runs until the mode switch is pressed, then returns.
void run_accelerometer_demo(LEDDriver& leds, i2c_inst_t* i2c)
{
    // Initialise I2C for accelerometer
    i2c_init(i2c, 400000);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);

    // Initialise and configure accelerometer
    Accelerometer accel(i2c);
    if (!accel.init()) {
        log(LogLevel::ERROR, "Accelerometer init failed");
        leds.set(0, Colours::RED);
        leds.show();
        return;
    }

    if (!accel.configure(AccelSampleRate::HZ_100, AccelRange::G_2)) {
        log(LogLevel::ERROR, "Accelerometer configure failed");
        leds.set(0, Colours::RED);
        leds.show();
        return;
    }

    log(LogLevel::INFORMATION, "Accelerometer ready");
    leds.clear();
    leds.set(0, Colours::GREEN);
    leds.show();
    interruptible_sleep_ms(200);

    while (!mode_change_requested()) {
        AccelDataFloat data;
        if (accel.read_g(&data)) {
            update_row(leds, 0, data.x);   // X axis: LEDs 0-3
            update_row(leds, 4, data.y);   // Y axis: LEDs 4-7
            update_row(leds, 8, data.z);   // Z axis: LEDs 8-11
            leds.show();
        }

        interruptible_sleep_ms(40);
    }
}

// WEEK 4: MICROPHONE demo.
// Runs until the mode switch is pressed, then returns.
void run_microphone_demo(LEDDriver& leds)
{
    static bool mic_initialised = false;
    if (!mic_initialised) {
        microphone_init();
        mic_initialised = true;
    }

    uint16_t raw[MIC_SAMPLE_COUNT];
    int16_t  processed[MIC_SAMPLE_COUNT];
    int16_t  fft_output[MIC_FFT_OUTPUT_SIZE];
    q15_t    mag[MIC_MAG_OUTPUT_SIZE];

    while (!mode_change_requested()) {
        microphone_read(raw, MIC_SAMPLE_COUNT);
        microphone_process(raw, processed, MIC_SAMPLE_COUNT);
        microphone_apply_window(processed, MIC_SAMPLE_COUNT);
        microphone_fft(processed, fft_output);
        microphone_magnitude_squared(fft_output, mag);
        update_leds_fft(leds, mag);
    }
}

int main()
{
    stdio_init_all();

    // Initialise PIO for LEDs
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    LEDDriver leds(pio0, 0);

    // Initialise the mode switch input and arm a rising-edge interrupt.
    // Idle = LOW, pressed = HIGH (3V3 -> SW -> GPIO15, 100k pulldown to ground).
    gpio_init(MODE_SWITCH_PIN);
    gpio_set_dir(MODE_SWITCH_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(MODE_SWITCH_PIN, GPIO_IRQ_EDGE_RISE,
                                        true, &mode_switch_isr);

    Mode mode = Mode::LEDS;

    for (;;) {
        leds.clear();
        leds.show();

        switch (mode) {
            case Mode::LEDS:
                run_leds_demo(leds);
                break;
            case Mode::ACCELEROMETER:
                run_accelerometer_demo(leds, i2c0);
                break;
            case Mode::MICROPHONE:
                run_microphone_demo(leds);
                break;
            default:
                break;
        }

        // Advance to the next mode, wrapping back to LEDS after MICROPHONE.
        mode = static_cast<Mode>((static_cast<int>(mode) + 1) % static_cast<int>(Mode::MODE_COUNT));
    }

    return 0;
}