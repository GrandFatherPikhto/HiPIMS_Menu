#include "hipims_spi.h"

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"

#include "hipims_debug.h"

/* SPI1 default pin mapping, no AFIO remap. Protocol v2: full duplex, FPGA
 * drives MISO with a status/ACK byte. */
#define HIPIMS_SPI          SPI1
#define HIPIMS_CS_PORT      GPIOA
#define HIPIMS_CS_PIN       LL_GPIO_PIN_4
#define HIPIMS_SCK_PIN      LL_GPIO_PIN_5
#define HIPIMS_MISO_PIN     LL_GPIO_PIN_6
#define HIPIMS_MOSI_PIN     LL_GPIO_PIN_7

#define HIPIMS_SPI_MAX_RETRIES 10

/* UART diagnostics policy. The happy path — including the periodic resend
 * of all HIPIMS_NUM_REGS registers every ~1.5s and the ~200ms fault poll —
 * must NOT print anything: each ~50-char line costs ~4-5ms of blocking TXE
 * polling at 115200, and the full resend would stall the 10ms main loop for
 * ~150ms+. So by default only failures (NACKed attempts) are dumped. Define
 * HIPIMS_DEBUG_SPI_FULL to log every attempt (ACK and NACK) for bring-up. */
#ifndef HIPIMS_DEBUG_SPI_FULL
#define HIPIMS_DEBUG_SPI_FULL 0
#endif

static uint8_t s_last_status = 0;

/* Diagnostics counters — surfaced to the UI via the ERRORS menu item so a
 * silently NACKed write is visible to the operator instead of vanishing. */
static uint32_t s_nack_count = 0;    /* NACKed individual attempts */
static uint32_t s_tx_failures = 0;   /* whole writes that never ACKed */

static void hipims_spi_gpio_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_AFIO);

    LL_GPIO_SetPinMode(HIPIMS_CS_PORT, HIPIMS_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(HIPIMS_CS_PORT, HIPIMS_SCK_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(HIPIMS_CS_PORT, HIPIMS_SCK_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(HIPIMS_CS_PORT, HIPIMS_MOSI_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(HIPIMS_CS_PORT, HIPIMS_MOSI_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(HIPIMS_CS_PORT, HIPIMS_MOSI_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    /* Master's MISO is an input, not an AF output — the FPGA drives it. */
    LL_GPIO_SetPinMode(HIPIMS_CS_PORT, HIPIMS_MISO_PIN, LL_GPIO_MODE_FLOATING);

    /* CS_N is driven directly by us (SPI peripheral NSS is software-managed),
     * since a write can span several 5-byte register writes under one
     * CS_N low. Idle high. */
    LL_GPIO_SetPinMode(HIPIMS_CS_PORT, HIPIMS_CS_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(HIPIMS_CS_PORT, HIPIMS_CS_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(HIPIMS_CS_PORT, HIPIMS_CS_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetOutputPin(HIPIMS_CS_PORT, HIPIMS_CS_PIN);
}

void hipims_spi_init(void)
{
    hipims_spi_gpio_init();

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_InitTypeDef init;
    LL_SPI_StructInit(&init);
    init.TransferDirection = LL_SPI_FULL_DUPLEX;
    init.Mode              = LL_SPI_MODE_MASTER;
    init.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
    init.ClockPolarity     = LL_SPI_POLARITY_LOW;   /* Mode 0 */
    init.ClockPhase        = LL_SPI_PHASE_1EDGE;    /* Mode 0 */
    init.NSS                = LL_SPI_NSS_SOFT;
    init.BaudRate            = LL_SPI_BAUDRATEPRESCALER_DIV64; /* APB2=72MHz -> ~1.1MHz SCK */
    init.BitOrder            = LL_SPI_MSB_FIRST;
    init.CRCCalculation      = LL_SPI_CRCCALCULATION_DISABLE;
    LL_SPI_Init(HIPIMS_SPI, &init);

    LL_SPI_Enable(HIPIMS_SPI);
}

static uint8_t hipims_spi_transfer_byte(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(HIPIMS_SPI)) {}
    LL_SPI_TransmitData8(HIPIMS_SPI, data);
    while (!LL_SPI_IsActiveFlag_RXNE(HIPIMS_SPI)) {}
    return LL_SPI_ReceiveData8(HIPIMS_SPI);
}

/* CRC-8, poly 0x07, init 0x00, no input/output reflection, no final XOR
 * (the SMBus PEC variant) — must match rtl/spi_slave_regs.vhd bit for bit. */
static uint8_t hipims_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
    }

    return crc;
}

/* The FPGA's glitch filter on CS_N/SCK/MOSI rejects anything shorter than
 * 4 of its clocks (80ns @ 50MHz) as noise. Two back-to-back GPIO BSRR writes
 * at 72MHz can land well under that, so a retry's CS_N-high gap needs an
 * explicit hold — otherwise the retry itself gets filtered out and NACKs
 * forever. Generous margin, this only runs on a NACK anyway. */
static void hipims_spi_cs_gap_delay(void)
{
    for (volatile uint32_t i = 0; i < 200; i++) {}
}

static uint8_t hipims_spi_transact(const uint8_t *frame /* 6 bytes: addr + value + crc */)
{
    LL_GPIO_ResetOutputPin(HIPIMS_CS_PORT, HIPIMS_CS_PIN);

    for (uint8_t i = 0; i < 6; i++)
    {
        hipims_spi_transfer_byte(frame[i]);
    }
    uint8_t status = hipims_spi_transfer_byte(0x00); /* dummy TX, real RX = status */

    while (LL_SPI_IsActiveFlag_BSY(HIPIMS_SPI)) {}
    LL_GPIO_SetOutputPin(HIPIMS_CS_PORT, HIPIMS_CS_PIN);

    return status;
}

static void hipims_spi_debug_dump(const uint8_t *frame, uint8_t status, uint8_t attempt)
{
    debug_str("SPI tx#");
    debug_hex8(attempt);
    debug_str(" addr="); debug_hex8(frame[0]);
    debug_str(" val=");  debug_hex8(frame[1]); debug_hex8(frame[2]); debug_hex8(frame[3]); debug_hex8(frame[4]);
    debug_str(" crc=");  debug_hex8(frame[5]);
    debug_str(" status="); debug_hex8(status);
    debug_str((status & HIPIMS_STATUS_ACK) ? " ACK" : " NACK");
    debug_newline();
}

bool hipims_spi_write_reg(uint8_t reg_addr, int32_t value)
{
    uint32_t uvalue = (uint32_t)value;
    uint8_t frame[6];

    frame[0] = reg_addr;
    frame[1] = (uint8_t)(uvalue >> 24);
    frame[2] = (uint8_t)(uvalue >> 16);
    frame[3] = (uint8_t)(uvalue >> 8);
    frame[4] = (uint8_t)(uvalue);
    frame[5] = hipims_crc8(frame, 5);

    for (uint8_t attempt = 0; attempt < HIPIMS_SPI_MAX_RETRIES; attempt++)
    {
        s_last_status = hipims_spi_transact(frame);
        bool acked = (s_last_status & HIPIMS_STATUS_ACK) != 0;

        /* Only failures (or HIPIMS_DEBUG_SPI_FULL) hit the UART — see the
         * policy note at the top of this file. */
        if (HIPIMS_DEBUG_SPI_FULL || !acked)
        {
            hipims_spi_debug_dump(frame, s_last_status, attempt);
        }

        if (acked)
        {
            return true;
        }

        s_nack_count++;
        hipims_spi_cs_gap_delay(); /* keep CS_N high past the glitch filter before retrying */
    }

    s_tx_failures++;
    return false;
}

uint8_t hipims_spi_last_status(void)
{
    return s_last_status;
}

uint32_t hipims_spi_nack_count(void)
{
    return s_nack_count;
}

uint32_t hipims_spi_tx_failures(void)
{
    return s_tx_failures;
}

void hipims_spi_clear_failures(void)
{
    s_nack_count = 0;
    s_tx_failures = 0;
}
