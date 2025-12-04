/*
 * ADC_main.c
 *
 * Created: 12/4/2025 9:50:32 AM
 * Author : Noname-29-lnin
 */ 

#include <avr/io.h>
#include "LCD.h"
#include "ads1115.h"

int main(void)
{
	lcd_init();
	
	lcd_goto_xy(0,0);
	lcd_display_string("Welcom DTUD");
	lcd_goto_xy(1,0);
	lcd_display_string("NTC Temp");
    uint8_t temp_button_press_1 = 0;
	uint8_t temp_button_press_hold = 0;
	
	while (1) {
		if(temp_button_press_1){
			//func_1
		} else if(temp_button_press_hold){
			// func_2
		} else {
			// func_3
		}
    }
}


