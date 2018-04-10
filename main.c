#include <msp430g2553.h>
#include "functions.h"

#define TX BIT1
#define RX BIT2
#define TX_LED BIT0
#define RX_LED BIT6
unsigned char buffer[32];
unsigned int i=0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                                                               // stop watchdog timer

    Config_Clock();
    Config_IO();
    Config_USCI();

    while(1)
    {
        Commander();
    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void RX_ISR()
{
    buffer[i]=UartReadChar();
    if(buffer[i]=='\n')
    {
        //UartSentChar(buffer[3]);
        buffer[1]='z';
        UartSendString(buffer);
        /*
        if(buffer[0]=='a' && buffer[1]=='b')
            UartSendString("right\n");
        else
            UartSendString("wrong\n");
        */
        i=0;
    }
    else
        i++;
}

