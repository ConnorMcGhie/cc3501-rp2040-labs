#include "microphone.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"


// Clock divisor for 44.1 kHz sample rate.
static constexpr float ADC_CLKDIV = (48'000'000.0f / 44'100.0f) - 1.0f;

// Left shift applied after DC removal. // Using 5 gives extra gain to better utilise the Q15 range (minimum is 3 with 12-bit ADC).
static constexpr int Q15_SHIFT = 5;

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

void microphone_process(const uint16_t *raw, int16_t *out, size_t num_samples)
{
    // (Step 2) Calculate DC bias as integer average of raw samples.
    // int32_t is safe here: 1024 samples * max 12-bit value (4095)
    // = 4,190,208 which is well within int32_t range.
    int32_t sum = 0;
    for (size_t i = 0; i < num_samples; i++) {
        sum += raw[i];
    }
    int32_t dc_bias = sum / (int32_t)num_samples;

    // (Step 3) Subtract DC bias and shift into Q15 range.
    for (size_t i = 0; i < num_samples; i++) {
        int32_t sample = (int32_t)raw[i] - dc_bias;
        out[i] = (int16_t)(sample << Q15_SHIFT);
    }
}