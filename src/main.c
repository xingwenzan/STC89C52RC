//
// Created by 86159 on 2023-10-09.
// ���жϰ� - ʵ���ҵ�·
// δ���
// �������ʾ��ĸ https://blog.csdn.net/weixin_41413511/article/details/102777228
//
#include "mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

/**
 * SMG ��ѡ ���Ƶ����������ʾɶ
 * A_DP һ��������� 8 �� LED ��ɣ��ֱ�Ϊ A B C D E F G DP(С����)
 * PORT �˿�
 * g ȫ�ֱ���
 */
#define SMG_A_DP_PORT P2
// ����������
#define LSA P3_0   // ��ѡ����
#define LSB P3_1   // λѡ����
// ���󰴼��˿�
#define KEY_PORT P1
// �������
#define motor P0

/**
 * ���� 0-9��a-z���ո� �����
 * ��һ���� 0-9
 * ���һ���ǿո�
 * ������ a-z
 * �������� a-z   gsmg_code[goal-'a'+10]   gaol ��Ŀ����ĸ
 */
const u8 gsmg_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x00};   // 0-9 �ո�
const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
const u8 password[] = {1, 2, 3, 4, 5, 6};

u8 x = 3, y = 3;   // ��������
u8 input[8];   // ������� �������һλ����������λ�ã������ڶ�λ��ԶΪ��
u8 lock = 0;   // ����״̬   0 ��δ������������   1 ��������������
u8 haveKey = 0;   // ���ް�������

void init() {
    for (int i = 0; i < 7; ++i) {
        input[i] = 10;
    }
    input[7] = 0;
}

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// ��ʱ���� ��λԼΪ����
void DelayMs(u16 t) {
    for (u16 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}

// ��������������
void turn_on() {
    if (lock) {   // ���Ѿ��򿪣���������

    } else {   // δ������������
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            DelayMs(50);
        }
        motor = ~0;
    }
}

// �ر�����������
void turn_off() {
    if (lock) {    // ����������Ҫ����
        lock = 0;
        for (u8 i = 0; i < 8; i++) {
            motor = fan[i];
            DelayMs(50);
        }
        motor = ~0;
    } else {   // δ�������������

    }
}

//�������
void left() {
    input[7] = (input[7] - 1 + 6) % 6;
}

// �������
void right() {
    input[7] = (input[7] + 1) % 6;
}

// ���
void clear() {
    for (int i = 0; i < 6; ++i) {
        input[i] = 10;
    }
    input[7] = 0;
}

// �жϲ�����
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

// ���Լ��� delete
void delete() {
    right();
    input[input[7]] = 10;
    left();
}

// ���Լ��� backspace
void backspace() {
    input[input[7]] = 10;
    left();
}

// ������ȡ�������Ϊ
void op() {
    if (!haveKey)return;// �ް������£�������
    haveKey = 0;
    u8 sign = x * 4 + y;
    switch (sign) {
        case 10:   // �������
            left();
            break;
        case 11:   // �������
            right();
            break;
        case 12:   // ���Լ��� backspace
            backspace();
            break;
        case 13:   // ���Լ��� delete
            delete();
            break;
        case 14:   // ���
            clear();
            break;
        case 15:   // �س�
            judge();
            break;
        default:   // 0-9 ��������
            input[input[7]] = sign;
            input[7] = (input[7] + 1) % 6;
            break;
    }
}

/**
 * ������ȡ������ֱ�Ӹ�ֵ�� x,y
 * �ҵĵ�Ƭ����λ����(x) ��λ����(y)   7-4 ��Ӧ 0-3 ��   4-0 ��Ӧ 0-3 ��
 * �͵�ƽ��ͨ
 */
void keyScan() {
    KEY_PORT = 0x0f;   // ��λ�����ź�
    if (KEY_PORT != 0x0f) {   // �б仯
        haveKey = 1;
        delay(1000);   // ����
        for (u8 i = 0; i < 4; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                x = i;
                break;
            }
        }
        KEY_PORT = 0xf0;   // ��λ�����ź�
        delay(1000);   // ����
        for (u8 i = 4; i < 8; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                y = 7 - i;
                break;
            }
        }
        DelayMs(250);
    }
}

// ��ʾ
void keyDisplay() {
    for (u8 i = 0; i < 8; ++i) {
        // ���� ��������һλӰ��
        SMG_A_DP_PORT = 0x00;
        // ������
        LSA = 1;
        LSA = 0;

        // λѡ
        // ȡλ��
        SMG_A_DP_PORT = ~(1 << i);
        // λ����
        LSB = 1;
        LSB = 0;

        // ��ѡ
        // ȡ����
        SMG_A_DP_PORT = gsmg_code[input[i]];
        // ������
        LSA = 1;
        LSA = 0;
        // ��ʱ
        delay(100);
    }

}

int main() {
    init();
    // ѭ��ɨ�貢��ʾ
    while (1) {
        keyScan();
        op();
        keyDisplay();
    }
    return 0;
}
