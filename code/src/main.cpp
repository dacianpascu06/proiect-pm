#include "lcd.h"
#include "twi.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  twi_init();
  lcd_init();

  lcd_set_cursor(0, 0);
  lcd_print("  Hello ");
  lcd_set_cursor(0, 1);
  lcd_print("  Dacian! ");

  for (;;) {
    _delay_ms(10000);
  }
}
