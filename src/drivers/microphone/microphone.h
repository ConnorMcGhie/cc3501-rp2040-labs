#pragma once

#include <stdint.h>
#include <stddef.h>

#define MIC_ADC_GPIO    26
#define MIC_ADC_CHANNEL  0
#define MIC_SAMPLE_COUNT 1024

/**
 * @brief Initialise the ADC peripheral and configure it for microphone capture.
 * Configures:
 *   - ADC clock divisor for 44.1 kHz sample rate
 *   - FIFO enabled, no DMA, 12-bit samples (no byte-shift)
 *   - Input muxed to MIC_ADC_CHANNEL
 *
 * Free-running mode is NOT enabled here; it is started and stopped
 * inside microphone_read() on each call.
 */
void microphone_init(void);

/**
 * @brief Capture a block of samples from the microphone.
 *
 * Starts free-running ADC mode, reads exactly `num_samples` 12-bit
 * values into `buffer` via the blocking FIFO read, then stops
 * free-running mode and drains the FIFO.
 *
 * @param buffer      Caller-allocated array of at least num_samples uint16_t.
 * @param num_samples Number of samples to capture (e.g. 1024).
 */
void microphone_read(uint16_t *buffer, size_t num_samples);

// Process raw ADC samples into a DC-removed, Q15 fixed-point buffer.
// Subtracts the DC bias (mean) and left-shifts to fill Q15 range.
void microphone_process(const uint16_t *raw, int16_t *out, size_t num_samples);
