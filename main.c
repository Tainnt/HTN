#include <msp430g2553.h>
#include "functions.h"

#define TX BIT1
#define RX BIT2
#define TX_LED BIT0
#define RX_LED BIT6

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
    //P1OUT ^= RX_LED;
    while(!(IFG2 & UCA0TXIFG)) {}
    char s[]="";
    UartReadString(s);
    if(s[0]=='a' && s[1]=='b' && s[2]=='c')
        UartSendString("right\n");
    else
        UartSendString("wrong\n");
    //P1OUT ^= TX_LED;                                // Turn TX_LED on after successful transmission*/
}

/*
#pragma vector=PORT1_VECTOR
__interrupt void SWITCH_ISR()
{
    P1OUT |= BIT6;
    _delay_cycles(500000);
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = 0x41;                                           // Character A
    while(!(IFG2 & UCA0TXIFG));
    P1OUT &= ~BIT6;
    P1IFG &= ~BIT3;
}
*/
