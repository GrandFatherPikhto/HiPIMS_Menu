#include "hipims_lcd1602.h"

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_utils.h"

/* Common PCF8574 "IIC/I2C 1602" backpack: address 0x27 (PCF8574T, all address
 * pins pulled up), bit map RS=P0 RW=P1 EN=P2 BL=P3 D4..D7=P4..P7. RW is
 * always driven low here (write-only), so no busy-flag polling — commands
 * just get a conservative fixed delay instead. */
#define LCD_I2C_ADDR   0x27
#define LCD_BIT_RS     (1U << 0)
#define LCD_BIT_EN     (1U << 2)
#define LCD_BIT_BL     (1U << 3)

static void i2c1_write_byte(uint8_t dev_addr7, uint8_t data)
{
    while (LL_I2C_IsActiveFlag_BUSY(I2C1)) {}

    LL_I2C_GenerateStartCondition(I2C1);
    while (!LL_I2C_IsActiveFlag_SB(I2C1)) {}

    LL_I2C_TransmitData8(I2C1, (uint8_t)(dev_addr7 << 1));
    while (!LL_I2C_IsActiveFlag_ADDR(I2C1)) {}
    LL_I2C_ClearFlag_ADDR(I2C1);

    while (!LL_I2C_IsActiveFlag_TXE(I2C1)) {}
    LL_I2C_TransmitData8(I2C1, data);

    while (!LL_I2C_IsActiveFlag_BTF(I2C1)) {}
    LL_I2C_GenerateStopCondition(I2C1);
}

static void i2c1_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);

    LL_I2C_InitTypeDef init;
    LL_I2C_StructInit(&init);
    init.PeripheralMode  = LL_I2C_MODE_I2C;
    init.ClockSpeed      = 100000;
    init.DutyCycle       = LL_I2C_DUTYCYCLE_2;
    init.OwnAddress1     = 0;
    init.TypeAcknowledge = LL_I2C_ACK;
    init.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &init);

    LL_I2C_Enable(I2C1);
}

/* nibble must already be positioned in bits 4..7. */
static void lcd_write4(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble & 0xF0) | LCD_BIT_BL | (rs ? LCD_BIT_RS : 0);
    i2c1_write_byte(LCD_I2C_ADDR, (uint8_t)(data | LCD_BIT_EN));
    i2c1_write_byte(LCD_I2C_ADDR, data);
}

static void lcd_send(uint8_t value, uint8_t rs)
{
    lcd_write4(value & 0xF0, rs);
    lcd_write4((uint8_t)(value << 4), rs);
    LL_mDelay(1);
}

#define lcd_command(cmd) lcd_send((cmd), 0)
#define lcd_data(d)      lcd_send((d), 1)

void lcd1602_init(void)
{
    i2c1_init();

    LL_mDelay(50); /* HD44780 power-on wait */

    /* Classic 4-bit init sequence (HD44780 datasheet, "Initializing by
     * instruction"): three raw 0x3 nibbles, then switch to 4-bit mode. */
    lcd_write4(0x30, 0);
    LL_mDelay(5);
    lcd_write4(0x30, 0);
    LL_mDelay(1);
    lcd_write4(0x30, 0);
    LL_mDelay(1);
    lcd_write4(0x20, 0);
    LL_mDelay(1);

    lcd_command(0x28); /* function set: 4-bit, 2 lines, 5x8 font */
    lcd_command(0x08); /* display off */
    lcd1602_clear();
    lcd_command(0x06); /* entry mode: increment, no shift */
    lcd_command(0x0C); /* display on, cursor off, blink off */
}

void lcd1602_clear(void)
{
    lcd_command(0x01);
    LL_mDelay(2);
}

void lcd1602_set_cursor(uint8_t col, uint8_t row)
{
    static const uint8_t row_offset[2] = { 0x00, 0x40 };
    lcd_command((uint8_t)(0x80 | (row_offset[row & 1u] + col)));
}

void lcd1602_print(const char *str)
{
    while (*str)
    {
        lcd_data((uint8_t)*str++);
    }
}
