//
// Created by 86159 on 2023-05-31.
// 定时器中断
//
#include "reg51.h"

// D1阴极管脚
#define LED P2_0

// 初始化定时器中断0
void init_time0() {
    LED = 1;

    TMOD |= 0X01;   // 选择为定时器0模式，工作方式1
    // 给定时器赋初值，定时1ms
    TH0 = 0XFC;
    TL0 = 0X66;
    ET0 = 1;   // 打开定时器0中断允许
    EA = 1;   // 打开总中断
    TR0 = 1;   // 打开定时器
}

int main() {
    init_time0();
    while (1);
}

void time0() __interrupt(1) {
    static int cnt = 0;
    //给定时器赋初值，定时1ms
    TH0 = 0XFC;
    TL0 = 0X66;

    cnt++;
    if (cnt == 1000) {
        LED = !LED;
        cnt = 0;
    }
}