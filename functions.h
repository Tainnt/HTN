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
int Get_RSSI(char* ReceivedString);
void SendCommand(char* SSID);

#endif /* FUNCTIONS_H_ */
