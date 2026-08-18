#ifndef HIPIMS_STORAGE_H
#define HIPIMS_STORAGE_H

#include <stdint.h>

/* Loads the last saved value of every register from flash into RAM. Call
 * once at startup. */
void storage_init(void);

int32_t storage_get(uint16_t addr);

/* Appends a record to the flash log (no-op if the value didn't actually
 * change). Compacts the log automatically once it fills up. Call this once
 * per finished edit, not on every encoder tick — see hipims_storage.c for
 * why. */
void storage_set(uint16_t addr, int32_t value);

#endif /* HIPIMS_STORAGE_H */
