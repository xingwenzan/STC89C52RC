#include"mcs51/reg51.h"    //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���

typedef unsigned int u16;
typedef unsigned char u8;

const u8 segout[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //8��
const u8 tab[] = {
        0x00, 0x7F, 0x49, 0x49, 0x49, 0x7F, 0x00, 0x00,   // ��
        0x77, 0x11, 0x11, 0xFF, 0x11, 0x11, 0x77, 0x00,   // ��
        0x4A, 0x5A, 0x69, 0xDF, 0x48, 0x4A, 0x0A, 0x00,   // ��
        0x43, 0x4C, 0xF0, 0xD0, 0x51, 0x5F, 0x40, 0x00,   // ��
        0x00, 0x00, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x0C,   // ��
        0x00, 0x3F, 0x01, 0x61, 0x01, 0x19, 0x03, 0x00,   // ��
        0x3F, 0x60, 0xAE, 0x2A, 0x2E, 0x21, 0x3F, 0x00,   // ��
        0x11, 0x52, 0x3C, 0xF0, 0x3F, 0x51, 0x13, 0x00,   // ��
        0x7C, 0x54, 0x7D, 0x02, 0xFC, 0xA9, 0xFF, 0x00,   // ��
        0x00, 0x00, 0x07, 0x05, 0x07, 0x00, 0x00, 0x00,   // ��
};  // ͼ��

// Ӳ���˿ڶ���
// �������źſ���
#define LATCH_A P1_0
#define SRCLK_A P1_1
#define SER_A P1_2

// �������źſ���
#define LATCH_B P2_2
#define SRCLK_B P2_1
#define SER_B P2_0

u8 j = 0;   // ��λ����

// ��ʼ����ʱ��0
void init_time0() {
    TMOD |= 0X01;   // ѡ��Ϊ��ʱ��0ģʽ��������ʽ1
    // ����ʱ������ֵ����ʱ1ms
    // �ҵ��� 11.0592MHz �ģ�C = [1ms/MC]�� = [1ms*11.0592MHz/12]�� = [921.6]�� = [399.99����H]�� �� [400H]�� = FC66
    // ���� 12MHz,C = [1ms*12MHz/12]�� = [1000]�� = [3E8H]�� = FC18
    TH0 = 0XFC;
    TL0 = 0X66;

    ET0 = 1;   // �򿪶�ʱ��0�ж�����
    EA = 1;   // �����ж�
    TR0 = 1;   // �򿪶�ʱ��
}

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// ��ʱ���� ��λԼΪ����
void DelayMs(u16 t) {
    for (u8 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}

// �����ֽڳ���
void SendByte(u8 dat) {
    for (u8 i = 0; i < 8; i++) {
        SRCLK_A = 0;
        SER_A = dat & 0x80;
        dat <<= 1;
        SRCLK_A = 1;
    }

}

// ����˫�ֽڳ���
// 595������n��595������Ҫ����n�ֽں�����
void Send2Byte(u8 dat1, u8 dat2) {
    SendByte(dat1);
    SendByte(dat2);
}

// 595�������
// 595�����������ݺ�������Ч
void Out595() {
    LATCH_A = 1;
//    delay(1);
    LATCH_A = 0;
}

//  ����λ���ֽڳ���
// ʹ������һƬ����595
void SendSeg(u8 dat) {
    for (u8 i = 0; i < 8; i++)  //�����ֽ�
    {
        SRCLK_B = 0;
        SER_B = dat & 0x80;
        dat <<= 1;
        SRCLK_B = 1;
    }
    LATCH_B = 1;    //����
//    delay(1);
    LATCH_B = 0;
}

int main() {
    init_time0();
    while (1) {
        for (u8 i = 0; i < 8; i++) //8����ʾ
        {
            SendSeg(segout[i]);

            Send2Byte(~tab[(j + i) % 80], 0xff);   // ��ɫ
//            Send2Byte(0xff, ~tab[i]);   // ��ɫ
//            Send2Byte(~tab[i], ~tab[i]);   // ˫ɫ
            Out595();
            DelayMs(1);

            Send2Byte(0xff, 0xff);//��ֹ��Ӱ
            Out595();
        }

    }
}


// ��ʱ����ʱ������ʱ�䣬����ɻ�
void time0()

__interrupt(1) {
static unsigned int cnt = 0;   // ��ʱ������
//����ʱ������ֵ����ʱ1ms
TH0 = 0XFC;
TL0 = 0X66;
// 1000 * 1ms = 1s
// ��ѡ��ֱ�� 1s ��ʱһ��ԭ�� C = [1s/MC]�� = [921600]��  51 ��ʱ���޷�ֱ����� 1s ��ʱ
cnt++;
if (cnt == 1000) {
cnt = 0;

//j+=8;   // ��֮���ֻ�
j++;   // ��һ����ֻ�

}
}