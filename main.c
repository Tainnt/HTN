#include <msp430.h>
#include "functions.h"

char* SSID[3]={"hahaha","Tuongvypro","Redmi"};      //mảng kí tự lưu tên 3 wifi cần lấy thông tin
char ReceivedString[100];                           //mảng kí tự nhận được từ esp8266
unsigned int i=0;                                   //biến đếm cho mảng ReceivedString
int RSSI[3];                                        //mảng chứa rssi tương ứng với 3 tên wifi
int endString=0;                                    //biến trạng thái xác định chuỗi đã kết thúc

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Config_Clock();
    Config_Pins();
    Config_USCI();

    //UART_SendString("AT\r\n");
    UART_SendString("AT+CWLAPOPT=1,6\r\n");  //cài đặt chế độ để 8266 chỉ trả về tên wifi và rssi
    _delay_cycles(1000000);

    SendCommand(SSID[0]);                   //gửi lệnh AT với tên wifi thứ nhất
    while(!endString) {}                    //kiểm tra đã hoàn tất nhận chuỗi từ 8266
    RSSI[0] = Get_RSSI(ReceivedString);     //lấy rssi tương ứng với tên wifi thứ nhất
    endString=0;                            //set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới

    SendCommand(SSID[1]);                   //gửi lệnh AT với tên wifi thứ hai
    while(!endString) {}                    //kiểm tra đã hoàn tất nhận chuỗi từ 8266
    RSSI[1] = Get_RSSI(ReceivedString);     //lấy rssi tương ứng với tên wifi thứ hai
    endString=0;                            //set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới

    SendCommand(SSID[2]);                   //gửi lệnh AT với tên wifi thứ ba
    while(!endString) {}                    //kiểm tra đã hoàn tất nhận chuỗi từ 8266
    RSSI[2] = Get_RSSI(ReceivedString);     //lấy rssi tương ứng với tên wifi thứ ba
    endString=0;                            //set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới

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
    if(!endString) //kiểm tra biến trạng thái chuỗi trước đó đã kết thúc chưa
    {
        ReceivedString[i]=UART_GetChar();       //bắt đầu nhận từng kí tự từ UCA0RXBUF
        if(ReceivedString[i]==')')              //nếu kí tự là ')' thì đã kết thúc chuỗi
        {
            i=0;                                //set lại biến đếm cho mảng ReceivedString
            endString=1;                        //set biến trạng thái đã kết thúc chuỗi
        }
        else
            i++;                                //nếu kí tự khác ')' thì tiếp tục nhận kí tự tiếp theo
    }
 }
