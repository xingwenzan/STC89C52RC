//
// Created by 86159 on 2023-05-12.
//
#include "reg51.h"

/*
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P0
// 按键
#define KEY1 P3_1
#define KEY2 P3_0
#define KEY3 P3_2
#define KEY4 P3_3
// LED
#define LED P2_0

// 共阴 0-F
int gsmg_code[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

int keyScan() {
    if (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0) {
        LED = 0;
        delay(1000);   //消抖
        if (KEY1 == 0) { return 1; }
        else if (KEY2 == 0) { return 2; }
        else if (KEY3 == 0) { return 3; }
        else if (KEY4 == 0) { return 4; }
    }
    LED = 1;
    delay(1000);
    return 0;
}

void keyDisplay() {
    SMG_A_DP_PORT = gsmg_code[keyScan()];
}

int main() {
    keyDisplay();
    return 0;
}
