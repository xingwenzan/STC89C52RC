//
// 未调完
//
#include "mcs51/reg51.h"
/*
 * SMG 段选 控制单个数码管显示啥
 * A_DP 一个数码管由 8 个 LED 组成，分别为 A B C D E F G DP(小数点)
 * PORT 端口
 * g 全局变量
 */
#define SMG_A_DP_PORT P0
#define LED P2_0
// 控制 38 译码器 -> 控制位选
#define LSA P2_2
#define LSB P2_3
#define LSC P2_4
// DS18B20 总线
#define DS18B20_PORT P3_7

// 共阴 0-F
int gsmg_code[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(int ten_us) {
    while (ten_us--);
}

// 延时，单位大致是 1 毫秒，每传入 1，大约延时 1ms
void delayMs(int ms) {
    for (int i = 0; i < ms; ++i) {
        for (int j = 0; j < 110; ++j) {

        }
    }
}

/**
 * @brief 数码管显
 * @param pos 从指定位置开始 数码管标识从左到右[1...8]
 * @param dat 将数据依次显示出来 dat数组存储的是段码数据
 */
void smg_display(int pos, int dat[]) {
    int idx = 0;
    for (unsigned char i = pos - 1; i < 8; ++i) {
        // 位选 控制哪个LED数码管点亮
        switch (i) {
            case 0:
                LSC = 1, LSB = 1, LSA = 1;
                break; // 111->7 LED8被点亮
            case 1:
                LSC = 1, LSB = 1, LSA = 0;
                break; // 110->6 LED7被点亮
            case 2:
                LSC = 1, LSB = 0, LSA = 1;
                break; // 101->5 LED6被点亮
            case 3:
                LSC = 1, LSB = 0, LSA = 0;
                break; // 100->4 LED5被点亮
            case 4:
                LSC = 0, LSB = 1, LSA = 1;
                break; // 011->3 LED4被点亮
            case 5:
                LSC = 0, LSB = 1, LSA = 0;
                break; // 010->2 LED3被点亮
            case 6:
                LSC = 0, LSB = 0, LSA = 1;
                break; // 001->1 LED2被点亮
            case 7:
                LSC = 0, LSB = 0, LSA = 0;
                break; // 000->0 LED1被点亮
            default:
                break;
        }
        // 数码管段选
        SMG_A_DP_PORT = dat[idx];
        idx++;
        delayMs(1);
        SMG_A_DP_PORT = gsmg_code[16]; // 消隐
    }
}

// DS18B20 复位
void ds18b20_reset() {
    DS18B20_PORT = 0;
    delay(75);
    DS18B20_PORT = 1;
    delay(2);
}

// 检测 DS18B20   1 未检测到   0 检测到
int ds18b20_check() {
    int time = 0;
    // 等待 200us，检测是否响应低电平，响应或超时则提出循环
    while (DS18B20_PORT && time < 20) {
        time++;
        delay(1);
    }
    if (time >= 20) { return 1; }   // 超时未响应，未检测到
    time = 0;   // 检测到了，计时归零
    // 等待 200us，检测是否响应高电平，响应或超时则提出循环
    while ((!DS18B20_PORT) && time < 20) {
        time++;
        delay(1);
    }
    if (time >= 20) { return 1; }   // 超时未响应，未检测到
    return 0;
}

// DS18B20 初始化 - 复位信号、检测响应
int ds18b20_init() {
    ds18b20_reset();
    return ds18b20_check();
}

// 写 0 时序
void ds18b20_write_0() {
    DS18B20_PORT = 0;
    delay(6);
    DS18B20_PORT = 1;
    delay(1);
}

// 写 1 时序
void ds18b20_write_1() {
    DS18B20_PORT = 0;
    delay(1);
    DS18B20_PORT = 1;
    delay(6);
}

// 给 DS18B20 写入字节
void ds18b20_write_byte(int num) {
    for (int i = 0; i < 8; ++i) {
        int tmp = (num >> i) & 0x01;
        if (tmp) {   // 写 1
            ds18b20_write_1();
        } else {   // 写 0
            ds18b20_write_0();
        }
    }
}

// 从 DS18B20 读入 - 读一位（0/1）
int ds18b20_read_bit() {
    int ans;
    DS18B20_PORT = 0;
    delay(1);
    if (DS18B20_PORT) { ans = 1; }   // 读 1
    else { ans = 0; }   // 读 0
    delay(5);
    return ans;
}

// 从 DS18B20 读入字节   低位 -> 高位
int ds18b20_read_byte() {
    int ans = 0;
    for (int i = 0; i < 8; ++i) {
        int tmp = ds18b20_read_bit();
        ans >>= 1;
        ans |= (tmp << 7);
    }
    return ans;
}

// DS18B20 开始温度转换
void ds18b20_start() {
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   // 发送 SKIP ROM 命令
    ds18b20_write_byte(0x44);   // 发送开始转换命令
}

// DS18B20 温度读取
float ds18b20_read_temperature() {
    int value;
    float ans;
    ds18b20_start();   // 开始温度转换
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   // 发送 SKIP ROM 命令
    ds18b20_write_byte(0xbe);   // 发送读存储器命令
    int numL = ds18b20_read_byte();   // 读低 8 位
    int numH = ds18b20_read_byte();   // 读高 8 位
    value = (numH << 8) | numL;
    if ((value & 0xf800) == 0xf800) // 高5位符号位
    {
        value = (~value) + 1;
        ans = value * (-0.0625);
    } else { ans = value * 0.0625; }
    return ans;
}

int main() {
    ds18b20_init();
    while (1) {
        int tmp_value, tmp_code[5];   // 最大测 125C，算上负号和小数点后共 5 位
        delayMs(1);
        tmp_value = (int) (ds18b20_read_temperature() * 10);
        if (tmp_value < 0) {
            tmp_value = -tmp_value;
            tmp_code[0] = 0x40;   // 显示负号
        } else {
            tmp_code[0] = 0x00;   // 不显示
        }
        tmp_code[1] = tmp_value / 1000 ? gsmg_code[1] : 0x00;
        tmp_code[2] = gsmg_code[(tmp_value % 1000) / 100];
        tmp_code[3] = gsmg_code[tmp_value % 1000 % 100 / 10] | 0x80; // dp段小数点
        tmp_code[4] = gsmg_code[tmp_value % 1000 % 100 % 10];
        smg_display(4, tmp_code);
    }
}
