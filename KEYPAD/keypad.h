/*
 * keypad.h
 *
 *  Created on: Dec 3, 2018
 *      Author: bjh885
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

void init_keypad();
unsigned int scan_key();
void init_keypadAB();
unsigned int scan_keyAB();
void init_keypadDE();
unsigned int scan_keyDE();




#endif /* KEYPAD_H_ */
