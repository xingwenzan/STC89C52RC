#include"mcs51/reg51.h"

typedef unsigned int u16;
typedef unsigned char u8;

// ���󰴼��˿�
#define KEY_PORT P1

u8 out[] = "0\n\0"
           "1\n\0"
           "2\n\0"
           "3\n\0"
           "4\n\0"
           "5\n\0"
           "6\n\0"
           "7\n\0"
           "8\n\0"
           "9\n\0"
           "a\n\0"
           "b\n\0"
           "c\n\0"
           "d\n\0"
           "e\n\0"
           "f\n\0";
u8 point = -1;   // ��������
u8 change = 0;

// ���ڳ�ʼ��
void InitUART() {
//    PCON = 1<<7;   // SMOD Ϊ 1���������� > 14400 ʱ����
    SCON = 0x40;   // SCON: ģʽ 1, 8-bit UART, ʹ�ܽ��ս�ֹ
    TMOD |= 0x20;   // TMOD: timer 1, mode 2, 8-bit ��װ
    TH1 = 0xFD;   // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;   // TR1:  timer 1 ��
    EA = 1;   //�����ж�
    //ES = 1;   //�򿪴����ж�
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
        change = 1;
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

    InitUART();

    while (1) {
        keyScan();
//        SendStr("11");
        if (change) {
//            SendStr("1212121");
            change = 0;
            SendStr(out + point * 3);
        }

        DelayMs(240);//��ʱѭ������
        DelayMs(240);
    }
}