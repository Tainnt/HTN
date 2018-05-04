#include <msp430.h>
#include "functions.h"
#include <math.h>

float dis[3];
int x=0;
int y=0;

void Config_Clock()
{
    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
    {
        while(1);                               // do not load, trap CPU!!
    }

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
}

//***********************************************************************************************************
void Config_Pins()
{
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1DIR |= BIT0 +BIT6;
    P1OUT &= ~(BIT0 +BIT6);
}

//***********************************************************************************************************

void Config_USCI()
{
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt ||||| bring this line to main.c directly
    _BIS_SR(GIE);
}

//***********************************************************************************************************
void UART_SendChar(unsigned char byte)
{
    while(!(IFG2 & UCA0TXIFG)) {}
    UCA0TXBUF = byte;
}

//***********************************************************************************************************
void UART_SendString(char* s)
{
    unsigned int i = 0;
    do
    {
        UART_SendChar(s[i]);
        i++;
    }while(s[i]!='\0');                                 //check the string is done yet? ('\0' mean the char is null)
}

//***********************************************************************************************************
void UART_SendInt(unsigned long n)
{
    unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
         UART_SendChar('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
         UART_SendChar(buffer[j]);
     }
}

//***********************************************************************************************************
void UART_SendFloat(float x, unsigned char length)
{
    unsigned long temp;
    if(length>4)length=4;
    // de trong 1 ki tu o dau cho hien thi dau
    if(x<0)
    {
        UART_SendChar('-');           //In so am
        x*=-1;
    }
    temp = (unsigned long)x;          // Chuyan thanh so nguyen.

    UART_SendInt(temp);

    x=((float)x-temp);//*mysqr(10,coma);
    if(length!=0)UART_SendChar('.');    // In ra dau "."
    while(length>0)
    {
        x*=10;
        length--;
    }
    temp=(unsigned long)(x+0.5);    //Lam tron
    UART_SendInt(temp);
}

//***********************************************************************************************************
char UART_GetChar()
{
    while (!(IFG2 & UCA0RXIFG)); // Doi nhan xong ky tu truoc
    return UCA0RXBUF;           // Moi cho phep nhan ky tu tiep theo
}

//***********************************************************************************************************
/*
 * Get RSSI from ReceivedString
 */
int Get_RSSI(char* ReceivedString)
{
    unsigned int i = 8;                                     // the index of the first '"'
    while( !( (ReceivedString[i] == ',') && (ReceivedString[i+1] == '-') ) )
    {
        i++;
    }
    i += 2;                                                 // i += 2 to get to the RSSI (skip the minus character)

    int RSSI = (ReceivedString[i] - '0')*10 + (ReceivedString[i + 1] - '0');
    RSSI = 0 - RSSI;                                        // revert to get the minus value

    return RSSI;
}
//***********************************************************************************************************
/*
 *send AT command from SSID
 */
void SendCommand(char* SSID)
{
    UART_SendString("AT+CWLAP=\"");
    UART_SendString(SSID);
    UART_SendString("\"\r\n");
}
//***********************************************************************************************************
int avr(int rssi[])
{
    float result=0;
    int i;
    for (i = 0; i < 3; i++)
    {
        result += rssi[i];
    }
    result = roundf(result/3);
    return result;
}
//***********************************************************************************************************
//  function    :   find position
//  3 wifi device: W1(0,0), W2(x2, 0), W3(x3, y3)
//  Oject: M(x, y)

float distance(int rssi)
{
    //  rssi1m = +25-62 = -37 | n = 2.7 -> 4.3
    int rssi1m = -37;
    int n = 4;
    return (powf( 10, (rssi1m - rssi)*1.0 / (10.0*n) ));
}
float distance_V2(int RSSI)
{
    float exp = (1.0/20.0) * (27.55 - 20.0*log10(2400.0) - RSSI);
    return powf(10, exp);
}
// w1(0;0)      -> d1
// w2(a;b)      -> d2
// w3(c;d)      -> d3
// Object(x;y)
void CalculatPosition(int RSSI[],float m,float n,float a,float b,float c,float d)
{
    float d12=0;
    float d13=0;
    float ab=0;
    float cd=0;
    float y1=0;
    float y2=0;
    int i;
    for (i = 0; i < 3; i++)
    {
        dis[i]=distance(RSSI[i]);
    }
    a =a-m;
    b =b-n;
    c =c-m;
    d =d-n;

    d12   = dis[0]*dis[0] - dis[1]*dis[1];
    d13   = dis[0]*dis[0] - dis[2]*dis[2];
    ab    = powf(a,2) + powf(b,2);
    cd    = powf(c,2) + powf(d,2);

    y1 = 0.5/(b*c - d*a);
    y2 = c*(d12 + ab) - a*(d13 + cd);
    y = y1*y2;
    x = (0.5/c) * (d13 + cd - 2.0*y*d) + m;
    y= y+n;
    UART_SendChar('[');
    UART_SendInt(x);
    UART_SendChar(',');
    UART_SendInt(y);
    UART_SendChar(']');
}

void SendPosition(char* SSID, char* PW, char* IP,char* port)
{
   UART_SendString("AT+CWJAP=\"");
   UART_SendString(SSID);
   UART_SendString("\",\"");
   UART_SendString(PW);
   UART_SendString("\"\r\n");
   _delay_cycles(10000000);
   UART_SendString("AT+CIPSTART=\"TCP\",\"");
   UART_SendString(IP);
   UART_SendString("\",");
   UART_SendString(port);
   UART_SendString("\r\n");
   _delay_cycles(2000000);
   UART_SendString("AT+CIPSEND=10\r\n");
   _delay_cycles(2000000);
   UART_SendInt("Nhom 0xff,");
   UART_SendInt(x);
   UART_SendChar(',');
   UART_SendInt(y);
   _delay_cycles(2000000);
   UART_SendString("AT+CIPCLOSE\r\n");
}
