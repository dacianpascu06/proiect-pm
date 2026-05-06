
#include "lcd.h"
#include "stdint.h"
#include "twi.h"
#include <util/delay.h>

uint8_t _backlight = LCD_BL;

void pcf8574_write(uint8_t data) {
  twi_start();
  twi_write((LCD_I2C_ADDR << 1) | 0x00);
  twi_write(data);
  twi_stop();
}

void lcd_pulse_en(uint8_t data) {
  pcf8574_write(data | LCD_EN);
  _delay_us(1);
  pcf8574_write(data & ~LCD_EN);
  _delay_us(50);
}

void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
  uint8_t data = (nibble & 0xF0) | mode | _backlight;
  lcd_pulse_en(data);
}

void lcd_write_byte(uint8_t byte, uint8_t mode) {
  lcd_write_nibble(byte & 0xF0, mode);
  lcd_write_nibble((byte << 4) & 0xF0, mode);
}

void lcd_command(uint8_t cmd) {
  lcd_write_byte(cmd, 0);
  if (cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME)
    _delay_ms(2);
  else
    _delay_us(50);
}

void lcd_putchar(char c) {
  lcd_write_byte((uint8_t)c, LCD_RS);
  _delay_us(50);
}

void lcd_print(const char *str) {
  while (*str)
    lcd_putchar(*str++);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  lcd_command(LCD_CMD_DDRAM | (col + ROW_OFFSET[row & 1]));
}

void lcd_clear(void) { lcd_command(LCD_CMD_CLEAR); }

void lcd_backlight(uint8_t on) {
  _backlight = on ? LCD_BL : 0;
  pcf8574_write(_backlight);
}

void lcd_init(void) {
  _delay_ms(50);

  lcd_write_nibble(0x30, 0);
  _delay_ms(5);
  lcd_write_nibble(0x30, 0);
  _delay_us(150);
  lcd_write_nibble(0x30, 0);
  _delay_us(150);

  lcd_write_nibble(0x20, 0);
  _delay_us(150);

  lcd_command(LCD_CMD_FUNCTION_SET);
  lcd_command(LCD_CMD_DISPLAY_OFF);
  lcd_command(LCD_CMD_CLEAR);
  lcd_command(LCD_CMD_ENTRY_MODE);
  lcd_command(LCD_CMD_DISPLAY_ON);
}
