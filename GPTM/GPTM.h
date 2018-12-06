/*
 * GPTM.h
 *
 *  Created on: Nov 21, 2018
 *      Author: bjh885
 */

#ifndef GPTM_H_
#define GPTM_H_

void init_timer0B_PWMperiodic(unsigned int, uint32_t, unsigned int, int);
void init_timer0B_PWMoneShot(unsigned int, uint32_t, unsigned int, int);
void init_timer0B_oneShot(int, int, int, int, int);
void init_timer0B_periodic(int, int, int, int, int);

#endif /* GPTM_H_ */
