//
// Created by 86159 on 2023-10-21.
//
#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

// 矩阵按键端口
#define KEY_PORT P1

const u8 out[] = "这是实验\r\n\0"   // 11(下一个的起始索引)
                 "的例程\r\n\0"   // 20
                 "具体原理是\r\n\0"   // 33
                 "对指针的应用\r\n\0"   // 48
                 "这一段的起始指针地址\r\n\0"   // 71
                 "是整个字符串的初始地址\r\n\0"   // 96
                 "加上这段之前的所有部分字符串的长度\r\n\0"   // 133
                 "就是该部分字符串的起始地址\r\n\0"   // 162
                 "注意：\r\n\0"   // 171
                 "中文占两个大小的长度\r\n\0"   // 194
                 "让我们试一试吧\r\n\0"   // 211
                 "按下按键\r\n\0"   // 222
                 "应该不会有错误的显示\r\n\0"   // 245
                 "好了，就这样\r\n\0"   // 260
                 "介绍完成\r\n\0"   // 271
                 "自己改一改加深印象吧\r\n\0";
const u16 idx[16] = {0, 11, 20, 33, 48, 71, 96, 133,
                     162, 171, 194, 211, 222, 245, 260, 271};   // 为方便不同长度字段选择设置
u8 point = -1;   // 按键坐标
u8 change = 0;


// 串口初始化
void InitUART(u8 p, u8 st) {
    PCON = p << 7;   // SMOD 为 p，当波特率 > 14400 时开启
    SCON = 0x40;   // SCON: 模式 1, 8-bit UART, 使能接收禁止
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit 重装
    TH1 = st;   // TH1:  重装值 9600 波特率 晶振 11.0592MHz
    TL1 = st;
    TR1 = 1;   // TR1:  timer 1 打开
    EA = 1;   //打开总中断
//    ES = 1;   //打开串口中断
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u8 t) {
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

/**
 * 按键读取函数，直接赋值到 x,y
 * 我的单片机高位是行(x) 低位是列(y)   7-4 对应 0-3 行   4-0 对应 0-3 行
 * 低电平接通
 */
void keyScan() {
    KEY_PORT = 0x0f;   // 高位发出信号
    if (KEY_PORT != 0x0f) {   // 有变化
        change = 1;   // 有按键按下去
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

    InitUART(0, 0xFD);

    while (1) {
        keyScan();
        if (change) {
            change = 0;
            SendStr(out + idx[point]);
        }

        DelayMs(100);   // 太快了，延时保证 1 按 1 输出
    }
}