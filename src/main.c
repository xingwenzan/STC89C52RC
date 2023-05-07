#include "reg51.h"

#define LED P2

void delay(int us) {   // ��ʱ����λ������΢��
    while (us--);
}

void waterLamp(int num, int us) {   // ��ˮ��
    /*
     * num:��ˮ�Ƶ���
     * us:���೤ʱ��
     */
    LED = 1;   // ��ʼ����ȫ����
    for (int i = 0; i < num; ++i) {
        LED = ~(1 << i);
        delay(us);
    }
}

int main() {
    waterLamp(8, 1000000);
    return 0;
}
