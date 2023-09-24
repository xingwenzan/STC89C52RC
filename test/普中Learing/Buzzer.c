//
// Created by 86159 on 2023-05-07.
//
#include "reg51.h"

#define beep P2_5

void delay(int us) {   // 延时，单位大致是 10 微秒
    while (us--);
}

void buzzer(int low, int high) {
    /*
     * 改变单次 delay 总长度（周期） -》 改变音调
     * 改变高低电平比例 -》 改变音量
     */
    beep = 0;
    while (1) {
        beep = 0;
        delay(low);
        beep = 1;
        delay(high);
    }
}

int main() {
    buzzer(100, 100);
    return 0;
}
