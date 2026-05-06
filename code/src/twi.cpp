#include "twi.h"
#include <avr/io.h>
#define TWBR_VALUE ((F_CPU / TWI_FREQ - 16) / 2) /* = 72              */
#define TWI_FREQ 100000UL

void twi_init(void) {
  TWBR = (uint8_t)TWBR_VALUE;
  TWSR = 0x00;
  TWCR = (1 << TWEN);
}

void twi_start(void) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

void twi_stop(void) {
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  while (TWCR & (1 << TWSTO))
    ;
}

uint8_t twi_write(uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  return (TWSR & 0xF8);
}
