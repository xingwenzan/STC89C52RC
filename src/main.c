//
// Created by 86159 on 2023-05-07.
//
#include "mcs51/reg51.h"

#define LED P2   // �ҵĵ�Ƭ���� p2 ʵ��ʱ������Ӧ��

unsigned char offNum = 3;   // ��ˮ�ƹرգ������ܹ��߼�Ȧ�������м�Ȧû����
unsigned char initLED;   // ��¼��ʼ״̬/һȦ�����һ��������״̬�����������ܹ��߼�Ȧ
unsigned char move;   // �������ĸ���

// ��ʼ�� LED
void init_led(unsigned char i) {
    LED = ~0;   // �ʼ���е�ȫ��
    // move��initLED ����Ӧ�ڿ�ʼ�ĵƵ�ǰһ������һȦ���������һ��
    move = (i + 7) % 8;    // (i-1+8)%8 ������
    initLED = ~(1 << move);
}

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

// ��ˮ���ƶ�
void waterLamp() {
    LED = offNum ? ~(1 << (++move % 8)) : ~0;   // Loading flash: 458 bytes (Intel HEX)
// Loading flash: 461 bytes (Intel HEX)
//    if (offNum){
//        LED =  ~(1<<(++move%8));
//    } else{
//        LED = ~0;
//        EA = 0;
//    }
    if (LED == initLED)offNum--;
}

int main() {
    init_led(0);
    init_time0();
    while (1);
    return 0;
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

waterLamp();

}
}