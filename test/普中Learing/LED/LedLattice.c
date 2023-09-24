//
// Created by 86159 on 2023-05-24.
// led 点阵实验 - 不完全成功
//
#include "reg51.h"

#define SRCLK P3_6
#define rCLK P3_5
#define SER P3_4
#define LEDDZ_COL_PORT P0

int num0[] = {0x00, 0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00};
int p[] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

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

void drawNum() {
    for (int i = 0; i < 8; ++i) {
        LEDDZ_COL_PORT = p[i];
        hc595Write(num0[i]);
        delay(100);
        hc595Write(0x00);
    }
}

int main() {

    while (1) {
        // 左上角点亮
        LEDDZ_COL_PORT = 0x7F;
        hc595Write(0x80);
        delay(50000);
        // 画 0
        drawNum();
        delay(5000000);
    }

    return 0;
}
