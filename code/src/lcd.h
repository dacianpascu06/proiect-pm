#include <stdint.h>

#define LCD_I2C_ADDR 0x27
#define LCD_RS (1 << 0)
#define LCD_RW (1 << 1)
#define LCD_EN (1 << 2)
#define LCD_BL (1 << 3)
#define LCD_D4 (1 << 4)
#define LCD_D5 (1 << 5)
#define LCD_D6 (1 << 6)
#define LCD_D7 (1 << 7)
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_HOME 0x02
#define LCD_CMD_ENTRY_MODE 0x06
#define LCD_CMD_DISPLAY_ON 0x0C
#define LCD_CMD_DISPLAY_OFF 0x08
#define LCD_CMD_FUNCTION_SET 0x28
#define LCD_CMD_DDRAM 0x80

const uint8_t ROW_OFFSET[2] = {0x00, 0x40};

void pcf8574_write(uint8_t data);

void lcd_pulse_en(uint8_t data);

void lcd_write_nibble(uint8_t nibble, uint8_t mode);
void lcd_write_byte(uint8_t byte, uint8_t mode);

void lcd_command(uint8_t cmd);

void lcd_putchar(char c);
void lcd_print(const char *str);

void lcd_set_cursor(uint8_t col, uint8_t row);

void lcd_clear(void);

void lcd_backlight(uint8_t on);

void lcd_init(void);
