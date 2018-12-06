#include <inttypes.h>
/*
 * adc.h
 *
 *  Created on: Nov 9, 2018
 *      Author: bjh885
 */

#ifndef ADC_H_
#define ADC_H_

void init_adc0(unsigned int, unsigned int, unsigned int);
uint32_t ADC0_InSeq3(void);
void init_adc1(unsigned int, unsigned int, unsigned int);
uint32_t ADC0_InSeq2(void);
void ADC0_interrupt(int, int, int, int, int, int, int, int, int, int);


#endif /* ADC_H_ */
