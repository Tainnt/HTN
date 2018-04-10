#include <msp430.h>

#include "functions.h"
#define SSID_1 "hahaha"
#define SSID_2 "Redmi"
#define SSID_3 "Tuongvypro"
char* SSID[3]={"hahaha","Redmi","Tuongvypro"};
char ReceivedString[100];
unsigned int i=0;
unsigned int a=0;
int RSSI[3];
int endString=0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Config_Clock();
    Config_Pins();
    Config_USCI();

    //UART_SendString("AT\r\n");
    UART_SendString("AT+CWLAPOPT=1,6\r\n");
    _delay_cycles(1000000);

    //SendCommand(SSID_1);
    SendCommand(SSID[0]);
    while(!endString) {}
    RSSI[0] = Get_RSSI(ReceivedString);
    endString=0;

    SendCommand(SSID[1]);
    while(!endString) {}
    RSSI[1] = Get_RSSI(ReceivedString);
    endString=0;

    SendCommand(SSID[2]);
    while(!endString) {}
    RSSI[2] = Get_RSSI(ReceivedString);
    endString=0;

}


//  Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(!endString)
    {
        ReceivedString[i]=UART_GetChar();
        if(ReceivedString[i]==')')
        {
            //UART_SendChar(buffer[3]);
            //ReceivedString[1]='z';
            //UART_SendString(ReceivedString);
            /*
            if(ReceivedString[0]=='a' && ReceivedString[1]=='b')
               UART_SendString("right\n");
            else
               UART_SendString("wrong\n");
            */
            i=0;
            //RSSI[0] = Get_RSSI(ReceivedString);
            endString=1;
        }
        else
            i++;
    }
 }
