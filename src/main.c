#include"mcs51/reg51.h"    //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义

typedef unsigned int u16;
typedef unsigned char u8;

const u8 segout[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //8列
const u8 tab[] = {
        0x00, 0x7F, 0x49, 0x49, 0x49, 0x7F, 0x00, 0x00,   // 日
        0x77, 0x11, 0x11, 0xFF, 0x11, 0x11, 0x77, 0x00,   // 出
        0x4A, 0x5A, 0x69, 0xDF, 0x48, 0x4A, 0x0A, 0x00,   // 东
        0x43, 0x4C, 0xF0, 0xD0, 0x51, 0x5F, 0x40, 0x00,   // 方
        0x00, 0x00, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x0C,   // ，
        0x00, 0x3F, 0x01, 0x61, 0x01, 0x19, 0x03, 0x00,   // 心
        0x3F, 0x60, 0xAE, 0x2A, 0x2E, 0x21, 0x3F, 0x00,   // 向
        0x11, 0x52, 0x3C, 0xF0, 0x3F, 0x51, 0x13, 0x00,   // 光
        0x7C, 0x54, 0x7D, 0x02, 0xFC, 0xA9, 0xFF, 0x00,   // 明
        0x00, 0x00, 0x07, 0x05, 0x07, 0x00, 0x00, 0x00,   // 。
};  // 图案

// 硬件端口定义
// 级联端信号控制
#define LATCH_A P1_0
#define SRCLK_A P1_1
#define SER_A P1_2

// 公共端信号控制
#define LATCH_B P2_2
#define SRCLK_B P2_1
#define SER_B P2_0

u8 j = 0;   // 字位移量

// 初始化定时器0
void init_time0() {
    TMOD |= 0X01;   // 选择为定时器0模式，工作方式1
    // 给定时器赋初值，定时1ms
    // 我的是 11.0592MHz 的，C = [1ms/MC]补 = [1ms*11.0592MHz/12]补 = [921.6]补 = [399.99……H]补 ≈ [400H]补 = FC66
    // 若是 12MHz,C = [1ms*12MHz/12]补 = [1000]补 = [3E8H]补 = FC18
    TH0 = 0XFC;
    TL0 = 0X66;

    ET0 = 1;   // 打开定时器0中断允许
    EA = 1;   // 打开总中断
    TR0 = 1;   // 打开定时器
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
//    delay(1);
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
//    delay(1);
    LATCH_B = 0;
}

int main() {
    init_time0();
    while (1) {
        for (u8 i = 0; i < 8; i++) //8列显示
        {
            SendSeg(segout[i]);

            Send2Byte(~tab[(j + i) % 80], 0xff);   // 红色
//            Send2Byte(0xff, ~tab[i]);   // 绿色
//            Send2Byte(~tab[i], ~tab[i]);   // 双色
            Out595();
            DelayMs(1);

            Send2Byte(0xff, 0xff);//防止重影
            Out595();
        }

    }
}


// 定时器计时，控制时间，到点干活
void time0()

__interrupt(1) {
static unsigned int cnt = 0;   // 定时器计数
//给定时器赋初值，定时1ms
TH0 = 0XFC;
TL0 = 0X66;
// 1000 * 1ms = 1s
// 不选择直接 1s 计时一次原因 C = [1s/MC]补 = [921600]补  51 定时器无法直接完成 1s 计时
cnt++;
if (cnt == 1000) {
cnt = 0;

//j+=8;   // 字之间轮换
j++;   // 字一点点轮换

}
}