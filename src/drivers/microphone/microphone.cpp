#include "microphone.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"


// Clock divisor for 44.1 kHz sample rate.
static constexpr float ADC_CLKDIV = (48'000'000.0f / 44'100.0f) - 1.0f;

void microphone_init(void)
{
    // Initialise the ADC hardware block (safe to call multiple times).
    adc_init();

    // Prepare the GPIO pin: disables digital input/output functions so the
    // ADC can use it as a high-impedance analogue input.
    adc_gpio_init(MIC_ADC_GPIO);

    // Select the ADC input channel connected to the microphone.
    adc_select_input(MIC_ADC_CHANNEL);

    // Set the sample rate divisor.
    adc_set_clkdiv(ADC_CLKDIV);

    // Configure the FIFO:
    adc_fifo_setup(
        true,   // en
        false,  // dreq_en  (no DMA)
        1,      // dreq_thresh
        false,  // err_in_fifo
        false   // byte_shift
    );
}

void microphone_read(uint16_t *buffer, size_t num_samples)
{
    // Start free-running sampling mode.
    adc_run(true);

    // Block on the FIFO until we have the requested number of samples.
    for (size_t i = 0; i < num_samples; i++) {
        buffer[i] = adc_fifo_get_blocking();
    }

    // Stop free-running sampling mode.
    adc_run(false);

    // Drain any residual samples that accumulated in the FIFO while we
    // were still stopping — keeps the FIFO clean for the next call.
    adc_fifo_drain();
}