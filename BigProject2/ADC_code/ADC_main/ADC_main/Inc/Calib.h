/*
 * Calib.h
 *
 * Created: 12/4/2025 11:28:34 PM
 *  Author: nguye
 */ 


#ifndef CALIB_H_
#define CALIB_H_

#include <avr/io.h>
#include "LCD.h"

// Calib
#define MODE_20		0
#define MODE_25		1
#define MODE_50		2
#define MODE_80		3

void Calib_display_Mode_20(void);

#endif /* CALIB_H_ */