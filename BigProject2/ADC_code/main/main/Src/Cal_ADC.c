/*
 * Cal_ADC.c
 *  Author: Noname-29-lnin
 */ 

#include "./../Inc/Func.h"
const uint8_t char_colsius[8] = {
	0b00110,0b01001,0b00110,0b00000,0b00000,0b00000,0b00000,0b00000
};
const uint8_t char_adcmode_vol[8] = {
	0b11111,0b01110,0b01110,0b01110,0b10101,0b10101,0b11011,0b11111
};

const uint8_t char_adcmode_vol_1[8] = {
	0b00000,0b00000,0b00000,0b00100,0b01010,0b01010,0b00100,0b00000
};

const uint8_t char_adcmode_vol_2[8] = {
	0b11000,0b10110,0b10101,0b10011,0b10111,0b00110,0b10001,0b11111
};

const uint8_t char_adcmode_cel[8] = {
	0b10001,0b00100,0b00111,0b00111,0b00111,0b00100,0b10001,0b11111
};

const uint8_t char_adcmode_cel_1[8] = {
	0b11111,0b11111,0b10001,0b01101,0b00011,0b01111,0b10000,0b11111
};

const uint8_t char_adcmode_cel_2[8] = {
	0b11000,0b10110,0b10101,0b10011,0b10111,0b00110,0b10001,0b11111
};

void CalADC_lcd_init(void){
	lcd_write_CusChar(BOLD_0, char_adcmode_vol);
	lcd_write_CusChar(BOLD_2, char_adcmode_vol_1);
	lcd_write_CusChar(BOLD_5, char_adcmode_vol_2);
	lcd_write_CusChar(BOLD_8, char_adcmode_cel);
	lcd_write_CusChar(ADCMODE_CEL_1, char_adcmode_cel_1);
	lcd_write_CusChar(ADCMODE_CEL_2, char_adcmode_cel_2);
}
void CalADC_lcd_VOL(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(1,0);
	lcd_send_command(BOLD_0);
	lcd_send_command(BOLD_2);
	lcd_send_command(BOLD_5);
	lcd_goto_xy(1,13);
	lcd_send_command('C');
	lcd_send_command('e');
	lcd_send_command('l');
}
void CalADC_lcd_CEL(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(1,0);
	lcd_send_command('V');
	lcd_send_command('o');
	lcd_send_command('l');
	lcd_goto_xy(1,13);
	lcd_send_command(BOLD_8);
	lcd_send_command(ADCMODE_CEL_1);
	lcd_send_command(ADCMODE_CEL_2);
}

float CalADC_vol(uint16_t value_hex){
	return (float)(value_hex * 6.144)/(32768.0);
}
float CalADC_cel(uint16_t value_hex){
	return (float)((value_hex * 6.144)/(32768.0) + 1.666667) * 12.0;
}