//
// Created by 86159 on 2023-05-25.
//
#include "reg51.h"

#define Motor P1_0


// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

void delayMs(int ms) {
    for (int i = 0; i < ms; ++i) {
        for (int j = 0; j < 110; ++j) {

        }
    }
}

int main() {
    Motor = 1;
    delayMs(5000);
    Motor = 0;
    delayMs(5000);
    return 0;
}
