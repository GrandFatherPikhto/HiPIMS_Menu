#include "hipims_encoder.h"

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_tim.h"

/* EC11: A/B -> TIM2 hardware quadrature decoder (PA0/PA1, no remap), button
 * -> plain GPIO (PA2). See techdocs/notes/pinout.md. */
#define ENCODER_PORT       GPIOA
#define ENCODER_A_PIN      LL_GPIO_PIN_0
#define ENCODER_B_PIN      LL_GPIO_PIN_1
#define BUTTON_PIN         LL_GPIO_PIN_2

#define BUTTON_DEBOUNCE_TICKS 3 /* consecutive stable polls before accepting a new state */

static void encoder_gpio_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    LL_GPIO_SetPinMode(ENCODER_PORT, ENCODER_A_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(ENCODER_PORT, ENCODER_A_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(ENCODER_PORT, ENCODER_B_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(ENCODER_PORT, ENCODER_B_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(ENCODER_PORT, BUTTON_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(ENCODER_PORT, BUTTON_PIN, LL_GPIO_PULL_UP);
}

void encoder_init(void)
{
    encoder_gpio_init();

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    LL_TIM_ENCODER_InitTypeDef enc_init;
    LL_TIM_ENCODER_StructInit(&enc_init);
    enc_init.EncoderMode    = LL_TIM_ENCODERMODE_X4_TI12; /* count every edge on both channels */
    enc_init.IC1Polarity    = LL_TIM_IC_POLARITY_RISING;
    enc_init.IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    enc_init.IC1Prescaler   = LL_TIM_ICPSC_DIV1;
    enc_init.IC1Filter      = LL_TIM_IC_FILTER_FDIV8_N8; /* mechanical-contact noise filtering */
    enc_init.IC2Polarity    = LL_TIM_IC_POLARITY_RISING;
    enc_init.IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    enc_init.IC2Prescaler   = LL_TIM_ICPSC_DIV1;
    enc_init.IC2Filter      = LL_TIM_IC_FILTER_FDIV8_N8;
    LL_TIM_ENCODER_Init(TIM2, &enc_init);

    LL_TIM_SetAutoReload(TIM2, 0xFFFF);
    LL_TIM_EnableCounter(TIM2);
}

int32_t encoder_read_delta(void)
{
    static uint16_t last_cnt = 0;

    uint16_t cnt = (uint16_t)LL_TIM_GetCounter(TIM2);
    int16_t delta = (int16_t)(cnt - last_cnt); /* wraps correctly, 16-bit counter */
    last_cnt = cnt;

    return delta;
}

bool encoder_button_clicked(void)
{
    static uint8_t stable_state = 1; /* pull-up: released = 1 */
    static uint8_t debounce_count = 0;

    uint8_t raw = LL_GPIO_IsInputPinSet(ENCODER_PORT, BUTTON_PIN) ? 1u : 0u;

    if (raw == stable_state)
    {
        debounce_count = 0;
        return false;
    }

    if (++debounce_count < BUTTON_DEBOUNCE_TICKS)
    {
        return false;
    }

    debounce_count = 0;
    stable_state = raw;
    return (stable_state == 0); /* pressed = 0 */
}
