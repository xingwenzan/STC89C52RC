//
// Created by 86159 on 2023-10-09.
// 无中断版 - 实验室电路
// 未完成
// 数码管显示字母 https://blog.csdn.net/weixin_41413511/article/details/102777228
//
#include "mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

/**
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P2
// 控制锁存器
#define LSA P3_0   // 段选开关
#define LSB P3_1   // 位选开关
// 蜂鸣器控制
#define BUZZER P3_7
// 矩阵按键端口
#define KEY_PORT P1
// 电机引脚
#define motor P0

/**
 * 共阴 0-9、a-z、空格 字码表
 * 第一行是 0-9
 * 最后一个是空格
 * 其余是 a-z
 * 调用例子 a-z   gsmg_code[goal-'a'+10]   gaol 是目标字母
 */
const u8 gsmg_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x00};   // 0-9 空格
const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
const u8 password[] = {1, 2, 3, 4, 5, 6};

u8 x = 3, y = 3;   // 按键坐标
u8 input[8];   // 输入读入 其中最后一位代表光标所在位置，倒数第二位永远为空
u8 lock = 0;   // 开关状态   0 锁未开启，即开门   1 锁开启，即锁门
u8 haveKey = 0;   // 有无按键按下

void init() {
    for (int i = 0; i < 7; ++i) {
        input[i] = 10;
    }
    input[7] = 0;
    BUZZER = 0;
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u16 t) {
    for (u16 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}

// 蜂鸣器响
void buzzer_on() {
    BUZZER = 1;
}

// 蜂鸣器灭
void buzzer_off() {
    BUZZER = 0;
}

// 开启锁，即锁门
void turn_on() {
    if (lock) {   // 锁已经打开，即已上锁
        buzzer_on();
    } else {   // 未上锁，需上锁
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            DelayMs(50);
        }
        motor = ~0;
    }
}

// 关闭锁，即开门
void turn_off() {
    if (lock) {    // 已上锁，需要解锁
        lock = 0;
        for (u8 i = 0; i < 8; i++) {
            motor = fan[i];
            DelayMs(50);
        }
        motor = ~0;
    } else {   // 未上锁，无需解锁

    }
}

//光标左移
void left() {
    input[7] = (input[7] - 1 + 6) % 6;
}

// 光标右移
void right() {
    input[7] = (input[7] + 1) % 6;
}

// 清空
void clear() {
    for (int i = 0; i < 6; ++i) {
        input[i] = 10;
    }
    input[7] = 0;
}

// 判断并操作
void judge() {
    u8 i = 0;
    while (i < 6) {
        if (input[i] == password[i])i++;
        else break;
    }
    clear();
    if (i == 6)turn_off();
    else turn_on();
}

// 电脑键盘 delete 删掉当前位置及以右的所有内容，并将光标留在当前位置（相当于清空后面的内容）
void delete() {
    for (int i = input[7]; i < 6; ++i) {
        input[i] = 10;
    }
//    input[input[7]] = 10;
}

// 电脑键盘 backspace 删掉当前位置的内容，并将光标留在当前位置（相当于内容整体左移一位）
void backspace() {
    for (int i = input[7]; i < 6; ++i) {
        input[i] = input[i + 1];
    }
//    input[input[7]] = 10;
//    left();
}

// 按键读取后进行行为
void op() {
    if (!haveKey)return;// 无按键按下，不操作
    haveKey = 0;
    u8 sign = x * 4 + y;
    switch (sign) {
        case 10:   // 光标左移
            left();
            break;
        case 11:   // 光标右移
            right();
            break;
        case 12:   // 电脑键盘 backspace
            backspace();
            break;
        case 13:   // 电脑键盘 delete
            delete();
            break;
        case 14:   // 清空
            clear();
            break;
        case 15:   // 回车
            judge();
            break;
        default:   // 0-9 密码输入
            input[input[7]] = sign;
            input[7] = (input[7] + 1) % 6;
            break;
    }
}

/**
 * 按键读取函数，直接赋值到 x,y
 * 我的单片机高位是行(x) 低位是列(y)   7-4 对应 0-3 行   4-0 对应 0-3 行
 * 低电平接通
 */
void keyScan() {
    KEY_PORT = 0x0f;   // 高位发出信号
    if (KEY_PORT != 0x0f) {   // 有变化
        buzzer_off();
        haveKey = 1;
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
        DelayMs(500);
    }
}

// 显示
void keyDisplay() {
    for (u8 i = 0; i < 8; ++i) {
        // 归零 消除对下一位影响
        SMG_A_DP_PORT = 0x00;
        // 段锁存
        LSA = 1;
        LSA = 0;

        // 位选
        // 取位码
        SMG_A_DP_PORT = ~(1 << i);
        // 位锁存
        LSB = 1;
        LSB = 0;

        // 段选
        // 取段码
        SMG_A_DP_PORT = gsmg_code[input[i]];
        // 段锁存
        LSA = 1;
        LSA = 0;
        // 延时
        delay(100);
    }

}

int main() {
    init();
    // 循环扫描并显示
    while (1) {
        keyScan();
        op();
        keyDisplay();
    }
    return 0;
}
