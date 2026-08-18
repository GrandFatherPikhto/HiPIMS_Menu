#include "hipims_fault.h"

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_utils.h"

#include "hipims_spi.h"
#include "hipims_storage.h"

#define FAULT_LED_PORT GPIOB
#define FAULT_LED_PIN  LL_GPIO_PIN_0

#define FAULT_POLL_DIVIDER 20 /* only actually poll every 20th tick (~10ms cadence -> ~200ms) */

static bool s_fault_a = false;
static bool s_fault_b = false;
static uint16_t s_tick_count = 0;

static void fault_led_set(bool on)
{
    if (on) { LL_GPIO_SetOutputPin(FAULT_LED_PORT, FAULT_LED_PIN); }
    else    { LL_GPIO_ResetOutputPin(FAULT_LED_PORT, FAULT_LED_PIN); }
}

void hipims_fault_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_GPIO_SetPinMode(FAULT_LED_PORT, FAULT_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(FAULT_LED_PORT, FAULT_LED_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(FAULT_LED_PORT, FAULT_LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    fault_led_set(false);
}

void hipims_fault_tick(void)
{
    s_tick_count++;
    if (s_tick_count < FAULT_POLL_DIVIDER)
    {
        return;
    }
    s_tick_count = 0;

    /* The protocol has no dedicated "ping" — re-assert REG_START at its own
     * current value purely to get a fresh status byte back. REG_START never
     * touches the fault latch (only a REG_ENABLE 0->1 edge does), so this is
     * safe to do regardless of fault state.
     *
     * The write itself can NACK (comms glitch, or — turns out — just because
     * nothing has ACKed yet during bring-up) but fault_a/fault_b are valid
     * in the status byte either way, ACK or NACK, so read them unconditionally
     * instead of only on success. */
    hipims_spi_write_reg(HIPIMS_REG_START, storage_get(HIPIMS_REG_START));

    uint8_t status = hipims_spi_last_status();
    s_fault_a = (status & HIPIMS_STATUS_FAULT_A) != 0;
    s_fault_b = (status & HIPIMS_STATUS_FAULT_B) != 0;

    fault_led_set(s_fault_a || s_fault_b);
}

bool hipims_fault_a(void) { return s_fault_a; }
bool hipims_fault_b(void) { return s_fault_b; }

bool hipims_fault_reset(void)
{
    bool ok1 = hipims_spi_write_reg(HIPIMS_REG_ENABLE, 0);
    LL_mDelay(1); /* hold at 0 so the FPGA sees a real edge, not a glitch */
    bool ok2 = hipims_spi_write_reg(HIPIMS_REG_ENABLE, 1);

    if (ok2)
    {
        storage_set(HIPIMS_REG_ENABLE, 1); /* keep the ENABLE menu item in sync with reality */
    }

    return ok1 && ok2;
}
