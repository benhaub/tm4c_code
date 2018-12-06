#include "tm4c123gh6pm.h"
/*
 * rgb.c
 *
 *  Created on: Sep 28, 2018
 *      Author: Ben
 */

void init_LED() {

    SYSCTL_RCGC1_R |= 0x20; // p.421, activate UART1
    SYSCTL_RCGC2_R |= 0x20; // p.424, activate clock gating for Port F

    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
    GPIO_PORTF_AFSEL_R &= 0x0E;
}

void LED_delay() {
    long unsigned i;
    for (i = 0; i < 1000000; i++);
}

void flash_red() {

    /* Turn on */
    GPIO_PORTF_DATA_R |= 0x02;
    LED_delay();
    GPIO_PORTF_DATA_R &= ~0x02;
    LED_delay();
}

void flash_blue() {

    GPIO_PORTF_DATA_R |= 0x04;
    LED_delay();
    GPIO_PORTF_DATA_R &= ~0x04;
    LED_delay();
}

void flash_green() {

    GPIO_PORTF_DATA_R |= 0x08;
    LED_delay();
    GPIO_PORTF_DATA_R &= ~0x08;
    LED_delay();
}

void stay_green() {

    GPIO_PORTF_DATA_R |= 0x08;
}

void turn_off_green() {

    GPIO_PORTF_DATA_R &= ~0x08;
}

void turn_white() {

    GPIO_PORTF_DATA_R |= 0x0E;
}

void flash_rgb() {

    flash_red();
    flash_green();
    flash_blue();
}

void stay_red() {

    GPIO_PORTF_DATA_R |= 0x02;
}

void turn_off_red() {

    GPIO_PORTF_DATA_R &= ~0x02;
}
