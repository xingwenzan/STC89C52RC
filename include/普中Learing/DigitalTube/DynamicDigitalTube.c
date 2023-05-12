//
// Created by 86159 on 2023-05-11.
//
#include "reg51.h"
/*
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P0
// 控制 38 译码器 -> 控制位选
#define LSA P2_2
#define LSB P2_3
#define LSC P2_4

// 共阴 0-F
int gsmg_code[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

// 动态数码管显示
void smg_display() {
    for (int i = 0; i < 8; ++i) {
        // 位选
        LSA = ((7 - i) >> 0) & 1;
        LSB = ((7 - i) >> 1) & 1;
        LSC = ((7 - i) >> 2) & 1;
        // 段选
        SMG_A_DP_PORT = gsmg_code[i];
        // 延时
        delay(100);
        // 归零 消除对下一位影响
        SMG_A_DP_PORT = 0x00;
    }
}

int main() {
    smg_display();
    return 0;
}
