#ifndef FUNC_H_
#define FUNC_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "./LCD.h"

// LCD
#define BOLD_0      0
extern const uint8_t char_bold_0[8];

#define BOLD_2      1
extern const uint8_t char_bold_2[8];

#define BOLD_5      2
extern const uint8_t char_bold_5[8];

#define BOLD_8      3
extern const uint8_t char_bold_8[8];

#define CELSIUS     7
extern const uint8_t char_colsius[8];

extern const uint8_t char_adcmode_vol[8];
extern const uint8_t char_adcmode_vol_1[8];
extern const uint8_t char_adcmode_vol_2[8];

extern const uint8_t char_adcmode_cel[8];
#define ADCMODE_CEL_1 4
extern const uint8_t char_adcmode_cel_1[8];
#define ADCMODE_CEL_2 5
extern const uint8_t char_adcmode_cel_2[8];

// Function prototypes
void calib_lcd_init(void);
void calib_lcd_20(void);
void calib_lcd_25(void);
void calib_lcd_50(void);
void calib_lcd_80(void);

void CalADC_lcd_init(void);
void CalADC_lcd_VOL(void);
void CalADC_lcd_CEL(void);
float CalADC_vol(uint16_t value_hex);
float CalADC_cel(uint16_t value_hex);

#endif
