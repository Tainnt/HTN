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
    /*
    P1DIR &= BUTTON;                                        // P1.3: input
    P1REN |= BUTTON;                                        // Enable pull resistor on pin P1.3
    P1OUT |= BUTTON;                                        // Enable pull up resistor

    P1IFG &= 0x00;                                          // Clear all interrupt flags, just to make sure
    P1IES |= BUTTON;                                        // Edge select on BUTTON: HIGH to LOW transition
    P1IE |= BUTTON;                                         // Enable Interrupt
    */
}


void Test_Clock()
{
    /*
     * If you are not measuring execution time but just clock frequencies, then you can use ACLK and SMCLK output option:
     * Just put this function after the Stop-The-Watchdog instruction
     * Just test the frequency on P1.4 using oscilloscope ?? :D ??
     */
    BCSCTL1 = CALBC1_16MHZ;                                 // this frequency for example
    DCOCTL = CALDCO_16MHZ;
    P1DIR |= 0x13;                                          // P1.0,1 and P1.4 outputs
    P1SEL |= 0x11;                                          // P1.0,4 ACLK, SMCLK output

}

void UartSentChar(unsigned char byte)
{
    while(!(IFG2 & UCA0TXIFG)) {}
    UCA0TXBUF = byte;
}

void UartSendString(unsigned char s[])
{
    unsigned int i = 0;
    do
    {
        UartSentChar(s[i]);
        i++;
    }while(s[i]!='\0');                                 //check the string is done yet? ('\0' mean the char is null)
}

char UartReadChar()
{
    while (!(IFG2 & UCA0RXIFG)); // Doi gui xong ky tu truoc
    return UCA0RXBUF; // Moi cho phep nhan ky tu tiep theo
}

void UartReadString(char s[])
{
    unsigned int i = 0;
    do
    {
        if(s[i]!='\n')
        {
            s[i] = UartReadChar();
            i++;
        }
        else
            i++;
    }while(s[i]!='\0');
}

void Commander()
{
    //char *x;
    //UartSentChar('T');
    //UartSendString(ESP_check);
    //
    //UartSendString("abc\n");
    //_delay_cycles(1*8000000);
    //UartReadString(x);
   // UartSendString(x);

}

