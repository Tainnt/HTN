#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void Config_Clock();
void Config_Pins();
void Config_USCI();

void UART_SendChar(unsigned char byte);
void UART_SendString(char* s);
void UART_SendInt(unsigned long n);
void UART_SendFloat(float x, unsigned char length);
char UART_GetChar();
int Get_RSSI(char* ReceivedString);
void SendCommand(char* SSID);
float distance(int rssi);
float distance_meter(int RSSI_dB);
float x_oject(float d1, float d2, float x3);
float y_oject(float d1, float d3, float x3, float y3, float x_oject);
void CaculatePosition(int RSSI[3],int one[2],int two[2],int three[2]);
double pow(int a,int b);

#endif /* FUNCTIONS_H_ */
