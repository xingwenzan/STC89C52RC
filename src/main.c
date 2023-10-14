//
// Created by 86159 on 2023-10-09.
// �������ʾ��ĸ https://blog.csdn.net/weixin_41413511/article/details/102777228
//
#include "mcs51/reg51.h"

// -------------------------������ɾ------------------------
typedef unsigned int u16;
typedef unsigned char u8;
// ���ֽṹ�壬�������ֵĵ����֣��磺��-zhang��ܰ-xin����-wne
typedef struct {
    u8 name[5];   // ������һ����ĸ��һ������ܣ�����಻�ᳬ�� 5 ����һ�������ֻ��Ҫһ���ֽڱ�ʾ
} NAME;

/**
 * SMG ��ѡ ���Ƶ����������ʾɶ
 * A_DP һ��������� 8 �� LED ��ɣ��ֱ�Ϊ A B C D E F G DP(С����)
 * PORT �˿�
 * g ȫ�ֱ���
 */
#define SMG_A_DP_PORT P0
// ����������
#define LSA P2_2   // ǰ���Թ��������������
#define LSB P2_3   // �����Թ��������������
// ���󰴼��˿�
#define KEY_PORT P1

/**
 * ���� 0-9��a-z���ո� �����
 * ��һ���� 0-9
 * ���һ���ǿո�
 * ������ a-z
 * �������� a-z   gsmg_code[goal-'a'+10]   gaol ��Ŀ����ĸ
 */
const u8 gsmg_code[37] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,   // 0-9
                          0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, 0x76, 0x10,   // a-i
                          0x0e, 0x7a, 0x38, 0x55, 0x54, 0x5c, 0x73, 0x67, 0x50,   // j-r
                          0x64, 0x78, 0x3e, 0x62, 0x6a, 0x36, 0x6e, 0x49, 0x00};   // s-z �ո�

/**
 * �ҵĵ�Ƭ���ǹ����� DP �����λ
 * ʵ�����豸�ǹ����� DP �����λ
 * ʵ��ʵ���赥����ȡ������
 */
NAME names[4][4];
u8 x = 3, y = 3;   // ��������

// ���ֳ�ʼ��
void name_init() {
    for (u8 i = 0; i < 4; ++i) {
        for (u8 j = 0; j < 4; ++j) {
            for (u8 k = 0; k < 5; ++k) {
                names[i][j].name[k] = 'z' + 1;
            }
        }
    }

    // zhang
    names[0][0].name[0] = 'z';
    names[0][0].name[1] = 'h';
    names[0][0].name[2] = 'a';
    names[0][0].name[3] = 'n';
    names[0][0].name[4] = 'g';
    // xin
    names[0][1].name[0] = 'x';
    names[0][1].name[1] = 'i';
    names[0][1].name[2] = 'n';
    // wen
    names[0][2].name[0] = 'w';
    names[0][2].name[1] = 'e';
    names[0][2].name[2] = 'n';

    // yao
    names[1][0].name[0] = 'y';
    names[1][0].name[1] = 'a';
    names[1][0].name[2] = 'o';
    // shun
    names[1][1].name[0] = 's';
    names[1][1].name[1] = 'h';
    names[1][1].name[2] = 'u';
    names[1][1].name[3] = 'n';
    // yu
    names[1][2].name[0] = 'y';
    names[1][2].name[1] = 'u';

    // han
    names[2][0].name[0] = 'h';
    names[2][0].name[1] = 'a';
    names[2][0].name[2] = 'n';
    // he
    names[2][1].name[0] = 'h';
    names[2][1].name[1] = 'e';
    // zhi
    names[2][2].name[0] = 'z';
    names[2][2].name[1] = 'h';
    names[2][2].name[2] = 'i';

    // chen
    names[3][0].name[0] = 'c';
    names[3][0].name[1] = 'h';
    names[3][0].name[2] = 'e';
    names[3][0].name[3] = 'n';
    // jia
    names[3][1].name[0] = 'j';
    names[3][1].name[1] = 'i';
    names[3][1].name[2] = 'a';
    // qi
    names[3][2].name[0] = 'q';
    names[3][2].name[1] = 'i';
}

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

/**
 * ������ȡ������ֱ�Ӹ�ֵ�� x,y
 * �ҵĵ�Ƭ����λ����(x) ��λ����(y)   7-4 ��Ӧ 0-3 ��   4-0 ��Ӧ 0-3 ��
 * �͵�ƽ��ͨ
 */
void keyScan() {
    KEY_PORT = 0x0f;   // ��λ�����ź�
    if (KEY_PORT != 0x0f) {   // �б仯
        delay(1000);   // ����
        for (u8 i = 0; i < 4; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                y = 3 - i;
                break;
            }
        }
        KEY_PORT = 0xf0;   // ����λ
        delay(1000);   // ����
        for (u8 i = 4; i < 8; ++i) {
            if (!((KEY_PORT >> i) & 1)) {
                x = 7 - i;
                break;
            }
        }
    }


}

// ɨ�貢��ʾ
void keyDisplay() {
    // ɨ��
    keyScan();

    // ��ʾ
    for (u8 i = 0; i < 5; ++i) {
        // λѡ���ҵ�����ܴ������� 7-0 �ţ�����ϰ�ߣ���ʹ�� 7-i��
        // ѡ���Թ���
        if (i >= 4) {
            LSB = 1;
            LSA = 0;
        } else {
            LSA = 1;
            LSB = 0;
        }
        // ѡ���Թ�
        SMG_A_DP_PORT = 1 << (i - LSA * 4);
        delay(100);
        // ��ѡ
        SMG_A_DP_PORT = gsmg_code[names[x][y].name[i] - 'a' + 10];
        // ��ʱ
        delay(100);
        // ���� ��������һλӰ��
        SMG_A_DP_PORT = 0x00;
    }
}

int main() {
    // ��ʼ��
    name_init();
    // ѭ��ɨ�貢��ʾ
    while (1)keyDisplay();
    return 0;
}
