#include <string.h>
#include <stdlib.h>

#include"ST7735.h"
/*
 * LCD.h
 *
 *  Created on: Nov 4, 2018
 *      Author: bjh885
 *
 * Specialized or commonly used functions that make use of the ST7735 functions
 * by wrapping them in other procedures. You can find things in here like
 * writing a string to the display, which makes use of drawChar()
 */

#ifndef LCD_H_
#define LCD_H_

unsigned char hex2char(unsigned int hex);
void writeLCD(char *, size_t, int, unsigned int, unsigned int, int);
void clearLCDLines(size_t, int);
void LCDinit(unsigned int, int);
void display2dgNum(int, int, int, unsigned int, unsigned int, int);
void display12BitHex(int, int, int, unsigned int, unsigned int, int);

#endif /* LCD_H_ */
