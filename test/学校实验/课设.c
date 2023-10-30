//
// Created by 86159 on 2023-10-30.
//
#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

#define motor P1   // 电机引脚
#define key P1_7   // 上锁/密码输入确认引脚

const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
const u8 password[] = "123456\0";

u8 input[7], idx = 0;   // 输入读入 输入位
u8 lock = 0;   // 开关状态   0 锁未开启，即开门   1 锁开启，即锁门

// 串口初始化
void InitUART(u8 p, u8 st) {
    PCON = p << 7;   // SMOD 为 p，当波特率 > 14400 时开启
    SCON = 0x50;   // SCON: 模式 1, 8-bit UART, 使能接收禁止
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit 重装
    TH1 = st;   // TH1:  重装值 9600 波特率 晶振 11.0592MHz
    TL1 = st;
    TR1 = 1;   // TR1:  timer 1 打开
    EA = 1;   //打开总中断
    ES = 1;   //打开串口中断
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u16 t) {
    for (u8 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
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

// 开启锁，即锁门
void turn_on() {
    if (lock) {   // 锁已经打开，即已上锁
        SendStr("已上锁，无须重复操作\r\n\0");
    } else {   // 未上锁，需上锁
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            DelayMs(50);
        }
        motor = ~0;
        SendStr("成功上锁\r\n\0");
    }
}

// 关闭锁，即开门
void turn_off() {
    if (lock) {    // 已上锁，需要解锁
        lock = 0;
        for (u8 i = 0; i < 8; i++) {
            motor = fan[i];
            DelayMs(50);
        }
        motor = ~0;
        SendStr("成功开锁\r\n\0");
    } else {   // 未上锁，无需解锁
        SendStr("已开锁，无须重复操作\r\n\0");
    }
}

u8 judge() {
    u8 i = 0;
    while (i < 6) {
        if (input[i] == password[i]) {
            input[i] = '\0';
//            input[i] = '0';
            i++;
        } else break;
    }
    return i == 6;
}

int main() {
    InitUART(0, 0xFD);
    input[6] = '\0';
    while (1) {
        // 按下按键，密码确认，正确则开门，错误/无输入则锁门
        if (!key) {
            idx = 0;
//            SendStr(password);
            if (judge()) {
//                SendStr("密码正确\r\n\0");
                turn_off();
            } else {
//                SendStr("密码错误/无输入\r\n\0");
                turn_on();
            }
            DelayMs(200);
        }
    }
}

void UART_SER(void)

__interrupt(4) //串行中断服务程序
{

if(RI)                        //判断是接收中断产生
{
input[idx++] =
SBUF;                 //读入缓冲区的值
idx%=6;
RI = 0;
}

}