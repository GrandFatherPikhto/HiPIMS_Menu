#ifndef HIPIMS_ENCODER_H
#define HIPIMS_ENCODER_H

#include <stdbool.h>
#include <stdint.h>

/* Raw TIM2 counts per encoder detent (4 quadrature edges per click in X4
 * mode) — confirmed against the physical encoder: one click = one step.
 * Lives here, next to the driver that owns the X4-decode assumption, so
 * main.c's raw-counts -> steps division stays in sync with the hardware. */
#define ENCODER_COUNTS_PER_STEP 4

typedef enum
{
    ENCODER_BUTTON_NONE = 0,
    ENCODER_BUTTON_CLICK,      /* short press-and-release: enter/edit/confirm */
    ENCODER_BUTTON_LONG_PRESS, /* held past the threshold: back/up a level */
} encoder_button_event_t;

void encoder_init(void);

/* Signed raw timer counts since the last call (positive = CW). Counts
 * accumulate in TIM2's hardware quadrature decoder between polls, so this
 * can safely be called at a slow, arbitrary rate instead of per edge. */
int32_t encoder_read_delta(void);

/* Debounced button event. Must be polled at a steady rate — see
 * BUTTON_DEBOUNCE_TICKS/BUTTON_LONG_PRESS_TICKS in hipims_encoder.c. A
 * CLICK fires on release, provided the hold wasn't already long enough to
 * fire LONG_PRESS while still held (which fires exactly once, at the
 * threshold, without waiting for release). */
encoder_button_event_t encoder_button_event(void);

#endif /* HIPIMS_ENCODER_H */
