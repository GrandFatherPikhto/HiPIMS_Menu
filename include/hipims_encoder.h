#ifndef HIPIMS_ENCODER_H
#define HIPIMS_ENCODER_H

#include <stdbool.h>
#include <stdint.h>

void encoder_init(void);

/* Signed raw timer counts since the last call (positive = CW). Counts
 * accumulate in TIM2's hardware quadrature decoder between polls, so this
 * can safely be called at a slow, arbitrary rate instead of per edge. */
int32_t encoder_read_delta(void);

/* Debounced click (falling edge). Must be polled at a steady rate — see
 * ENCODER_BUTTON_DEBOUNCE_TICKS in hipims_encoder.c. */
bool encoder_button_clicked(void);

#endif /* HIPIMS_ENCODER_H */
