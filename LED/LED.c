/*
 * rgb.c
 *
 *  Created on: Sep 28, 2018
 *      Author: Ben
 */

#include "rgb.h" /*For LED Flashing functions*/
#include "ports.h" /* For PORTX Definitions */

void flash_red() {

    /* Turn on */
    GPIO_PORTF_DATA |= 0x02;
    delay();
    GPIO_PORTF_DATA &= ~0x02;
    delay();
}

void flash_blue() {

    GPIO_PORTF_DATA |= 0x04;
    delay();
    GPIO_PORTF_DATA &= ~0x04;
    delay();
}

void flash_green() {

    GPIO_PORTF_DATA |= 0x08;
    delay();
    GPIO_PORTF_DATA &= ~0x08;
    delay();
}

void flash_green_stay() {

    GPIO_PORTF_DATA |= 0x08;
}

void turn_off_green() {

    GPIO_PORTF_DATA &= ~0x08;
}

void turn_white() {

    GPIO_PORTF_DATA |= 0x0E;
}

void flash_rgb() {

    flash_red();
    flash_green();
    flash_blue();
}

void flash_red_stay() {

    GPIO_PORTF_DATA |= 0x02;
}

void turn_off_red() {

    GPIO_PORTF_DATA &= ~0x02;
}

void delay () {
    long unsigned i;
    for (i = 0; i < 1000000; i++);
}
