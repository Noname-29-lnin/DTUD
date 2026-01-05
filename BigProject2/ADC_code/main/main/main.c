/*
 * main.c
 *
 * Created: 12/5/2025 2:26:53 PM
 * Author : Noname-29-lnin
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
void calib_lcd_reset(void);

// Global Variables
uint16_t ads1115_Data;
volatile bool btn_savemode = false;
typedef enum {
	CALIB_20	= 0,
	CALIB_25	= 1,
	CALIB_50	= 2,
	CALIB_80	= 3,
	CALIB_PRINT = 4,
	CALIB_RESET = 5
} SELECTMODE_e;
volatile SELECTMODE_e btn_selectmode = CALIB_20;
typedef enum {
	ADC_CEL 		= 0,
	ADC_VOL 		= 1,
	ADC_PRINT_CALIB = 2
} ADCMODE_e;
volatile ADCMODE_e btn_adcmode = ADC_CEL;

#define EEP_ADDR_20     0x00
#define EEP_ADDR_25     0x02
#define EEP_ADDR_50     0x04
#define EEP_ADDR_80     0x06
uint16_t calib_temp		= 0;
uint16_t calib_data_20	= 0;
uint16_t calib_data_25	= 2222;
uint16_t calib_data_50	= 13333;
uint16_t calib_data_80	= 26667;
void Save_Calib_To_EEPROM(void) {
	eeprom_update_word((uint16_t*)EEP_ADDR_20, calib_data_20);
	eeprom_update_word((uint16_t*)EEP_ADDR_25, calib_data_25);
	eeprom_update_word((uint16_t*)EEP_ADDR_50, calib_data_50);
	eeprom_update_word((uint16_t*)EEP_ADDR_80, calib_data_80);
}
uint16_t caladc_temp	= 0;
float calib_slope = 0.00225f;
float calib_offset = 20.0f;
const float standard_temps[4] = {20.0f, 25.0f, 50.0f, 80.0f};
void Update_Calib_Coefficients(void) {
	float sum_x = 0;
	float sum_y = 0;
	float sum_xy = 0;
	float sum_xx = 0;
	float count = 4.0f;
	float adc_values[4];
	adc_values[0] = (float)((int16_t)calib_data_20);
	adc_values[1] = (float)((int16_t)calib_data_25);
	adc_values[2] = (float)((int16_t)calib_data_50);
	adc_values[3] = (float)((int16_t)calib_data_80);
	for (int i = 0; i < 4; i++) {
		sum_x  += adc_values[i];
		sum_y  += standard_temps[i];
		sum_xy += adc_values[i] * standard_temps[i];
		sum_xx += adc_values[i] * adc_values[i];
	}
	float denominator = (count * sum_xx) - (sum_x * sum_x);
	if (denominator != 0) {
		calib_slope = ((count * sum_xy) - (sum_x * sum_y)) / denominator;
		calib_offset = (sum_y - (calib_slope * sum_x)) / count;
		} else {
		calib_slope = 0.00225f;
		calib_offset = 20.0f;
	}
}
void Reset_Calib_To_EEPROM(void) {
	calib_data_20 = 0;
	calib_data_25 = 2222;
	calib_data_50 = 13333;
	calib_data_80 = 26667;
	Save_Calib_To_EEPROM();
	Update_Calib_Coefficients();
}
void Load_Calib_From_EEPROM(void) {
	uint16_t temp_val;
	// Read data calib 20
	temp_val = eeprom_read_word((uint16_t*)EEP_ADDR_20);
	if(temp_val != 0xFFFF) calib_data_20 = temp_val;
	// Read data calib 25
	temp_val = eeprom_read_word((uint16_t*)EEP_ADDR_25);
	if(temp_val != 0xFFFF) calib_data_25 = temp_val;
	// Read data calib 50
	temp_val = eeprom_read_word((uint16_t*)EEP_ADDR_50);
	if(temp_val != 0xFFFF) calib_data_50 = temp_val;
	// Read data calib 80
	temp_val = eeprom_read_word((uint16_t*)EEP_ADDR_80);
	if(temp_val != 0xFFFF) calib_data_80 = temp_val;
	Update_Calib_Coefficients();
}
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

float CalADC_cel_aftercalib(uint16_t value_hex){
	int16_t raw = (int16_t)value_hex;
	float temperature = (calib_slope * (float)raw) + calib_offset;
	return temperature;
}

uint16_t Read_data_ADC(){
	return ads1115_readADC_SingleEnded(ADS1115_ADDR_GND, 1, DATARATE_128SPS, FSR_6_144);
}

int main(void)
{
	_delay_ms(100);
	DDRB |= (1 << 0);
	PORTB |= (1 << 0);
	//PORTB &= ~(1 << 5);
	lcd_init();
	i2c_init();
	_delay_ms(100);
	PORTB &= ~(1 << 0);

	btn_savemode = false;
	btn_selectmode = CALIB_20;
	btn_adcmode = ADC_CEL;
	DDRD	&= ~((1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE));
	DDRD	|= (1<<PIN_CHECK);
	PORTD	|= (1 << POS_MODE_CALIB) | (1 << BTN_CHECKMODE) | (1 << BTN_SAVEMODE);
	PORTD   &= ~(1<<PIN_CHECK);
	btn_interrup();
	_delay_ms(100);
	PORTB |= (1 << 0);
	
	lcd_goto_xy(0,0);
	lcd_write_string("DTUD - N04");
	lcd_goto_xy(1, 0);
	lcd_write_string("NTC: 20 - 80oC");
	DDRD |= (1 << 7);
	PORTD |= (1 << 7);
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
						case CALIB_RESET:
							calib_lcd_reset();
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
					case CALIB_RESET:
						Reset_Calib_To_EEPROM();
						_delay_ms(100);
						lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
						_delay_ms(100);
						lcd_goto_xy(0,0);
						lcd_write_string("Done Reset");
						_delay_ms(500);
						break;
					default:
						Func_Calib20();
						break;
				}
				// save all value when calib in EEPROM
				lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
				_delay_ms(100);
				Save_Calib_To_EEPROM();
				_delay_ms(500);
				lcd_goto_xy(0,0);
				lcd_write_string("Saved to EEPROM ");
				_delay_ms(500);
			}
		} else {
			PORTD &= ~(1 << 6);
			lcd_goto_xy(0,0);
			lcd_write_string("Normal mode");
			while(!btn_savemode){}
			btn_savemode = false;
			// write all values on EEPROM 
			_delay_ms(500);
			lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
			_delay_ms(500);
			lcd_goto_xy(0,0);
			lcd_write_string("Done Load");
			lcd_goto_xy(1,0);
			lcd_write_string("EEPROM");
			Load_Calib_From_EEPROM();
			_delay_ms(500);
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
							caladc_temp = Read_data_ADC();
							lcd_write_data(CalADC_vol(caladc_temp), 4);
							lcd_send_data(' ');
							_delay_ms(500);
						}
						break;
					case ADC_CEL:
						lcd_goto_xy(1, 14);
						lcd_send_data(CELSIUS);
						lcd_send_data('C');
						while(!btn_savemode){
							lcd_goto_xy(1, 0);
							caladc_temp = Read_data_ADC();
							lcd_write_data(CalADC_cel(caladc_temp), 2);
							//lcd_write_data(CalADC_cel_aftercalib(caladc_temp), 2);
							lcd_send_data(' ');
							_delay_ms(500);
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
							caladc_temp = Read_data_ADC();
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
		if (btn_selectmode > CALIB_RESET) {btn_selectmode = CALIB_20;}
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
		calib_temp = Read_data_ADC();
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
		calib_temp = Read_data_ADC();
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
		calib_temp = Read_data_ADC();
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
		calib_temp = Read_data_ADC();
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
void calib_lcd_reset(void){
	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
	_delay_ms(100);
	lcd_goto_xy(0,0);
	lcd_write_string("Reset Value ?");
}
