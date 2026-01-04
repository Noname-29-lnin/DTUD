/*
 * LCD.c
 *  Author: Noanme-29-lnin
 */ 

#include "./../Inc/LCD.h"

static void lcd_pulse_enable(void)
{
	CTL_BUS |= (1 << LCD_EN);
	_delay_us(200);
	CTL_BUS &= ~(1 << LCD_EN);
	_delay_us(200);
}

/*
static void lcd_send_nibble(uint8_t nibble)
{
	DATA_BUS &= ~((1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7)); // reset
	DATA_BUS |= (nibble & 0xF0);
	lcd_pulse_enable();
}
*/
/*
static void lcd_send_nibble(uint8_t byte)
{
	DATA_BUS &= ~((1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7)); // clear PB2..PB5
	DATA_BUS |= ((byte & 0xF0) >> 2); // ??a bit7..4 -> PB5..PB2
	lcd_pulse_enable();
}

void lcd_send_command(uint8_t command)
{
	CTL_BUS &= ~(1 << LCD_RS);        // RS=0
	lcd_send_nibble(command & 0xF0);
	lcd_send_nibble((command << 4) & 0xF0);
	CTL_BUS &= ~(1 << LCD_RS);        // RS=0
}
void lcd_send_data(uint8_t data)
{
	CTL_BUS |= (1 << LCD_RS);         // RS=1
	lcd_send_nibble(data & 0xF0);
	lcd_send_nibble((data << 4) & 0xF0);
	CTL_BUS &= ~(1 << LCD_RS);        // RS=0
}
*/
/*
void lcd_init(void)
{
	DATA_DDR	|= (1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
	CTL_DDR		|= (1<<LCD_EN)|(1<<LCD_RW)|(1<<LCD_RS);
	CTL_BUS		&= ~(1<<LCD_RW);   // RW=0 write mode

	lcd_send_command(0x30);
	_delay_ms(5);
	lcd_send_command(0x30);
	_delay_ms(5);
	lcd_send_command(0x20);
	_delay_ms(5);

	lcd_send_command(LCD_CMD_4BIT_2ROW_5X7);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_ENTRY_MODE_INCREASE);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_DISPLAY_CURSOR_BLINK);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
}
*/
/*
void lcd_init(void)
{
	DATA_DDR |= (1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
	CTL_DDR  |= (1<<LCD_EN)|(1<<LCD_RW)|(1<<LCD_RS);

	CTL_BUS &= ~(1<<LCD_RS); // RS=0
	CTL_BUS &= ~(1<<LCD_RW); // RW=0
	_delay_ms(20);
	lcd_send_nibble(0x30);   // ch? nibble cao (0x3)
	_delay_ms(5);
	lcd_send_nibble(0x30);
	_delay_us(150);
	lcd_send_nibble(0x30);
	_delay_us(150);

	lcd_send_nibble(0x20);   // nibble cao = 0x2
	_delay_us(150);

	// T? ?ây m?i dùng send_command 4-bit bình th??ng
	lcd_send_command(LCD_CMD_4BIT_2ROW_5X7);       // 0x28
	lcd_send_command(LCD_CMD_DISPLAY_NO_CURSOR);   // 0x0C (ho?c blink n?u b?n mu?n)
	lcd_send_command(LCD_CMD_ENTRY_MODE_INCREASE); // 0x06
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);       // 0x01
	_delay_ms(2);
}
*/
static void lcd_send_nibble(uint8_t byte)
{
	DATA_BUS &= ~((1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7));
	DATA_BUS |= ((byte & 0xF0) >> 2); // bit7..4 -> PB5..PB2
	lcd_pulse_enable();
}

void lcd_send_command(uint8_t command)
{
	CTL_BUS &= ~(1 << LCD_RS);
	lcd_send_nibble(command & 0xF0);
	lcd_send_nibble((command << 4) & 0xF0);
	_delay_us(50);
}

void lcd_send_data(uint8_t data)
{
	CTL_BUS |= (1 << LCD_RS);
	lcd_send_nibble(data & 0xF0);
	lcd_send_nibble((data << 4) & 0xF0);
	_delay_us(50);
}

void lcd_init(void)
{
	DATA_DDR |= (1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
	CTL_DDR  |= (1<<LCD_EN)|(1<<LCD_RW)|(1<<LCD_RS);

	CTL_BUS &= ~((1<<LCD_RS)|(1<<LCD_RW)|(1<<LCD_EN));
	
	_delay_ms(50);
	
	lcd_send_nibble(0x30);
	_delay_ms(5);
	lcd_send_nibble(0x30);
	_delay_us(150);
	lcd_send_nibble(0x30);
	_delay_us(150);
	lcd_send_nibble(0x20);
	_delay_us(150);

	lcd_send_command(LCD_CMD_4BIT_2ROW_5X7);
	lcd_send_command(LCD_CMD_DISPLAY_NO_CURSOR);
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(2);
	lcd_send_command(LCD_CMD_ENTRY_MODE_INCREASE);
}

void lcd_write_string(const char* str)
{
	while(*str != '\0')
	{
		lcd_send_data(*str);
		str++;
	}
}

void lcd_write_data(float data, uint8_t digit)
{
	if (digit > 4) digit = 4;
	if (data < 0) {
		lcd_send_data('-');
		data = -data;
	}
	uint32_t int_part = (uint32_t)data;
	float frac_part = data - int_part;
	char buff[12];
	uint8_t idx = 0;
	if (int_part == 0) {
		buff[idx++] = '0';
		} else {
		char rev[12];
		uint8_t r = 0;
		while (int_part > 0) {
			rev[r++] = (int_part % 10) + '0';
			int_part /= 10;
		}
		while (r > 0)
		buff[idx++] = rev[--r];
	}
	buff[idx] = '\0';
	for (uint8_t k = 0; k < idx; k++)
	lcd_send_data(buff[k]);
	if (digit > 0) {
		lcd_send_data('.');
		for (uint8_t d = 0; d < digit; d++) {
			frac_part *= 10;
			uint8_t digit_val = (uint8_t)frac_part;
			lcd_send_data('0' + digit_val);
			frac_part -= digit_val;
		}
	}
}

void lcd_write_CusChar(uint8_t location, const uint8_t *charmap)
{
	location &= 0x07;
	lcd_send_command(0x40 | (location << 3));  // Set CGRAM address

	for (uint8_t i = 0; i < 8; i++) {
		lcd_send_data(charmap[i]);
	}
	lcd_send_command(0x80);
}

void lcd_goto_xy(uint8_t line, uint8_t pos) //line = 0 or 1
{
	lcd_send_command((0x80|(line<<6))+pos);
	_delay_us (50);
}

void lcd_clear(void)
{
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
}
