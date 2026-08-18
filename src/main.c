#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"

#include "hipims_debug.h"
#include "hipims_fault.h"
#include "hipims_lcd1602.h"
#include "hipims_menu.h"
#include "hipims_spi.h"

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
    menu_init();

    for (;;)
    {
        menu_tick();
        hipims_fault_tick();
        LL_mDelay(10);
    }
}
