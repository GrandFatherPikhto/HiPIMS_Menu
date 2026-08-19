#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"

#include "menu.h"

#include "hipims_debug.h"
#include "hipims_encoder.h"
#include "hipims_fault.h"
#include "hipims_lcd1602.h"
#include "hipims_menu_glue.h"
#include "hipims_spi.h"
#include "hipims_storage.h"

/* Raw TIM2 counts per encoder detent (4 quadrature edges per click in X4
 * mode) — confirmed against the physical encoder: one click = one step. */
#define ENCODER_COUNTS_PER_STEP 4

/* HSE 8MHz crystal x9 PLL -> 72MHz SYSCLK/HCLK, APB1=36MHz, APB2=72MHz. */
static void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {}

    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1) {}

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1) {}

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

    LL_SetSystemCoreClock(72000000UL);
    LL_Init1msTick(SystemCoreClock);
}

int main(void)
{
    SystemClock_Config();
    debug_init();
    debug_str("HIPIMS boot\r\n");
    hipims_spi_init();
    lcd1602_init();
    hipims_fault_init();
    encoder_init();
    storage_init();

    menu_init();                          /* generated: src/menu, in-RAM only, defaults to 0 */
    hipims_menu_glue_load_from_storage(); /* pull the real persisted values in before first draw */
    hipims_menu_glue_resend_all();        /* and push them to the FPGA, which forgets everything on reset */
    menu_set_dirty();                     /* force the first draw — ctx starts clean */

    static int32_t s_encoder_accum = 0;

    for (;;)
    {
        s_encoder_accum += encoder_read_delta();
        int32_t steps = s_encoder_accum / ENCODER_COUNTS_PER_STEP;
        s_encoder_accum -= steps * ENCODER_COUNTS_PER_STEP;
        if (steps != 0)
        {
            if (steps > 127) { steps = 127; }
            if (steps < -127) { steps = -127; }
            menu_position((int8_t)steps);
        }

        switch (encoder_button_event())
        {
        case ENCODER_BUTTON_CLICK:      menu_enter(); break;
        case ENCODER_BUTTON_LONG_PRESS: menu_back();  break;
        default: break;
        }

        hipims_menu_glue_draw();
        hipims_fault_tick();
        hipims_menu_glue_tick();
        LL_mDelay(10);
    }
}
