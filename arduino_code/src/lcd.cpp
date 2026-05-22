
#include "lcd.h"
#include "stdint.h"
#include "twi.h"
#include <util/delay.h>

uint8_t backlight = LCD_BL;

// helper function to send data to the controller
void pcf8574_write(uint8_t data) {
  // typical i2c
  twi_start();
  // sends slave address
  twi_write((LCD_I2C_ADDR << 1) | 0x00);
  // sends the actual data
  twi_write(data);
  twi_stop();
}

void lcd_pulse_en(uint8_t data) {
  // send the data with the pulse beat  on
  pcf8574_write(data | LCD_EN);
  _delay_us(1);
  // quickly resend the data with the pulse beat off to make the lcd capture the
  // data
  pcf8574_write(data & ~LCD_EN);
  _delay_us(50);
}

void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
  uint8_t data = (nibble & 0xF0) | mode | backlight;
  lcd_pulse_en(data);
}

// sends a full byte as two nibbles, high nibble first
void lcd_write_byte(uint8_t byte, uint8_t mode) {
  // F0 = 11110000
  lcd_write_nibble(byte & 0xF0, mode);        // high nibble
  lcd_write_nibble((byte << 4) & 0xF0, mode); // low nibble
}

void lcd_command(uint8_t cmd) {
  // mode = 0 for command
  lcd_write_byte(cmd, 0);
  // clear and home take up to 1.52ms, all other commands < 40us
  if (cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME)
    _delay_ms(2);
  else
    _delay_us(50);
}

// sends a character to the current cursor position
void lcd_putchar(char c) {
  // mode is 1 for text
  lcd_write_byte((uint8_t)c, 1);
  _delay_us(50);
}

void lcd_print(const char *str) {
  while (*str)
    lcd_putchar(*str++);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  // every spot on the screen has a specific memory address stored
  // in the lcd ddram, row0 is at 0x00 and row1 is at 0x40
  // the command is setting the cursor position at row + col
  lcd_command(LCD_CMD_DDRAM | (col + ROW_OFFSET[row & 1]));
}

// send clear command
void lcd_clear(void) { lcd_command(LCD_CMD_CLEAR); }

void lcd_backlight(uint8_t on) {
  // control blacklight
  backlight = 0;
  if (on) {
    backlight = LCD_BL;
  }
  pcf8574_write(backlight);
}

void lcd_init(void) {
  // this is a specific init function to set the lcd
  // to operate in nibble mode
  // as we can send 8 bits to the controller
  // but between the controller and the lcd are 11 pins
  _delay_ms(50); // wait for LCD power-on reset >= 40ms

  // send 0x30 three times to guarantee 8-bit mode regardless of startup state
  lcd_write_nibble(0x30, 0);
  _delay_ms(5);
  lcd_write_nibble(0x30, 0);
  _delay_us(150);
  lcd_write_nibble(0x30, 0);
  _delay_us(150);

  // switch to 4-bit mode
  lcd_write_nibble(0x20, 0);
  _delay_us(150);

  lcd_command(LCD_CMD_FUNCTION_SET); // 4-bit, 2 lines, 5x8 font
  lcd_command(LCD_CMD_DISPLAY_OFF);  // display off (required by init spec)
  lcd_command(LCD_CMD_CLEAR);        // clear
  lcd_command(LCD_CMD_ENTRY_MODE);   // cursor moves right mode
  lcd_command(LCD_CMD_DISPLAY_ON);   // display on (pg 209)
}
