//
// Created by 86159 on 2023-05-14.
//
#include "reg51.h"

/*
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P0
#define KEY_PORT P1

// 共阴 0-F
int gsmg_code[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

// 矩阵按键读取 行列式扫描
int keyScanRanks() {
    int key_value = 0;

    KEY_PORT = 0xf7;
    if (KEY_PORT != 0xf7) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x77:
                key_value = 1;
                break;
            case 0xb7:
                key_value = 5;
                break;
            case 0xd7:
                key_value = 9;
                break;
            case 0xe7:
                key_value = 13;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xf7);

    KEY_PORT = 0xfb;
    if (KEY_PORT != 0xfb) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7b:
                key_value = 2;
                break;
            case 0xbb:
                key_value = 6;
                break;
            case 0xdb:
                key_value = 10;
                break;
            case 0xeb:
                key_value = 14;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xfb);

    KEY_PORT = 0xfd;
    if (KEY_PORT != 0xfd) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7d:
                key_value = 3;
                break;
            case 0xbd:
                key_value = 7;
                break;
            case 0xdd:
                key_value = 11;
                break;
            case 0xed:
                key_value = 15;
                break;
        }
    }
    while (KEY_PORT != 0xfd);

    KEY_PORT = 0xfe;
    if (KEY_PORT != 0xfe) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7e:
                key_value = 4;
                break;
            case 0xbe:
                key_value = 8;
                break;
            case 0xde:
                key_value = 12;
                break;
            case 0xee:
                key_value = 16;
                break;
        }
    }
    while (KEY_PORT != 0xfe);

    return key_value;
}

// 矩阵按键读取 线翻转式扫描
int keyScanFilp() {
    int key_value = 0;

    KEY_PORT = 0x0f;
    if (KEY_PORT != 0x0f) {
        delay(1000);
        if (KEY_PORT != 0x0f) {
            KEY_PORT = 0x0f;
            switch (KEY_PORT) {
                case 0x07:
                    key_value = 1;
                    break;
                case 0x0b:
                    key_value = 2;
                    break;
                case 0x0d:
                    key_value = 3;
                    break;
                case 0x0e:
                    key_value = 4;
                    break;
                default:
                    break;
            }
            KEY_PORT = 0xf0;
            switch (KEY_PORT) {
                case 0x70:
                    break;
                case 0xb0:
                    key_value += 4;
                    break;
                case 0xd0:
                    key_value += 8;
                    break;
                case 0xe0:
                    key_value += 12;
                    break;
                default:
                    break;
            }
            while (KEY_PORT != 0xf0);
        }
    } else
        key_value = 0;

    return key_value;
}

void keyDisplay() {
//    int key = keyScanRanks();
    int key = keyScanFilp();
    if (key) {
        SMG_A_DP_PORT = gsmg_code[key - 1];
    }
}

int main() {
    keyDisplay();
    return 0;
}
