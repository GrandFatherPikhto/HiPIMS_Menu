#ifndef HIPIMS_LCD1602_H
#define HIPIMS_LCD1602_H

#include <stdint.h>

void lcd1602_init(void);
void lcd1602_clear(void);
void lcd1602_set_cursor(uint8_t col, uint8_t row);
void lcd1602_print(const char *str);

#endif /* HIPIMS_LCD1602_H */
