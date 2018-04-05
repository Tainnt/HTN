/*
 * functions.h
 *
 *  Created on: Apr 2, 2018
 *      Author: Cong Phase
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void Config_Clock();
void Config_USCI();
void Config_IO();
void Test_Clock();
void UartSentChar(unsigned char s);
void UartSendString(unsigned char s[]);
char UartReadChar();
void UartReadString(char s[]);
void Commander();

#endif /* FUNCTIONS_H_ */