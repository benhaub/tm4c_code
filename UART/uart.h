/*
 * uart.h
 *
 *  Created on: Nov 30, 2018
 *      Author: bjh885
 */

#ifndef UART_H_
#define UART_H_

void UART1_init(int);
void UART1_send(unsigned char);
unsigned char UART1_recieve(void);
unsigned char ToUpperCase(unsigned char);

#endif /* UART_H_ */
