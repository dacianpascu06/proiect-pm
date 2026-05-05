#include <avr/io.h>
#include <util/delay.h>
int main() {

  DDRB = (1 << PB5);
  for (;;) {
    PORTB |= (1 << PB5);
    _delay_ms(1000);
    PORTB &= ~(1 << PB5);
    _delay_ms(1000);
  }
}
