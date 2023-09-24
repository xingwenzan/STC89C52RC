//
// Created by 86159 on 2023-05-23.
//
#include "reg51.h"

#define SRCLK P3_6
#define rCLK P3_5
#define SER P3_4
#define LEDDZ_COL_PORT P0

int nums[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

void hc595Write(int num) {
    for (int i = 0; i < 8; ++i) {
        SER = (num >> (7 - i)) & 1;
        SRCLK = 0;
        delay(1);
        SRCLK = 1;
        delay(1);
    }
    rCLK = 0;
    delay(1);
    rCLK = 1;
    delay(1);
}

int main() {
    LEDDZ_COL_PORT = 0;
    while (1) {
        for (int i = 0; i < 8; ++i) {
            int num = nums[i];
            hc595Write(0x00);
            hc595Write(num);
            delay(50000);
        }
        delay(100000);
    }

    return 0;
}
