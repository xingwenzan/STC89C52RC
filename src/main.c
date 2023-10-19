#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

// 矩阵按键端口
#define KEY_PORT P1

u8 out[] = "0\n\0"
           "1\n\0"
           "2\n\0"
           "3\n\0"
           "4\n\0"
           "5\n\0"
           "6\n\0"
           "7\n\0"
           "8\n\0"
           "9\n\0"
           "a\n\0"
           "b\n\0"
           "c\n\0"
           "d\n\0"
           "e\n\0"
           "f\n\0";
u8 point = -1;   // 按键坐标
u8 change = 0;

// 串口初始化
void InitUART() {
//    PCON = 1<<7;   // SMOD 为 1，当波特率 > 14400 时开启
    SCON = 0x40;   // SCON: 模式 1, 8-bit UART, 使能接收禁止
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit 重装
    TH1 = 0xFD;   // TH1:  重装值 9600 波特率 晶振 11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;   // TR1:  timer 1 打开
    EA = 1;   //打开总中断
    //ES = 1;   //打开串口中断
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u8 t) {
    for (int i = 0; i < t; ++i)
        for (int j = 0; j < 110; ++j);
}

// 发送一个字节
void SendByte(u8 dat) {
    SBUF = dat;
    while (!TI);
    TI = 0;
}

// 发送一个字符串
void SendStr(u8 *s) {
    while (*s != '\0')// \0 表示字符串结束标志，
        //通过检测是否字符串末尾
    {
        SendByte(*s);
        s++;
    }
}

/**
 * 按键读取函数，直接赋值到 x,y
 * 我的单片机高位是行(x) 低位是列(y)   7-4 对应 0-3 行   4-0 对应 0-3 行
 * 低电平接通
 */
void keyScan() {
    KEY_PORT = 0x0f;   // 高位发出信号
    if (KEY_PORT != 0x0f) {   // 有变化
        change = 1;
        delay(1000);   // 消抖
        for (u8 i = 0; i < 4; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                point = i * 4;
                break;
            }
        }
        KEY_PORT = 0xf0;   // 低位发出信号
        delay(1000);   // 消抖
        for (u8 i = 4; i < 8; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                point += (7 - i);
                break;
            }
        }
    }


}

int main() {

    InitUART();

    while (1) {
        keyScan();
//        SendStr("11");
        if (change) {
//            SendStr("1212121");
            change = 0;
            SendStr(out + point * 3);
        }

        DelayMs(240);//延时循环发送
        DelayMs(240);
    }
}