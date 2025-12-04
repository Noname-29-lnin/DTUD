/*
 * LCD.c
 *
 * Created: 12/4/2025 9:43:44 AM
 *  Author: Noname-29-lnin
 */ 
#include "LCD.h"


void lcd_init(void) {	
	
	DATA_DDR = (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5)| (1<<LCD_D4);
	CTL_DDR |= (1<<LCD_EN)|(1<<LCD_RW)|(1<<LCD_RS);

	
	DATA_BUS = (0<<LCD_D7)|(0<<LCD_D6)|(1<<LCD_D5)|(0<<LCD_D4);
	CTL_BUS|= (1<<LCD_EN)|(0<<LCD_RW)|(0<<LCD_RS);
	_delay_ms(1);
	CTL_BUS &=~(1<<LCD_EN);
	_delay_ms(1);
	
	lcd_send_command(LCD_CMD_4BIT_2ROW_5X7);
	_delay_ms(1);
	lcd_send_command(LCD_CMD_DISPLAY_CURSOR_BLINK);
	_delay_ms(1);
	lcd_send_command(0x80);
	
}

void lcd_send_command (uint8_t command)
{
	DATA_BUS=(command&0b11110000); 
	CTL_BUS &=~(1<<LCD_RS);
	CTL_BUS |=(1<<LCD_EN);
	_delay_ms(1);
	CTL_BUS &=~(1<<LCD_EN);
	_delay_ms(1);
	DATA_BUS=((command&0b00001111)<<4);
	CTL_BUS |=(1<<LCD_EN);
	_delay_ms(1);
	CTL_BUS &=~(1<<LCD_EN);
	_delay_ms(1);
}

void lcd_write_word(uint8_t word[20])
{
	int i=0;
	while(word[i]!='\0')
	{
		lcd_write_character(word[i]);
		i++;
	}
}

void lcd_write_character(uint8_t character)
{
	
	DATA_BUS=(character & 0b11110000);
	CTL_BUS|=(1<<LCD_RS);
	CTL_BUS |=(1<<LCD_EN);
	_delay_ms(2);
	CTL_BUS &=~(1<<LCD_EN);
	_delay_ms(2);
	DATA_BUS=((character & 0b00001111)<<4);
	CTL_BUS |=(1<<LCD_EN);
	_delay_ms(2);
	CTL_BUS &=~(1<<LCD_EN);
	_delay_ms(2);
	
}

void lcd_clear(void)
{
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
}

void lcd_goto_xy(uint8_t line, uint8_t pos) //line = 0 or 1
{
	lcd_send_command((0x80|(line<<6))+pos);
	_delay_us (50);
}

void lcd_display_data(float data, uint8_t digit)
{
	if (digit > 4) digit = 4;
	if (data < 0) {
		lcd_write_character('-');
		data = -data;
	}
	uint32_t int_part = (uint32_t)data;
	char buffer[8];
	uint8_t i = 0;
	if (int_part == 0)
		buffer[i++] = '0';
	else {
		uint32_t temp = int_part;
		char rev[8];
		uint8_t j = 0;

		while (temp > 0) {
			rev[j++] = (temp % 10) + '0';
			temp /= 10;
		}
		while (j > 0) buffer[i++] = rev[--j];
	}

	buffer[i] = '\0';
	lcd_write_word((uint8_t*)buffer);
	if (digit == 0) return;
	lcd_write_character('.');

	float fractional = data - (float)int_part;
	for (uint8_t k = 0; k < digit; k++)
	fractional *= 10;
	uint32_t frac_part = (uint32_t)(fractional + 0.001);
	char frac_buf[8];
	for (int8_t k = digit - 1; k >= 0; k--) {
		frac_buf[k] = (frac_part % 10) + '0';
		frac_part /= 10;
	}
	frac_buf[digit] = '\0';
	lcd_write_word((uint8_t*)frac_buf);
}

void lcd_display_Vol(uint8_t data){
	 lcd_goto_xy(1, 0);
	 lcd_display_data(data, 4);

	 lcd_goto_xy(1, 14);
	 lcd_write_character('V');
}

void lcd_display_Col(uint8_t data){
	lcd_goto_xy(1, 0);
	lcd_display_data(data, 2);

	lcd_goto_xy(1, 14);
	lcd_write_character(0xDF);
	lcd_write_character('C');
}

void lcd_display_string(const char *str)
{
	while (*str != '\0'){
		lcd_write_character(*str);
		str++;
	}
}
