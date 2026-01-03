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
#include <avr/eeprom.h>
#include <stdint.h>

#include "./Inc/ADS1115.h"
#include "./Inc/TWI_protocol.h"
#include "./Inc/Func.h"

#define POS_MODE_CALIB		0        // PD0
#define BTN_CHECKMODE		2
#define BTN_SAVEMODE		3
#define PIN_MODE_CALIB		PIND
#define PIN_CHECK			4
#define CHECK_MODE_CALIB	((PIN_MODE_CALIB & (1 << POS_MODE_CALIB)) == 0)
#define CHECK_LED_ON		PORTD|=(1<<PIN_CHECK)
#define CHECK_LED_OFF		PORTD&=~(1<<PIN_CHECK)
// Functional Proccess
void btn_interrup(void);
void Func_Calib20(void);
void Func_Calib25(void);
void Func_Calib50(void);
void Func_Calib80(void);
void ADC_print_calib(void);

// float CalADC_vol(uint16_t value_hex){
// 	return (float)(value_hex * 6.144)/(32768.0);
// }
float CalADC_vol(uint16_t value_hex){
    int16_t raw = (int16_t)value_hex;          // signed
    return ((float)raw) * 6.144f / 32768.0f;   // volts
}

float CalADC_cel(uint16_t value_hex){
	int16_t raw = (int16_t)value_hex;          // signed
	return (float)((((float)raw) * 6.144f)/(32768.0f) + 1.666667f) * 12.0;
}

// Global Variables
uint16_t ads1115_Data;
volatile bool btn_savemode = false;
typedef enum {
	CALIB_20	= 0,
	CALIB_25	= 1,
	CALIB_50	= 2,
	CALIB_80	= 3,
	CALIB_PRINT = 4
} SELECTMODE_e;
volatile SELECTMODE_e btn_selectmode = CALIB_20;
typedef enum {
	ADC_CEL 		= 0,
	ADC_VOL 		= 1,
	ADC_PRINT_CALIB = 2
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
	DDRB |= (1 << 0);
	PORTB |= (1 << 0);
	//PORTB &= ~(1 << 5);
	lcd_init();
	i2c_init();
	DDRD |= (1 << 7);
	PORTD |= (1 << 7);

	btn_savemode = false;
	btn_selectmode = CALIB_20;
	btn_adcmode = ADC_CEL;
	DDRD	&= ~((1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE));
	DDRD	|= (1<<PIN_CHECK);
	PORTD	|= (1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE);
	PORTD   &= ~(1<<PIN_CHECK);
	btn_interrup();
	
	
	lcd_goto_xy(0,0);
	lcd_write_string("DTUD - N04");
	lcd_goto_xy(1, 0);
	lcd_write_string("NTC: 20 - 80oC");
	CHECK_LED_ON;
	while(!btn_savemode){}
		btn_savemode = false;
	CHECK_LED_OFF;
	//_delay_ms(1000);
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(100);
	DDRD |= (1 << 6);
	while(1){
		if(CHECK_MODE_CALIB){
			PORTD |= (1 << 6);
			lcd_goto_xy(0,0);
			lcd_write_string("Calib mode");
			while(!btn_savemode){}
			lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
			_delay_ms(100);
			btn_savemode = false;
			while(1){
				calib_lcd_init();
				while (!btn_savemode){
					switch(btn_selectmode){
						case CALIB_20:
							calib_lcd_20();
							break;
						case CALIB_25:
							calib_lcd_25();
							break;
						case CALIB_50:
							calib_lcd_50();
							break;
						case CALIB_80:
							calib_lcd_80();
							break;
						case CALIB_PRINT:
							calib_lcd_print_result();
							break;
						default:
							calib_lcd_20();
							break;
					}
					_delay_ms(500);
				}
				btn_savemode = false;
				lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
				_delay_ms(100);
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
					case CALIB_PRINT:
						ADC_print_calib();
						break;
					default:
						Func_Calib20();
						break;
				}
				// save all value when calib in EEPROM
			}
		} else {
			PORTD &= ~(1 << 6);
			lcd_goto_xy(0,0);
			lcd_write_string("Normal mode");
			while(!btn_savemode){}
			btn_savemode = false;
			// write all values on EEPROM 
			lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
			_delay_ms(100);
			while(1){
				CalADC_lcd_init();
				btn_savemode = false;
				while(!btn_savemode){
					switch(btn_adcmode){
						case ADC_VOL: 
							CalADC_lcd_VOL();
							break;
						case ADC_CEL:
							CalADC_lcd_CEL();
							break;
						case ADC_PRINT_CALIB:
							CalADC_lcd_Print_Calib();
							break;
						default:
							CalADC_lcd_CEL();
							break;
					}
					_delay_ms(500);
				}
				btn_savemode = false;
				lcd_clear();
				_delay_ms(100);
				lcd_goto_xy(0,0);
				lcd_write_string("DTUD - N04");
				switch(btn_adcmode){
					case ADC_VOL: 
						lcd_goto_xy(1, 14);
						lcd_send_data('V');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);	
							lcd_write_data(CalADC_vol(caladc_temp), 4);
							lcd_send_data(' ');
							_delay_ms(100);
						}
						break;
					case ADC_CEL:
						lcd_goto_xy(1, 14);
						lcd_send_data(CELSIUS);
						lcd_send_data('C');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);
							lcd_write_data(CalADC_cel(caladc_temp), 2);
							lcd_send_data(' ');
							_delay_ms(100);
						}
						break;
					case ADC_PRINT_CALIB:
						ADC_print_calib();
						break;
					default:
						lcd_goto_xy(1, 14);
						lcd_send_data('V');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 0, DATARATE_128SPS, FSR_6_144);	
							lcd_write_data(CalADC_vol(caladc_temp), 4);
						}
						break;
				}
				btn_savemode = false;
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
	// if(CHECK_MODE_CALIB){
		btn_selectmode++;
		if (btn_selectmode > CALIB_PRINT) {btn_selectmode = CALIB_20;}
	// } else {
		btn_adcmode ++;
		if(btn_adcmode > ADC_PRINT_CALIB){ btn_adcmode = ADC_CEL;}
	// }
}

// BTN2 - SaveMode
ISR(INT1_vect){
	btn_savemode = true;
}

// Function - Calib20
void Func_Calib20(void){
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
void Func_Calib25(void){
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
void Func_Calib50(void){
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
void Func_Calib80(void){
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

void ADC_print_calib(void){
	btn_savemode = false;
	lcd_clear();
	_delay_ms(100);
	lcd_goto_xy(0,0);
	lcd_write_string("Calib 20oC");
	lcd_goto_xy(1, 0);
	lcd_write_data(CalADC_vol(calib_data_20), 4);
	lcd_goto_xy(1, 15);
	lcd_send_data('V');
	while (!btn_savemode){}
	
	btn_savemode = false;
	lcd_clear();
	_delay_ms(100);
	lcd_goto_xy(0,0);
	lcd_write_string("Calib 25oC");
	lcd_goto_xy(1, 0);
	lcd_write_data(CalADC_vol(calib_data_25), 4);
	lcd_goto_xy(1, 15);
	lcd_send_data('V');
	while (!btn_savemode){}
		
	btn_savemode = false;
	lcd_clear();
	_delay_ms(100);
	lcd_goto_xy(0,0);
	lcd_write_string("Calib 50oC");
	lcd_goto_xy(1, 0);
	lcd_write_data(CalADC_vol(calib_data_50), 4);
	lcd_goto_xy(1, 15);
	lcd_send_data('V');
	while (!btn_savemode){}
		
	btn_savemode = false;
	lcd_clear();
	_delay_ms(100);
	lcd_goto_xy(0,0);
	lcd_write_string("Calib 80oC");
	lcd_goto_xy(1, 0);
	lcd_write_data(CalADC_vol(calib_data_80), 4);
	lcd_goto_xy(1, 15);
	lcd_send_data('V');
	while (!btn_savemode){}
	btn_savemode = false;
}