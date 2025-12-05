/*
 * LCD.h
 *
 * Created: 12/4/2025 9:43:59 AM
 *  Author: Noname-29-lnin
 */ 


#ifndef LCD_H_
#define LCD_H_

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define DATA_BUS		PORTB
#define CTL_BUS			PORTC
#define DATA_DDR		DDRB
#define CTL_DDR			DDRC

#define LCD_D4			2
#define LCD_D5			3
#define LCD_D6			4
#define LCD_D7			5

#define LCD_EN			2
#define	LCD_RW			1
#define	LCD_RS			0

#define LCD_CMD_CLEAR_DISPLAY	          0x01
#define LCD_CMD_CURSOR_HOME		          0x02

// Display control
#define LCD_CMD_DISPLAY_OFF                0x08
#define LCD_CMD_DISPLAY_NO_CURSOR          0x0c
#define LCD_CMD_DISPLAY_CURSOR_NO_BLINK    0x0E
#define LCD_CMD_DISPLAY_CURSOR_BLINK       0x0F

// Function set
#define LCD_CMD_4BIT_2ROW_5X7              0x28
#define LCD_CMD_8BIT_2ROW_5X7              0x38

//functions prototype
void lcd_init(void);
void lcd_send_command (uint8_t );
void lcd_write_character(uint8_t );
void lcd_write_word(uint8_t []);
void lcd_clear(void);
void lcd_goto_xy (uint8_t , uint8_t );

void lcd_display_Vol(uint8_t data);
void lcd_display_Col(uint8_t data);
void lcd_display_string(const char *str);

#endif /* LCD_H_ */
