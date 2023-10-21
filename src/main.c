//
// Created by 86159 on 2023-10-21.
//
// ����
#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

// ���󰴼��˿�
#define KEY_PORT P1

const u8 out[] = "����ʵ��\r\n\0"   // 11(��һ������ʼ����)
                 "������\r\n\0"   // 20
                 "����ԭ����\r\n\0"   // 33
                 "��ָ���Ӧ��\r\n\0"   // 48
                 "��һ�ε���ʼָ���ַ\r\n\0"   // 71
                 "�������ַ����ĳ�ʼ��ַ\r\n\0"   // 96
                 "�������֮ǰ�����в����ַ����ĳ���\r\n\0"   // 133
                 "���Ǹò����ַ�������ʼ��ַ\r\n\0"   // 162
                 "ע�⣺\r\n\0"   // 171
                 "����ռ������С�ĳ���\r\n\0"   // 194
                 "��������һ�԰�\r\n\0"   // 211
                 "���°���\r\n\0"   // 222
                 "Ӧ�ò����д������ʾ\r\n\0"   // 245
                 "���ˣ�������\r\n\0"   // 260
                 "�������\r\n\0"   // 271
                 "�Լ���һ�ļ���ӡ���\r\n\0";
const u16 idx[16] = {0, 11, 20, 33, 48, 71, 96, 133,
                     162, 171, 194, 211, 222, 245, 260, 271};   // Ϊ���㲻ͬ�����ֶ�ѡ������
u8 point = -1;   // ��������
u8 change = 0;


// ���ڳ�ʼ��
void InitUART(u8 st) {
//    PCON = 1<<7;   // SMOD Ϊ 1���������� > 14400 ʱ����
    SCON = 0x40;   // SCON: ģʽ 1, 8-bit UART, ʹ�ܽ��ս�ֹ
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit ��װ
    TH1 = st;   // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz
    TL1 = st;
    TR1 = 1;   // TR1:  timer 1 ��
    EA = 1;   //�����ж�
//    ES = 1;   //�򿪴����ж�
}

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// ��ʱ���� ��λԼΪ����
void DelayMs(u8 t) {
    for (int i = 0; i < t; ++i)
        for (int j = 0; j < 110; ++j);
}

// ����һ���ֽ�
void SendByte(u8 dat) {
    SBUF = dat;
    while (!TI);
    TI = 0;
}

// ����һ���ַ���
void SendStr(u8 *s) {
    while (*s != '\0')// \0 ��ʾ�ַ���������־��
        //ͨ������Ƿ��ַ���ĩβ
    {
        SendByte(*s);
        s++;
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
        change = 1;   // �а�������ȥ
        delay(1000);   // ����
        for (u8 i = 0; i < 4; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                point = i * 4;
                break;
            }
        }
        KEY_PORT = 0xf0;   // ��λ�����ź�
        delay(1000);   // ����
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
            SendStr(out + idx[point]);
        }

        DelayMs(100);   // ̫���ˣ���ʱ��֤ 1 �� 1 ���
    }
}