//
// Created by 86159 on 2023-05-07.
//
#include "reg51.h"

#define LED P2_0

void delay(int us) {   // 延时，单位大致是 10 微秒
    while (us--);
}

void blink(int us) {   // 闪烁
    while (1) {
        LED = 1 - LED;
        delay(us);
    }
}

int main() {
    LED = 0;
    blink(1000000);
    return 0;
}
