/*
 * initProcedures.h
 *
 *  Created on: Nov 4, 2018
 *      Author: bjh885
 *
 * This file contains various routines for initializing ports such as
 *  enabling switch 1, switch 2, switch 1 and 2, LED enables, keypad enables, ect
 */

#ifndef GPIO_INIT_PROCEDURES_H_
#define GPIO_INIT_PROCEDURES_H_

void init_sw1();
void init_sw2();
void sw1_interrupt(int, int, int);
void sw2_interrupt(int, int, int, int);

#endif /* GPIO_INIT_PROCEDURES_H_ */
