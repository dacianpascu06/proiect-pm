#include <stdint.h>

void twi_init(void);
void twi_start(void);
void twi_stop(void);
uint8_t twi_write(uint8_t data);
