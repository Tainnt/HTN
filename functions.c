/*
 * functions.c
 *
 *  Created on: Apr 2, 2018
 *      Author: Cong Phase
 */
#include "functions.h"
#include <msp430g2553.h>

#define TX BIT1
#define RX BIT2
#define TX_LED BIT0
#define RX_LED BIT6
#define BUTTON BIT3
#define ESP_check "AT"
#define ESP_getInfo "AT+CWLAP"
unsigned char s[4]="a12";
void Config_Clock()
{
    if(CALBC1_8MHZ == 0xFF)
    {
        while(1);
    }
    DCOCTL = 0;                                             // Just make a reset on this reg.
    BCSCTL1 = CALBC1_8MHZ;                                 // Set range
    DCOCTL = CALDCO_8MHZ;                                  // Set DCO step and modulation
    BCSCTL2 |= SELM_0;                                      // MCLK Source Select 0: DCOCLK
}

void Config_USCI()
{
    P1SEL |= TX + RX;                                        // Secondary peripheral module function is selected.
    P1SEL2 |= TX + RX;                                       //                        ...                      .
    UCA0CTL1 |= UCSWRST;                                    // Hold at reset state for USCI configuration. Already RST with PUC, but still do this
    UCA0CTL0 = 0x00;                                        // Parity disabled, Odd parity, LSB first, 8bit, 1 stop bit, UART mode, Asynchronous
    UCA0CTL1 |= UCSSEL_2;                                   // USCI 0 Clock Source: SMCLK
    UCA0MCTL = UCBRF_1 | UCBRS_0 | UCOS16;                  // For getting 9600 baudrate at 8MHZ
    UCA0BR0 = 52;                                           //
    UCA0BR1 = 0;                                            // ...
    UCA0CTL1 &= ~UCSWRST;                                   // Clear reset bit to use USCI with UART configurations
    IE2 |= UCA0RXIE;                                        // Enable RX interrupt
    _BIS_SR(GIE);
}

void Config_IO()
{
    P1OUT = 0x00;
    P1DIR = TX_LED + RX_LED;                                // P1.6 and P1.0: outputs
}


void UartSentChar(unsigned char data)
{
    while (!(IFG2&UCA0TXIFG)); // Doi gui xong ky tu truoc
    UCA0TXBUF= data; // Moi cho phep gui ky tu tiep theo
}

void UartSendString(unsigned char s[])
{
    unsigned int i = 0;
    do
    {
        UartSentChar(s[i]);
        i++;
    }while(s[i]!='\0');          //check the string is done yet? ('\0' mean the char is end)
    //UartSentChar('\r');
    //UartSentChar('\n');
    UartSentChar('\0');
}

char UartReadChar()
{
    while (!(IFG2 & UCA0TXIFG)); // Doi gui xong ky tu truoc
    return UCA0RXBUF; // Moi cho phep nhan ky tu tiep theo
}


void Commander()
{
    //char *x;
    //UartSentChar('T');
    //UartSendString(ESP_check);
    //unsigned char p[]="abc";
    //UartSendString(p);
    //_delay_cycles(1*8000000);
   // UartSendString(x);
}

void Commander2()
{
    if(s[0]=='a' && s[1]=='1' && s[2]=='2')
        UartSendString("Right\n");
    else
        UartSendString("Wrong\n");
}

