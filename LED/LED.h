/*
 * rgb.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Ben
 */

#ifndef RGB_H_
#define RGB_H_

/*
 * Flashes the red LED, delays, then turns off
 */
void flash_red();

/*
 * Turns on the red LED, but does not shut it off
 */
void flash_red_stay();

void turn_off_red();

void flash_green();

void flash_green_stay();

void turn_off_green();

void flash_blue();

void turn_white();

void flash_rgb();

void delay ();



#endif /* RGB_H_ */
