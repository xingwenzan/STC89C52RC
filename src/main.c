//
// Created by 86159 on 2023-10-09.
// 无中断版 - 实验室电路
// 数码管显示字母 https://blog.csdn.net/weixin_41413511/article/details/102777228
//
#include "mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;
// 名字结构体，代表名字的单个字，如：张-zhang、馨-xin、文-wne
typedef struct {
    u8 name[5];   // 理论上一个字母用一个晶体管，但最多不会超过 5 个，一个晶体管只需要一个字节表示
} NAME;

/**
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P0
// 控制锁存器
#define LSA P2_2   // 段选开关
#define LSB P2_3   // 位选开关
// 矩阵按键端口
#define KEY_PORT P1

u8 sign = 0;   // 中断标志，有中断变 1，进行扫描

/**
 * 共阴 0-9、a-z、空格 字码表
 * 第一行是 0-9
 * 最后一个是空格
 * 其余是 a-z
 * 调用例子 a-z   gsmg_code[goal-'a'+10]   gaol 是目标字母
 */
const u8 gsmg_code[37] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,   // 0-9
                          0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, 0x76, 0x10,   // a-i
                          0x0e, 0x7a, 0x38, 0x55, 0x54, 0x5c, 0x73, 0x67, 0x50,   // j-r
                          0x64, 0x78, 0x3e, 0x62, 0x6a, 0x36, 0x6e, 0x49, 0x00};   // s-z 空格

/**
 * 我的单片机是共阴且 DP 在最高位
 * 实验室设备是共阳且 DP 在最高位
 * 实际实验需单进行取反处理
 */
NAME names[4][4];
u8 x = 0, y = 0;   // 按键坐标

// 名字初始化
void name_init() {
    for (u8 i = 0; i < 4; ++i) {
        for (u8 j = 0; j < 4; ++j) {
            for (u8 k = 0; k < 5; ++k) {
                names[i][j].name[k] = 'z' + 1;
            }
        }
    }

    // zhang
    names[0][0].name[0] = 'z';
    names[0][0].name[1] = 'h';
    names[0][0].name[2] = 'a';
    names[0][0].name[3] = 'n';
    names[0][0].name[4] = 'g';
    // xin
    names[0][1].name[0] = 'x';
    names[0][1].name[1] = 'i';
    names[0][1].name[2] = 'n';
    // wen
    names[0][2].name[0] = 'w';
    names[0][2].name[1] = 'e';
    names[0][2].name[2] = 'n';

    // yao
    names[1][0].name[0] = 'y';
    names[1][0].name[1] = 'a';
    names[1][0].name[2] = 'o';
    // shun
    names[1][1].name[0] = 's';
    names[1][1].name[1] = 'h';
    names[1][1].name[2] = 'u';
    names[1][1].name[3] = 'n';
    // yu
    names[1][2].name[0] = 'y';
    names[1][2].name[1] = 'u';

    // han
    names[2][0].name[0] = 'h';
    names[2][0].name[1] = 'a';
    names[2][0].name[2] = 'n';
    // he
    names[2][1].name[0] = 'h';
    names[2][1].name[1] = 'e';
    // zhi
    names[2][2].name[0] = 'z';
    names[2][2].name[1] = 'h';
    names[2][2].name[2] = 'i';

    // chen
    names[3][0].name[0] = 'c';
    names[3][0].name[1] = 'h';
    names[3][0].name[2] = 'e';
    names[3][0].name[3] = 'n';
    // jia
    names[3][1].name[0] = 'j';
    names[3][1].name[1] = 'i';
    names[3][1].name[2] = 'a';
    // qi
    names[3][2].name[0] = 'q';
    names[3][2].name[1] = 'i';
}

// 初始化外部中断0
void init_int0() {
    IT0 = 1; // 外部中断0的触发方式为1(电平下降沿)
    EX0 = 1; // 允许外部中断0
    EA = 1; // 中断总开关
    KEY_PORT = 0x0f;   // 高位发出信号
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

/**
 * 按键读取函数，直接赋值到 x,y
 * 我的单片机高位是行(x) 低位是列(y)   7-4 对应 0-3 行   4-0 对应 0-3 行
 * 低电平接通
 */
void keyScan() {
    sign = 0;
    KEY_PORT = 0x0f;   // 高位发出信号
    delay(1000);   // 消抖
    for (u8 i = 0; i < 4; ++i) {
        if (!((KEY_PORT >> i) & 1)) {
            x = i;
            break;
        }
    }
    KEY_PORT = 0xf0;   // 低位发出信号
    delay(1000);   // 消抖
    for (u8 i = 4; i < 8; ++i) {
        if (!((KEY_PORT >> i) & 1)) {
            y = 7 - i;
            break;
        }
    }
}

// 扫描并显示
void keyDisplay() {
    // 显示
    for (u8 i = 0; i < 5; ++i) {
        // 位选
        // 取位码
        SMG_A_DP_PORT = ~(1 << i);
        // 位锁存
        LSB = 1;
        LSB = 0;
        // 段选
        // 取段码
        SMG_A_DP_PORT = gsmg_code[names[x][y].name[i] - 'a' + 10];
        // 段锁存
        LSA = 1;
        LSA = 0;
        // 延时
        delay(100);
        // 归零 消除对下一位影响
        SMG_A_DP_PORT = 0x00;
    }
}

int main() {
    // 初始化
    name_init();
    init_int0();
    // 循环扫描并显示
    while (1) {
        KEY_PORT = 0x0f;   // 高位发出信号
        if (sign)keyScan();
        keyDisplay();
    }
    return 0;
}


// 外部中断0服务函数
void int0_interrupt_callback()

__interrupt(0) {
// 按键按下触发的中断 对机械按键进行消抖
//delay(1000);
sign = 1;

}