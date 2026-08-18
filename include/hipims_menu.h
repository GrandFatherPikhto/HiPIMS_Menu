#ifndef HIPIMS_MENU_H
#define HIPIMS_MENU_H

void menu_init(void);

/* Call at a steady rate (e.g. every 10ms) from the main loop. */
void menu_tick(void);

#endif /* HIPIMS_MENU_H */
