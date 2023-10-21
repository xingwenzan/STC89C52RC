//
// Created by 86159 on 2023-10-21.
//
// 串口
#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

// 矩阵按键端口
#define KEY_PORT P1

const u8 out[] = "青天有月来几时\r\n\0"
                 "我今停杯一问之\r\n\0"
                 "人攀明月不可得\r\n\0"
                 "月行却与人相随\r\n\0"
                 "皎如飞镜临丹阙\r\n\0"
                 "绿烟灭尽清辉发\r\n\0"
                 "但见宵从海上来\r\n\0"
                 "宁知晓向云间没\r\n\0"
                 "白兔捣药秋复春\r\n\0"
                 "嫦娥孤栖与谁邻\r\n\0"
                 "今人不见古时月\r\n\0"
                 "今月曾经照古人\r\n\0"
                 "古人今人若流水\r\n\0"
                 "共看明月皆如此\r\n\0"
                 "唯愿当歌对酒时\r\n\0"
                 "月光长照金樽里\r\n\0";
u8 point = -1;   // 按键坐标
u8 change = 0;

// 串口初始化
void InitUART(u8 st) {
//    PCON = 1<<7;   // SMOD 为 1，当波特率 > 14400 时开启
    SCON = 0x40;   // SCON: 模式 1, 8-bit UART, 使能接收禁止
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit 重装
    TH1 = st;   // TH1:  重装值 9600 波特率 晶振 11.0592MHz
    TL1 = st;
    TR1 = 1;   // TR1:  timer 1 打开
    EA = 1;   //打开总中断
//    ES = 1;   //打开串口中断
}

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void DelayMs(u8 t) {
    for (int i = 0; i < t; ++i)
        for (int j = 0; j < 110; ++j);
}

// 发送一个字节
void SendByte(u8 dat) {
    SBUF = dat;
    while (!TI);
    TI = 0;
}

// 发送一个字符串
void SendStr(u8 *s) {
    while (*s != '\0')// \0 表示字符串结束标志，
        //通过检测是否字符串末尾
    {
        SendByte(*s);
        s++;
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
        change = 1;   // 有按键按下去
        delay(1000);   // 消抖
        for (u8 i = 0; i < 4; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                point = i * 4;
                break;
            }
        }
        KEY_PORT = 0xf0;   // 低位发出信号
        delay(1000);   // 消抖
        for (u8 i = 4; i < 8; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                point += (7 - i);
                break;
            }
        }
    }


}

int main() {

    InitUART(0xFD);

    while (1) {
        keyScan();
        if (change) {
            change = 0;
            SendStr(out + point * 17);
        }

        DelayMs(100);   // 太快了，延时保证 1 按 1 输出
    }
}