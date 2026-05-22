#include "twi.h"
#include <avr/io.h>
#define TWBR_VALUE ((F_CPU / TWI_FREQ - 16) / 2) // pagina 180 datasheet
#define TWI_FREQ 100000UL                        // valoare standard

void twi_init(void) {
  /* Set I2C clock frequency */
  TWBR = (uint8_t)TWBR_VALUE;
  // set prescaler to 1
  TWSR = 0x00;
  /* enable I2C control register */
  TWCR = (1 << TWEN);
}

void twi_start(void) {
  // pg 183 datasheet
  // send start condition
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  // wait for START condition to be sent
  while (!(TWCR & (1 << TWINT)))
    ;
}

void twi_stop(void) {
  // send STOP condition(corresponding bit in TWCR)
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  // wait for stop to complete
  while (TWCR & (1 << TWSTO))
    ;
}

void twi_write(uint8_t data) {
  // Send a byte of data (TWCR + TWDR)
  TWDR = data;
  /* Enable I2C communication and clear interrupt flag */
  TWCR = (1 << TWINT) | (1 << TWEN);
  // wait for transfer to complete (TWINT flag)
  while (!(TWCR & (1 << TWINT)))
    ;
}
