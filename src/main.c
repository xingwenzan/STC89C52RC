//
// Created by 86159 on 2023-10-30.
//
#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

#define motor P1   // �������
#define key P1_7   // ����/��������ȷ������

const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
const u8 password[] = "123456\0";

u8 input[7], idx = 0;   // ������� ����λ
u8 lock = 0;   // ����״̬   0 ��δ������������   1 ��������������

// ���ڳ�ʼ��
void InitUART(u8 p, u8 st) {
    PCON |= p << 7;   // SMOD Ϊ p���������� > 14400 ʱ����
    SCON = 0x50;   // SCON: ģʽ 1, 8-bit UART, ʹ�ܽ���
    TMOD &= 0x0F;   // �����ʱ��1ģʽλ
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit ��װ
    TH1 = st;   // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz
    TL1 = st;
    ET1 = 0;   // ��ֹ��ʱ��1�ж�
    TR1 = 1;   // TR1:  timer 1 ��
    EA = 1;   //�����ж�
    ES = 1;   //�򿪴����ж�
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

// ��������������
void turn_on() {
    if (lock) {   // ���Ѿ��򿪣���������
        SendStr("�������������ظ�����\r\n\0");
    } else {   // δ������������
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            DelayMs(50);
        }
        motor = ~0;
        SendStr("�ɹ�����\r\n\0");
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
        SendStr("�ɹ�����\r\n\0");
    } else {   // δ�������������
        SendStr("�ѿ����������ظ�����\r\n\0");
    }
}

u8 judge() {
    u8 i = 0;
    while (i < 6) {
        if (input[i] == password[i]) {
            input[i] = '\0';
//            input[i] = '0';
            i++;
        } else break;
    }
    return i == 6;
}

int main() {
    InitUART(0, 0xFD);
    input[6] = '\0';
    while (1) {
        // ���°���������ȷ�ϣ���ȷ���ţ�����/������������
        if (!key) {
            idx = 0;
            if (judge()) {
                turn_off();
            } else {
                turn_on();
            }
            DelayMs(250);
        }
    }
    return 0;
}

void UART_SER()

__interrupt(4) //�����жϷ������
{

if(RI)                        //�ж��ǽ����жϲ���
{
ES = 0;
input[idx++] =
SBUF;                 //���뻺������ֵ
idx%=6;
RI = 0;
ES = 1;
}
return;
}