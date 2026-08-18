#ifndef HIPIMS_DEBUG_H
#define HIPIMS_DEBUG_H

#include <stdint.h>

void debug_init(void);
void debug_str(const char *s);
void debug_hex8(uint8_t v);
void debug_newline(void);

#endif /* HIPIMS_DEBUG_H */
