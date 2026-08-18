#include "hipims_debug.h"

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_usart.h"

/* USART1 default pins, no AFIO remap. TX only — RX pin is wired but this
 * console never reads anything back. */
#define DEBUG_USART   USART1
#define DEBUG_PORT    GPIOA
#define DEBUG_TX_PIN  LL_GPIO_PIN_9
#define DEBUG_RX_PIN  LL_GPIO_PIN_10

void debug_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_AFIO | LL_APB2_GRP1_PERIPH_USART1);

    LL_GPIO_SetPinMode(DEBUG_PORT, DEBUG_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(DEBUG_PORT, DEBUG_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(DEBUG_PORT, DEBUG_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(DEBUG_PORT, DEBUG_RX_PIN, LL_GPIO_MODE_FLOATING);

    LL_USART_InitTypeDef init;
    LL_USART_StructInit(&init);
    init.BaudRate           = 115200;
    init.DataWidth          = LL_USART_DATAWIDTH_8B;
    init.StopBits           = LL_USART_STOPBITS_1;
    init.Parity             = LL_USART_PARITY_NONE;
    init.TransferDirection  = LL_USART_DIRECTION_TX;
    init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    init.OverSampling       = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(DEBUG_USART, &init);

    LL_USART_ConfigAsyncMode(DEBUG_USART);
    LL_USART_Enable(DEBUG_USART);
}

static void debug_putc(char c)
{
    while (!LL_USART_IsActiveFlag_TXE(DEBUG_USART)) {}
    LL_USART_TransmitData8(DEBUG_USART, (uint8_t)c);
}

void debug_str(const char *s)
{
    while (*s)
    {
        debug_putc(*s++);
    }
}

void debug_hex8(uint8_t v)
{
    static const char hex[] = "0123456789ABCDEF";
    debug_putc(hex[v >> 4]);
    debug_putc(hex[v & 0x0F]);
}

void debug_newline(void)
{
    debug_putc('\r');
    debug_putc('\n');
}
