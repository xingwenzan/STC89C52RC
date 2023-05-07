#include "reg51.h"

#define LED P2

void delay(int us) {   // 延时，单位大致是微秒
    while (us--);
}

void waterLamp(int num, int us) {   // 流水灯
    /*
     * num:流水灯灯数
     * us:亮多长时间
     */
    LED = 1;   // 初始化，全不亮
    for (int i = 0; i < num; ++i) {
        LED = ~(1 << i);
        delay(us);
    }
}

int main() {
    waterLamp(8, 1000000);
    return 0;
}
