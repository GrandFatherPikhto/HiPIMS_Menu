#include "hipims_storage.h"

#include <string.h>

#include "stm32f1xx_ll_system.h" /* pulls in the device header for FLASH-> registers */

#include "hipims_spi.h" /* HIPIMS_NUM_REGS */

/* Wear-leveled key/value log in the LAST 1KB page of flash (STM32F103C8:
 * 64KB flash, 1KB pages -> page 63 at 0x0800FC00). Firmware is ~4KB today;
 * as long as it stays well under ~63KB this page is never touched by the
 * linker. If the build ever grows close to that, this needs a proper
 * reserved linker region instead of just this comment.
 *
 * Each save appends a 6-byte record (addr + 32-bit value) instead of
 * erasing+rewriting the whole page, so the ~10000-cycle page-erase
 * endurance is spent roughly once per STORAGE_MAX_RECORDS saves instead of
 * once per save — see storage_set(). */
#define STORAGE_PAGE_ADDR    0x0800FC00UL
#define STORAGE_PAGE_SIZE    1024u
#define STORAGE_RECORD_BYTES 6u
#define STORAGE_MAX_RECORDS  (STORAGE_PAGE_SIZE / STORAGE_RECORD_BYTES)
#define STORAGE_EMPTY_ADDR   0xFFFFu
#define STORAGE_NUM_SLOTS    HIPIMS_NUM_REGS

static int32_t s_cache[STORAGE_NUM_SLOTS];
static uint16_t s_next_record = 0;

static uint16_t flash_read_hw(uint32_t addr)
{
    return *(volatile uint16_t *)addr;
}

static void flash_wait_busy(void)
{
    while (FLASH->SR & FLASH_SR_BSY) {}
}

static void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static void flash_erase_page(uint32_t page_addr)
{
    flash_wait_busy();
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = page_addr;
    FLASH->CR |= FLASH_CR_STRT;
    flash_wait_busy();
    FLASH->CR &= ~FLASH_CR_PER;
}

static void flash_program_hw(uint32_t addr, uint16_t data)
{
    flash_wait_busy();
    FLASH->CR |= FLASH_CR_PG;
    *(volatile uint16_t *)addr = data;
    flash_wait_busy();
    FLASH->CR &= ~FLASH_CR_PG;
}

/* addr written last: a power loss mid-record leaves it reading as
 * STORAGE_EMPTY_ADDR, so a torn write is just ignored on the next scan
 * instead of being read back as a bogus value. */
static void write_record(uint16_t slot, uint16_t addr, int32_t value)
{
    uint32_t base = STORAGE_PAGE_ADDR + (uint32_t)slot * STORAGE_RECORD_BYTES;
    uint16_t value_lo = (uint16_t)((uint32_t)value & 0xFFFFu);
    uint16_t value_hi = (uint16_t)(((uint32_t)value >> 16) & 0xFFFFu);

    flash_program_hw(base + 0u, value_lo);
    flash_program_hw(base + 2u, value_hi);
    flash_program_hw(base + 4u, addr);
}

static void compact(void)
{
    flash_unlock();
    flash_erase_page(STORAGE_PAGE_ADDR);

    for (uint16_t addr = 0; addr < STORAGE_NUM_SLOTS; addr++)
    {
        write_record(addr, addr, s_cache[addr]);
    }

    flash_lock();
    s_next_record = STORAGE_NUM_SLOTS;
}

void storage_init(void)
{
    memset(s_cache, 0, sizeof(s_cache));
    s_next_record = 0;

    for (uint16_t slot = 0; slot < STORAGE_MAX_RECORDS; slot++)
    {
        uint32_t base = STORAGE_PAGE_ADDR + (uint32_t)slot * STORAGE_RECORD_BYTES;
        uint16_t addr = flash_read_hw(base + 4u);

        if (addr == STORAGE_EMPTY_ADDR)
        {
            break; /* sequential log: first empty slot marks the end */
        }

        s_next_record = (uint16_t)(slot + 1u);

        if (addr < STORAGE_NUM_SLOTS)
        {
            uint16_t value_lo = flash_read_hw(base + 0u);
            uint16_t value_hi = flash_read_hw(base + 2u);
            s_cache[addr] = (int32_t)(((uint32_t)value_hi << 16) | value_lo);
        }
    }
}

int32_t storage_get(uint16_t addr)
{
    if (addr >= STORAGE_NUM_SLOTS) { return 0; }
    return s_cache[addr];
}

void storage_set(uint16_t addr, int32_t value)
{
    if (addr >= STORAGE_NUM_SLOTS || s_cache[addr] == value)
    {
        return; /* out of range, or unchanged - nothing worth wearing flash for */
    }

    s_cache[addr] = value;

    if (s_next_record >= STORAGE_MAX_RECORDS)
    {
        compact();
        return;
    }

    flash_unlock();
    write_record(s_next_record, addr, value);
    flash_lock();
    s_next_record++;
}
