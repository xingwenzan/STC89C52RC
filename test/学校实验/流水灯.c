//
// Created by 86159 on 2023-10-05.
//
#include "mcs51/reg51.h"

#define LED P2

unsigned char offNum = 3;   // 流水灯关闭，控制总共走几圈
unsigned char initLED;   // 记录初始状态，辅助控制总共走几圈
unsigned char move;   // 控制亮哪个灯

// 初始化 LED
void init_led(unsigned char i) {
    LED = ~0;   // 最开始所有灯全灭
    // move、initLED 定义应在开始的灯的前一个，即一圈灯亮的最后一个
    move = (i + 7) % 8;    // (i-1+8)%8 防负数
    initLED = ~(1 << move);
}

// 初始化定时器0
void init_time0() {
    TMOD |= 0X01;   // 选择为定时器0模式，工作方式1
    // 给定时器赋初值，定时1ms
    TH0 = 0XFC;
    TL0 = 0X66;

    ET0 = 1;   // 打开定时器0中断允许
    EA = 1;   // 打开总中断
    TR0 = 1;   // 打开定时器
}

// 流水灯移动
void waterLamp() {
    LED = offNum ? ~(1 << (++move % 8)) : ~0;   // Loading flash: 458 bytes (Intel HEX)
// Loading flash: 461 bytes (Intel HEX)
//    if (offNum){
//        LED =  ~(1<<(++move%8));
//    } else{
//        LED = ~0;
//        EA = 0;
//    }
    if (LED == initLED)offNum--;
}

int main() {
    init_led(0);
    init_time0();
    while (1);
    return 0;
}

// 定时器计时，控制时间，到点干活
void time0()

__interrupt(1) {
static unsigned int cnt = 0;   // 定时器计数
//给定时器赋初值，定时1ms
TH0 = 0XFC;
TL0 = 0X66;
// 1000 * 1ms = 1s
cnt++;
if (cnt == 1000) {
cnt = 0;

waterLamp();

}
}