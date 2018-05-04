#include <msp430.h>
#include "functions.h"

//char* SSID[3]={"WATERFOUNTAINS","AI-THINKER_ACA782","ESP_ACA5B3"};            // mảng kí tự lưu tên 3 wifi cần lấy thông tin
char* SSID[3]={"hahaha","Tuongvypro","Ngoc Anh"};
char ReceivedString[100];                                   // mảng kí tự nhận được từ esp8266
unsigned int i=0;                                           // biến đếm cho mảng ReceivedString
int endString=0;                                            // biến trạng thái xác định chuỗi đã kết thúc
int RSSI[3]={-71,-60,-54};                                                // mảng chứa rssi tương ứng với 3 tên wifi
char *NetworkSSID="AI-THINKER_ACA782";
char *NetworkPW="12345678";
char *IP="192.168.4.1";
char *Port="5000";


int main(void)
{
    int n;
    int rssi_1[3];
    int rssi_2[3];
    int rssi_3[3];
    float w1[2]={420,840};                                        // mảng vị trí wifi thứ nhất
    float w2[2]={210,0};                                      // mảng vị trí wifi thứ hai
    float w3[2]={180,690};                                    // mảng vị trí wifi thứ ba
    WDTCTL = WDTPW + WDTHOLD;                           // Stop WDT
    Config_Clock();                                     // Thiết lập xung cho hệ thống
    Config_Pins();                                      // Thiết lập các ngõ ra vào cho hệ thống
    Config_USCI();                                      // Cài đặt cho chế độ uart
    UART_SendString("AT+CWLAPOPT=1,6\r\n");             // Cài đặt chế độ để 8266 chỉ trả về tên wifi và rssi
    _delay_cycles(10000);
    P1OUT|=BIT0;
    /*
    for(n=0;n<3;n++)
    {
        SendCommand(SSID[0]);                       // gửi lệnh AT với tên wifi thứ nhất
        while(!endString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_1[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ nhất
        endString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[0]=avr(rssi_1);

    for(n=0;n<3;n++)
    {
        SendCommand(SSID[1]);                       // gửi lệnh AT với tên wifi thứ hai
        while(!endString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_2[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ hai
        endString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[1]=avr(rssi_2);

    for(n=0;n<3;n++)
    {
        SendCommand(SSID[2]);                       // gửi lệnh AT với tên wifi thứ ba
        while(!endString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_3[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ ba
        endString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[2]=avr(rssi_3);
    */
    CalculatPosition(RSSI,w1[0],w1[1],w2[0],w2[1],w3[0],w3[1]);   // hàm tính toán vị trí và xuất ra trên serial

    SendPosition(NetworkSSID,NetworkPW,IP,Port);
    P1OUT&=~BIT0;
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
    if(!endString)                              // kiểm tra biến trạng thái chuỗi trước đó đã kết thúc chưa
    {
        ReceivedString[i]=UART_GetChar();       // bắt đầu nhận từng kí tự từ UCA0RXBUF
        if(ReceivedString[i]==')')              // nếu kí tự là ')' thì đã kết thúc chuỗi
        {
            i=0;                                // set lại biến đếm cho mảng ReceivedString
            endString=1;                        // set biến trạng thái đã kết thúc chuỗi
        }
        else
            i++;                                // nếu kí tự khác ')' thì tiếp tục nhận kí tự tiếp theo
    }
 }


