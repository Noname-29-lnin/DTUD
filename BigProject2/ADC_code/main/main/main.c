/*
 * main.c
 *
 * Created: 12/5/2025 2:26:53 PM
 * Author : nguye
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "./Inc/ADS1115.h"
#include "./Inc/TWI_protocol.h"
#include "./Inc/Func.h"

#define POS_MODE_CALIB		0        // PD0
#define BTN_CHECKMODE		2
#define BTN_SAVEMODE		3
#define PIN_MODE_CALIB		PIND
#define CHECK_MODE_CALIB	((PIN_MODE_CALIB & (1 << POS_MODE_CALIB)) != 0)
// Functional Proccess
void btn_interrup(void);
void Func_Calib20(void);
void Func_Calib25(void);
void Func_Calib50(void);
void Func_Calib80(void);
// Global Variables
uint16_t ads1115_Data;
volatile bool btn_savemode = false;
typedef enum {
	CALIB_20	= 0,
	CALIB_25	= 1,
	CALIB_50	= 2,
	CALIB_80	= 3
} SELECTMODE_e;
volatile SELECTMODE_e btn_selectmode = CALIB_20;
typedef enum {
	ADC_CEL = 0,
	ADC_VOL = 1
} ADCMODE_e;
volatile ADCMODE_e btn_adcmode = ADC_CEL;

uint16_t calib_temp		= 0;
uint16_t calib_data_20	= 0;
uint16_t calib_data_25	= 2222;
uint16_t calib_data_50	= 13333;
uint16_t calib_data_80	= 26667;

uint16_t caladc_temp	= 0;

int main(void)
{
	i2c_init();								// Init I2C
	DDRD	&= ~((1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE));
	PORTD	|= (1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE);
	btn_interrup();
	
	lcd_goto_xy(0,0);
	lcd_write_string("DTUD - N04");
	lcd_goto_xy(1, 0);
	lcd_write_string("NTC: 20 - 80oC");
	_delay_ms(1000);
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(100);
    while (1) 
    {
		if(CHECK_MODE_CALIB){
			calib_lcd_init();
			switch(btn_selectmode){
				case CALIB_20:
					Func_Calib20();
					break;
				case CALIB_25:
					Func_Calib25();
					break;
				case CALIB_50:
					Func_Calib50();
					break;
				case CALIB_80:
					Func_Calib80();
					break;
				default:
					Func_Calib20();
					break;
			}
		} else {
			CalADC_lcd_init();
			//btn_savemode = false;
			switch(btn_adcmode){
				case ADC_VOL: 
					CalADC_lcd_VOL();
					if(!btn_savemode){
						break;
					} else {
						lcd_clear();
						lcd_goto_xy(1, 14);
						lcd_send_data('V');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);	
							lcd_write_data(CalADC_vol(caladc_temp), 4);
						}
						btn_savemode = false;
					}
					break;
				case ADC_CEL:
					CalADC_lcd_CEL();
					if(!btn_savemode){
						break;
						} else {
						lcd_clear();
						lcd_goto_xy(1, 14);
						lcd_send_data(CELSIUS);
						lcd_send_data('C');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
							lcd_write_data(CalADC_cel(caladc_temp), 2);
						}
						btn_savemode = false;
					}
					break;
				default:
					CalADC_lcd_CEL();
					if(!btn_savemode){
						break;
						} else {
						lcd_clear();
						lcd_goto_xy(1, 14);
						lcd_send_data(CELSIUS);
						lcd_send_data('C');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
							lcd_write_data(CalADC_cel(caladc_temp), 2);
						}
						btn_savemode = false;
					}
					break;
			}
		}
    }
}

void btn_interrup(void){
	EICRA |= (1 << ISC01) | (1 << ISC11);     // falling edge INT0, INT1
	EICRA &= ~((1 << ISC00) | (1 << ISC10));
	EIMSK |= (1 << INT0) | (1 << INT1);       // enable INT0 + INT1
	sei();
}

// BTN1 - SelectMode 
ISR(INT0_vect) {
	if(CHECK_MODE_CALIB){
		btn_selectmode++;
		if (btn_selectmode > CALIB_80) {btn_selectmode = CALIB_20;}
	} else {
		btn_adcmode ++;
		if(btn_adcmode > ADC_VOL){ btn_adcmode = ADC_CEL;}
	}
}

// BTN2 - SaveMode
ISR(INT1_vect){
	btn_savemode = true;
}

// Function - Calib20
void Func_Calib20(void){
	calib_lcd_20();
	if(btn_savemode) {
		btn_savemode = false;
		lcd_goto_xy(0,0);
		lcd_write_string("Calib 20oC");
		lcd_goto_xy(1, 15);
		lcd_send_data('V');
		while(!btn_savemode){
			calib_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
			lcd_goto_xy(1, 0);
			lcd_write_data(CalADC_vol(calib_temp), 4);
			_delay_ms(50);
		}
		 calib_data_20 = calib_temp;
		 btn_savemode = false;
	}
}
void Func_Calib25(void){
	calib_lcd_25();
	if(btn_savemode) {
		btn_savemode = false;
		lcd_goto_xy(0,0);
		lcd_write_string("Calib 25oC");
		lcd_goto_xy(1, 15);
		lcd_send_data('V');
		while(!btn_savemode){
			calib_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
			lcd_goto_xy(1, 0);
			lcd_write_data(CalADC_vol(calib_temp), 4);
			_delay_ms(50);
		}
		calib_data_25 = calib_temp;
		btn_savemode = false;
	}
}
void Func_Calib50(void){
	calib_lcd_50();
	if(btn_savemode){
		btn_savemode = false;
		lcd_goto_xy(0,0);
		lcd_write_string("Calib 50oC");
		lcd_goto_xy(1, 15);
		lcd_send_data('V');
		while(!btn_savemode){
			calib_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
			lcd_goto_xy(1, 0);
			lcd_write_data(CalADC_vol(calib_temp), 4);
			_delay_ms(50);
		}
		calib_data_50 = calib_temp;
		btn_savemode = false;
	}
}
void Func_Calib80(void){
	calib_lcd_80();
	if(btn_savemode){
		btn_savemode = false;
		lcd_goto_xy(0,0);
		lcd_write_string("Calib 80oC");
		lcd_goto_xy(1, 15);
		lcd_send_data('V');
		while(!btn_savemode){
			calib_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
			lcd_goto_xy(1, 0);
			lcd_write_data(CalADC_vol(calib_temp), 4);
			_delay_ms(50);
		}
		calib_data_80 = calib_temp;
		btn_savemode = false;
	}
}