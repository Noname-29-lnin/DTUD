/*
 * calib.c
 *  Author: Noname-29-lnin
 */ 

#include "./../Inc/Func.h"

const uint8_t char_bold_0[8] = {
	0b10001,0b00100,0b00100,0b00100,0b00100,0b00100,0b10001,0b00000
};

const uint8_t char_bold_2[8] = {
	0b10001,0b00100,0b11100,0b11001,0b10011,0b00111,0b00000,0b00000
};

const uint8_t char_bold_5[8] = {
	0b00000,0b00111,0b00001,0b11100,0b11100,0b00100,0b10001,0b00000
};

const uint8_t char_bold_8[8] = {
	0b10001,0b00100,0b00100,0b10001,0b00100,0b00100,0b10001,0b00000
};

void calib_lcd_init(void){
	lcd_write_CusChar(BOLD_0, char_bold_0);
	lcd_write_CusChar(BOLD_2, char_bold_2);
	lcd_write_CusChar(BOLD_5, char_bold_5);
	lcd_write_CusChar(BOLD_8, char_bold_8);
	lcd_write_CusChar(CELSIUS, char_colsius);
}

void calib_lcd_20(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(0,0);
	lcd_send_data(BOLD_2);
	lcd_send_data(BOLD_0);
	lcd_goto_xy(0, 14);
	lcd_send_data('2');
	lcd_send_data('5');
	lcd_goto_xy(1, 0);
	lcd_send_data('5');
	lcd_send_data('0');
	lcd_goto_xy(1, 14);
	lcd_send_data('8');
	lcd_send_data('0');
}
void calib_lcd_25(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(0,0);
	lcd_send_data('2');
	lcd_send_data('0');
	lcd_goto_xy(0, 14);
	lcd_send_data(BOLD_2);
	lcd_send_data(BOLD_5);
	lcd_goto_xy(1, 0);
	lcd_send_data('5');
	lcd_send_data('0');
	lcd_goto_xy(1, 14);
	lcd_send_data('8');
	lcd_send_data('0');
}
void calib_lcd_50(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(0,0);
	lcd_send_data('2');
	lcd_send_data('0');
	lcd_goto_xy(0, 14);
	lcd_send_data('2');
	lcd_send_data('5');
	lcd_goto_xy(1, 0);
	lcd_send_data(BOLD_5);
	lcd_send_data(BOLD_0);
	lcd_goto_xy(1, 14);
	lcd_send_data('8');
	lcd_send_data('0');
}
void calib_lcd_80(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(0,0);
	lcd_send_data('2');
	lcd_send_data('0');
	lcd_goto_xy(0, 14);
	lcd_send_data('2');
	lcd_send_data('5');
	lcd_goto_xy(1, 0);
	lcd_send_data('5');
	lcd_send_data('0');
	lcd_goto_xy(1, 14);
	lcd_send_data(BOLD_8);
	lcd_send_data(BOLD_0);
}

void calib_lcd_print_result(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(5);
	lcd_goto_xy(0,0);
	lcd_write_string("Val_Calib ?");
}
