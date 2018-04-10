/*
 * functions.h
 *
 *  Created on: Apr 5, 2018
 *      Author: Cong Phase
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void Config_Clock();
void Config_Pins();
void Config_USCI();

void UART_SendString(char* s);
void UART_SendChar(unsigned char byte);
char UART_GetChar();
void UART_SaveString(char* string);

int Get_RSSI(char* ReceivedString);
void Commander();
void SendCommand(char* SSID_1);

#endif /* FUNCTIONS_H_ */
