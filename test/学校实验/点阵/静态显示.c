//
// Created by 86159 on 2023-10-26.
//
#include"mcs51/reg51.h"    //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义

typedef unsigned int u16;
typedef unsigned char u8;

const u8 segout[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //8列
const u8 tab[] = {0xB9, 0xA9, 0xEE, 0x10, 0xFF, 0x39, 0x54, 0x93};  // 图案

// 硬件端口定义
// 级联端信号控制
#define LATCH_A P1_0
#define SRCLK_A P1_1
#define SER_A P1_2

// 公共端信号控制
#define LATCH_B P2_2
#define SRCLK_B P2_1
#define SER_B P2_0

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u8 t) {
    for (u8 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}

// 发送字节程序
void SendByte(u8 dat) {
    for (u8 i = 0; i < 8; i++) {
        SRCLK_A = 0;
        SER_A = dat & 0x80;
        dat <<= 1;
        SRCLK_A = 1;
    }

}

// 发送双字节程序
// 595级联，n个595，就需要发送n字节后锁存
void Send2Byte(u8 dat1, u8 dat2) {
    SendByte(dat1);
    SendByte(dat2);
}

// 595锁存程序
// 595级联发送数据后，锁存有效
void Out595() {
    LATCH_A = 1;
    delay(1);
    LATCH_A = 0;
}

//  发送位码字节程序
// 使用另外一片单独595
void SendSeg(u8 dat) {
    for (u8 i = 0; i < 8; i++)  //发送字节
    {
        SRCLK_B = 0;
        SER_B = dat & 0x80;
        dat <<= 1;
        SRCLK_B = 1;
    }
    LATCH_B = 1;    //锁存
    delay(1);
    LATCH_B = 0;
}

int main() {
    while (1) {
        for (u8 i = 0; i < 8; i++) //8列显示
        {
            SendSeg(segout[i]);

            Send2Byte(~tab[i], 0xff);   // 红色
//            Send2Byte(0xff, ~tab[i]);   // 绿色
//            Send2Byte(~tab[i], ~tab[i]);   // 双色
            Out595();
            DelayMs(1);

            Send2Byte(0xff, 0xff);//防止重影
            Out595();
        }
    }
}
