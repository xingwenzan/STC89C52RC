//
// δ����
//
#include "mcs51/reg51.h"
/*
 * SMG ��ѡ ���Ƶ����������ʾɶ
 * A_DP һ��������� 8 �� LED ��ɣ��ֱ�Ϊ A B C D E F G DP(С����)
 * PORT �˿�
 * g ȫ�ֱ���
 */
#define SMG_A_DP_PORT P0
#define LED P2_0
// ���� 38 ������ -> ����λѡ
#define LSA P2_2
#define LSB P2_3
#define LSC P2_4
// DS18B20 ����
#define DS18B20_PORT P3_7

// ���� 0-F
int gsmg_code[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(int ten_us) {
    while (ten_us--);
}

// ��ʱ����λ������ 1 ���룬ÿ���� 1����Լ��ʱ 1ms
void delayMs(int ms) {
    for (int i = 0; i < ms; ++i) {
        for (int j = 0; j < 110; ++j) {

        }
    }
}

/**
 * @brief �������
 * @param pos ��ָ��λ�ÿ�ʼ ����ܱ�ʶ������[1...8]
 * @param dat ������������ʾ���� dat����洢���Ƕ�������
 */
void smg_display(int pos, int dat[]) {
    int idx = 0;
    for (unsigned char i = pos - 1; i < 8; ++i) {
        // λѡ �����ĸ�LED����ܵ���
        switch (i) {
            case 0:
                LSC = 1, LSB = 1, LSA = 1;
                break; // 111->7 LED8������
            case 1:
                LSC = 1, LSB = 1, LSA = 0;
                break; // 110->6 LED7������
            case 2:
                LSC = 1, LSB = 0, LSA = 1;
                break; // 101->5 LED6������
            case 3:
                LSC = 1, LSB = 0, LSA = 0;
                break; // 100->4 LED5������
            case 4:
                LSC = 0, LSB = 1, LSA = 1;
                break; // 011->3 LED4������
            case 5:
                LSC = 0, LSB = 1, LSA = 0;
                break; // 010->2 LED3������
            case 6:
                LSC = 0, LSB = 0, LSA = 1;
                break; // 001->1 LED2������
            case 7:
                LSC = 0, LSB = 0, LSA = 0;
                break; // 000->0 LED1������
            default:
                break;
        }
        // ����ܶ�ѡ
        SMG_A_DP_PORT = dat[idx];
        idx++;
        delayMs(1);
        SMG_A_DP_PORT = gsmg_code[16]; // ����
    }
}

// DS18B20 ��λ
void ds18b20_reset() {
    DS18B20_PORT = 0;
    delay(75);
    DS18B20_PORT = 1;
    delay(2);
}

// ��� DS18B20   1 δ��⵽   0 ��⵽
int ds18b20_check() {
    int time = 0;
    // �ȴ� 200us������Ƿ���Ӧ�͵�ƽ����Ӧ��ʱ�����ѭ��
    while (DS18B20_PORT && time < 20) {
        time++;
        delay(1);
    }
    if (time >= 20) { return 1; }   // ��ʱδ��Ӧ��δ��⵽
    time = 0;   // ��⵽�ˣ���ʱ����
    // �ȴ� 200us������Ƿ���Ӧ�ߵ�ƽ����Ӧ��ʱ�����ѭ��
    while ((!DS18B20_PORT) && time < 20) {
        time++;
        delay(1);
    }
    if (time >= 20) { return 1; }   // ��ʱδ��Ӧ��δ��⵽
    return 0;
}

// DS18B20 ��ʼ�� - ��λ�źš������Ӧ
int ds18b20_init() {
    ds18b20_reset();
    return ds18b20_check();
}

// д 0 ʱ��
void ds18b20_write_0() {
    DS18B20_PORT = 0;
    delay(6);
    DS18B20_PORT = 1;
    delay(1);
}

// д 1 ʱ��
void ds18b20_write_1() {
    DS18B20_PORT = 0;
    delay(1);
    DS18B20_PORT = 1;
    delay(6);
}

// �� DS18B20 д���ֽ�
void ds18b20_write_byte(int num) {
    for (int i = 0; i < 8; ++i) {
        int tmp = (num >> i) & 0x01;
        if (tmp) {   // д 1
            ds18b20_write_1();
        } else {   // д 0
            ds18b20_write_0();
        }
    }
}

// �� DS18B20 ���� - ��һλ��0/1��
int ds18b20_read_bit() {
    int ans;
    DS18B20_PORT = 0;
    delay(1);
    if (DS18B20_PORT) { ans = 1; }   // �� 1
    else { ans = 0; }   // �� 0
    delay(5);
    return ans;
}

// �� DS18B20 �����ֽ�   ��λ -> ��λ
int ds18b20_read_byte() {
    int ans = 0;
    for (int i = 0; i < 8; ++i) {
        int tmp = ds18b20_read_bit();
        ans >>= 1;
        ans |= (tmp << 7);
    }
    return ans;
}

// DS18B20 ��ʼ�¶�ת��
void ds18b20_start() {
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   // ���� SKIP ROM ����
    ds18b20_write_byte(0x44);   // ���Ϳ�ʼת������
}

// DS18B20 �¶ȶ�ȡ
float ds18b20_read_temperature() {
    int value;
    float ans;
    ds18b20_start();   // ��ʼ�¶�ת��
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   // ���� SKIP ROM ����
    ds18b20_write_byte(0xbe);   // ���Ͷ��洢������
    int numL = ds18b20_read_byte();   // ���� 8 λ
    int numH = ds18b20_read_byte();   // ���� 8 λ
    value = (numH << 8) | numL;
    if ((value & 0xf800) == 0xf800) // ��5λ����λ
    {
        value = (~value) + 1;
        ans = value * (-0.0625);
    } else { ans = value * 0.0625; }
    return ans;
}

int main() {
    ds18b20_init();
    while (1) {
        int tmp_value, tmp_code[5];   // ���� 125C�����ϸ��ź�С����� 5 λ
        delayMs(1);
        tmp_value = (int) (ds18b20_read_temperature() * 10);
        if (tmp_value < 0) {
            tmp_value = -tmp_value;
            tmp_code[0] = 0x40;   // ��ʾ����
        } else {
            tmp_code[0] = 0x00;   // ����ʾ
        }
        tmp_code[1] = tmp_value / 1000 ? gsmg_code[1] : 0x00;
        tmp_code[2] = gsmg_code[(tmp_value % 1000) / 100];
        tmp_code[3] = gsmg_code[tmp_value % 1000 % 100 / 10] | 0x80; // dp��С����
        tmp_code[4] = gsmg_code[tmp_value % 1000 % 100 % 10];
        smg_display(4, tmp_code);
    }
}
