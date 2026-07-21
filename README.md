# Introduction

This repository contains my completed tasks lab tasks for cc3501 Lab 2 (LED Display), Lab 3 (Digital Spirit Level) and Lab 4 (Audio Analysis).

These programs are designed for an raspberry pi pico development PCB which features 12 WS2182D LEDs, an RP2040 IC, S08OB381-026 Microphone IC, 
LIS3DHTR Accelerometer IC, a TS-1187A-B-A-B Tactile Switch and other important components such as a flash memory, USB Micro B socket, 
voltage regular, 12 MHz cryrstal and an Op-amp.

## Demos and functionality

The purpose of main.cpp is to provide visual demos for each driver of interest using the LEDs (LEDs, accelerometer, mic). The three demos 
can be cycled through by pressing the tactile switch (wired to GPIO 15) which triggers an interrupt (`mode_switch_isr`) on the rising edge.
Each demo loop uses `interruptible_sleep_ms()` which just slices `sleep_ms()` into 10ms intervals to make the interface more responsive.

# Demo 1: LED display (Lab 2)

The LED driver wraps a WS2812 PIO state machine for 12 addressable LEDs. The demo creates an LEDDriver object defined by the class in
`leds.h` and calls `run_leds_demo()` which is a function that showcases the LEDDriver methods individually in an infite loop:

1. Stage multiple changes (`leds.set()`) committing after each (`leds.show()`), with a delay in between to see changes. 
Stage a different pattern and commit at once.

2. Clear everything (`leds.clear`) and commit.

3. Set multiple LEDs at once (`leds.set_multiple`), then commit changes (utilising a `_dirty` flag to track whether staged changes 
haven't been committed).

4. Querying the colour of an LED (`leds.get`) and setting another LED to the same colour.

# Demo 2: Digital Spirit Level (Lab 3)

This demo utilises the accelerometer drivers which communicate through I2C protocol defining `Accelerometer` as a class object. The class features
methods to read/write raw register data and configure things like `AccelSampleRate` `AccelRange` and defines `Sensitivty` 
(based on information from the datasheet), which is used to translate relative to the force of gravity (`read_g`).

The demo in main creates an accelerometer object and configures it with the default gravity range (+- 2g). `run_accelerometer_demo` polls at 40ms
intervals and feeds the X/Y/Z into `update_row(), which lights a 4-LED row per axis with a middle pair of green LEDs indicating a level axis. 
The green LED pair will shift along the LED row corresponding with its respective axis tilt a singular red LED at one side indicating extreme 
tilt pas +-0.6g.

# Demo 3: Audio Analysis (Lab 4)

This task involves using the microphone drivers which samples audio through the RP2040's onboard ADC at ~44.1 kHz into a 1024-sample buffer, then removes 
the DC offset and shifts the values into Q15 fixed-point format. It then applies a Hanning window (using matlab-generated coefficients) and runs a real 
FFT using CMSIS-DSP's arm_rfft_q15, before computing the magnitude-squared.

The demo initialises the microphone and creates an LEDDriver object. It then runs an infinite loop which runs the procedure above using functions
from the microphone driver (`microphone_read` to capture raw 12-bit samples, `microphone_process()` to remove DC bias, `microphone_apply_window()`
apply Hanning window, `micrphone_fft()` runs FFT and `micrphone_magnitude_squared()` rescales output), and then runs `update_leds_fft()` which
groups the 513 FFT bins into 12 log-spaced bands and sums energy per band, mapped to a green -> red brightness gradient per LED.
