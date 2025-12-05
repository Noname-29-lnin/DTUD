#include <avr/io.h>
#include <stdbool.h>
#define F_CPU 16000000UL
#include "./Inc/LCD.h"
#include "./Inc/ads1115.h"
#include "./Inc/Calib.h"

#define POS_MODE_CALIB     0        // PD0
#define PIN_MODE_CALIB     PIND
#define CHECK_MODE_CALIB   ((PIN_MODE_CALIB & (1 << POS_MODE_CALIB)) != 0)

volatile uint8_t mode_calib = 0;
volatile bool mode_save = true;

int main(void)
{
	i2c_init();
	lcd_init();

	DDRD	&= ~(1 << POS_MODE_CALIB);   // set PD0 as INPUT
	PORTD	|= (1 << POS_MODE_CALIB);   // ENABLE pull-up

	lcd_goto_xy(0,0);
	lcd_display_string("Welcom DTUD");
	lcd_goto_xy(1,0);
	lcd_display_string("NTC Temp");

	while (1) {
		
	}
}
