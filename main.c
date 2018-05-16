#include <msp430.h>
#include "functions.h"

char ReceivedPos[20]="";                                    // mảng chứa các tọa độ nhận được
unsigned int j=0;                                           // biến đếm của mảng ReceivedPos
int Mode=1;                                                 // Mode 0: nhận chuỗi 6 tọa độ, Mode 1: nhận các chuỗi khác
int RcvPos=0;
char ReceivedString[100]="";                                // mảng kí tự nhận được từ esp8266
unsigned int i=0;                                           // biến đếm cho mảng ReceivedString
int EndString=0;                                            // biến trạng thái xác định chuỗi đã kết thúc
int RSSI[3]={-70,-57,-54};                                  // mảng chứa rssi tương ứng với 3 tên wifi
char* SSID[3]={"EmbeddedSystem","ES_02","ES_03"};           // mảng kí tự lưu tên 3 wifi cần lấy thông tin
char *NetworkSSID="hahaha";
char *NetworkPW="987abc123";
char *IP="192.168.100.24";
char *Port="5000";
//char* test="+IPD,24:420,840,210,0,180,690.";

int main(void)
{
    int n;                                              // biến đếm cho vòng lặp lấy 3 rssi
    int rssi_1[3];                                      // mảng chứa 3 giá trị của wifi 1
    int rssi_2[3];                                      // mảng chứa 3 giá trị của wifi 2
    int rssi_3[3];                                      // mảng chứa 3 giá trị của wifi 3
    WDTCTL = WDTPW + WDTHOLD;                           // Stop WDT
    Config_Clock();                                     // thiết lập xung cho hệ thống
    Config_Pins();                                      // thiết lập các ngõ ra vào cho hệ thống
    Config_USCI();                                      // cài đặt cho chế độ uart
    UART_SendString("AT+CWLAPOPT=1,6\r\n");             // cài đặt chế độ để 8266 chỉ trả về tên wifi và rssi
    _delay_cycles(10000);
    P1OUT|=BIT0;
    ConnectTCPSever(NetworkSSID,NetworkPW,IP,Port);     // kết nối với tcp server
    Mode=0;                                             // mode 0: nhận chuỗi 6 tọa độ
    UART_SendString("READY to RECEIVE");
    while(!RcvPos) {}                                   // đợi đến khi nhận xong chuỗi 6 tọa độ
    SplitString(ReceivedPos);                           // cắt chuỗi và lấy ra 6 tọa đồ cần thiết
    for(n=0;n<3;n++)
    {
        SendCommand(SSID[0]);                       // gửi lệnh AT với tên wifi thứ nhất
        while(!EndString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_1[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ nhất
        EndString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[0]=avr(rssi_1);

    for(n=0;n<3;n++)
    {
        SendCommand(SSID[1]);                       // gửi lệnh AT với tên wifi thứ hai
        while(!EndString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_2[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ hai
        EndString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[1]=avr(rssi_2);

    for(n=0;n<3;n++)
    {
        SendCommand(SSID[2]);                       // gửi lệnh AT với tên wifi thứ ba
        while(!EndString) {}                        // kiểm tra đã hoàn tất nhận chuỗi từ 8266
        rssi_3[n] = Get_RSSI(ReceivedString);       // lấy rssi tương ứng với tên wifi thứ ba
        EndString=0;                                // set lại biến kết thúc chuỗi để bắt đầu nhận chuỗi mới
    }
    RSSI[2]=avr(rssi_3);

    CalculatPosition(RSSI);                         // hàm tính toán vị trí
    SendPosition();                                 // hàm gửi lại tọa độ cho server
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
    if(!EndString)                                  // kiểm tra biến trạng thái chuỗi trước đó đã kết thúc chưa
    {
        if (Mode)
        {
            ReceivedString[i]=UART_GetChar();       // bắt đầu nhận từng kí tự từ UCA0RXBUF
            if(ReceivedString[i]==')')              // nếu kí tự là ')' thì đã kết thúc chuỗi
            {
                i=0;                                // set lại biến đếm cho mảng ReceivedString
                EndString=1;                        // set biến trạng thái đã kết thúc chuỗi
            }
            else
                i++;                                // nếu kí tự khác ')' thì tiếp tục nhận kí tự tiếp theo
            if(i==100)
                i=0;                                // nếu i=100 set lại biến i=0 để tránh tràn mảng ReceivedString
        }
        else
        {
            ReceivedPos[j]=UART_GetChar();
            if (ReceivedPos[j]=='.')                // kiểm tra nếu đến dấu '.' là kết thúc chuỗi 6 tọa độ
            {
                RcvPos=1;                           // bật cờ thông báo đã nhận xong chuỗi 6 tọa độ
                Mode=1;                             // chuyển về mode 1 để uart nhận các chuỗi khác
            }
            j++;
        }
    }
 }
