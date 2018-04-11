#include <msp430.h>
#include "functions.h"
float dis[3];
float x=0,y=0;
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
char* Get_AP1(char* ReceivedString, char* AP1_SSID, int length)
{
    char AP1[2];                                            // 2 elements: [0] -> Index Number, [1] -> RSSI
    AP1[0] = '1';                                           // Just assume it to 1, no need to care about its SSID anymore

    unsigned int i = 7;                                     // +CWLAP: takes 7 spaces, from 0 to 6

    while(ReceivedString[i] != '\0')                        // search throughout the entire received string
    {
        if(ReceivedString[i] == '(' )                       // searching from left to right, if '(' is met for the first time, then start comparing
        {
            do
            {
                int j;
                for(j = 0; j < length; j++)
                {
                    if(ReceivedString[i+2] == AP1_SSID[j])  // +2 from the '(' (+1 for '"', +1 more for the starting character of that SSID)
                        continue;
                    break;                                  // break out of comparing loop
                }
                if(j == length)                             // it perfectly matches the AP1_SSID
                {
                    i += 3;                                 // get through ", to get to the RSSI without the minus mark
                    int tmp_ssid = (ReceivedString[i] - '0')*10 + (ReceivedString[i+1] - '0');
                                                            // convert the RSSI from character to int
                    AP1[1] = -tmp_ssid;                     // then assign to AP1[1]
                    return AP1;                             // finish getting AP's index number and its RSSI
                }
                else                                        // not match
                {
                    break;                                  // go for the next SSID in the ReceivedString
                }
            }while(ReceivedString[i] != ')');               // stop comparing if ) is met for the second time
        }
        while(ReceivedString[i] != '\n')                    // at this line, it means that comparing is interrupted at the middle,
        {                                                   // so, I i++ until the present character is '\n', which is the start of the next SSID
            i++;
        }
        i += 7;                                             // i += 7 to get through the "+CWLAP:"
    }

    return AP1;
}
*/
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
/*
 * power function
 */
double pow(int a,int b)
{
    int i,result=1;
    for(i=0;i<b;i++)
    {
        result=result*a;
    }
    return result;
}

//  function    :   find position
//  3 wifi device: W1(0,0), W2(x2, 0), W3(x3, y3)
//  Oject: M(x, y)

float distance(int rssi)
{
    //  rssi1m = +25-62 = -37 | n = 2.7 -> 4.3
    int rssi1m = -37;
    int n = 2;
    return (pow( 10, (rssi1m - rssi) / (10*n) ));
}
/*
float distance_meter(int RSSI_dB)
{
    freqMHz = 2400;
    Exp = (1.0/20) * ( 27.55 - 20*log10(freqMHz) - RSSI_dB)
    return (pow( 10, Exp ));
}
*/
// x (Oject)
float x_oject(float d1, float d2, float x3)
{
    return ( (pow(d1, 2) - pow(d2, 2) + pow(x3, 2)) / (2*x3));
}
// y (Oject)
float y_oject(float d1, float d3, float x3, float y3, float Xoject)
{
    return ( ( ( pow(d1,2) - pow(d3,2) + pow(x3,2) + pow(y3,2) ) / (2*y3) ) - (Xoject*x3/y3) );
}

void CaculatePosition(int RSSI[3],int one[2],int two[2],int three[2])
{
    int i;
    for (i = 0; i < 3; i++)
    {
        dis[i]=distance(RSSI[i]);
    }
    x=x_oject(dis[1], dis[2], three[0]);
    y=y_oject(dis[1], dis[3], three[0], three[1], x);
    UART_SendChar('[');
    UART_SendFloat(x, 2);
    UART_SendChar(',');      //kí tự dấu phẩy
    UART_SendFloat(y, 2);
    UART_SendChar(']');
}
