/*
 * TWI_protocol.h
 *
 *  Author: Noname-29-lnin
 */ 


#ifndef TWI_PROTOCOL_H_
#define TWI_PROTOCOL_H_

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
// Library reference
#include <avr/io.h>
#include <inttypes.h>
#include <compat/twi.h>

#define I2C_READ    1
#define I2C_WRITE   0
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/* I2C clock in Hz */
#define SCL_CLOCK  100000L


void i2c_init(void);

/** 
 @brief Terminates the data transfer and releases the I2C bus 
 @return none
 */
extern void i2c_stop(void);

/** 
 @brief Issues a start condition and sends address and transfer direction 

 @param    addr address and transfer direction of I2C device
 @retval   0   device accessible 
 @retval   1   failed to access device 
 */
extern unsigned char i2c_start(unsigned char addr);


/**
 @brief Issues a repeated start condition and sends address and transfer direction 

 @param   addr address and transfer direction of I2C device
 @retval  0 device accessible
 @retval  1 failed to access device
 */
extern unsigned char i2c_rep_start(unsigned char addr);


/**
 @brief Issues a start condition and sends address and transfer direction 
   
 If device is busy, use ack polling to wait until device ready 
 @param    addr address and transfer direction of I2C device
 @return   none
 */
extern void i2c_start_wait(unsigned char addr);

 
/**
 @brief Send one byte to I2C device
 @param    data  byte to be transfered
 @retval   0 write successful
 @retval   1 write failed
 */
extern unsigned char i2c_write(unsigned char data);


/**
 @brief    read one byte from the I2C device, request more data from device 
 @return   byte read from I2C device
 */
extern unsigned char i2c_readAck(void);

/**
 @brief    read one byte from the I2C device, read is followed by a stop condition 
 @return   byte read from I2C device
 */
extern unsigned char i2c_readNak(void);

/** 
 @brief    read one byte from the I2C device
 
 Implemented as a macro, which calls either @ref i2c_readAck or @ref i2c_readNak
 
 @param    ack 1 send ack, request more data from device<br>
               0 send nak, read is followed by a stop condition 
 @return   byte read from I2C device
 */
extern unsigned char i2c_read(unsigned char ack);
#define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak();

#endif /* TWI_PROTOCOL_H_ */