/*
 * Calib.c
 *
 * Created: 12/4/2025 11:28:22 PM
 *  Author: Noname-29-lnin
 */ 
#include "./../Inc/Calib.h"

void Calib_display_Mode_20(void){
	lcd_goto_xy(0,0);
	lcd_write_character('2');
	lcd_write_character('0');
	
	lcd_goto_xy(0,14);
	lcd_write_character('2');
	lcd_write_character('5');
	
	lcd_goto_xy(1,0);
	lcd_write_character('5');
	lcd_write_character('0');
	
	lcd_goto_xy(1,14);
	lcd_write_character('8');
	lcd_write_character('0');
}