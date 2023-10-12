//
// Created by 86159 on 2023-10-12.
//
#include "mcs51/reg51.h"

#define beep P2_5

typedef unsigned char u8;
typedef unsigned int u16;


//生日快乐歌的音符频率表，不同频率由不同的延时来决定
const u8 SONG_TONE[] = {212, 212, 190, 212, 159, 169, 212, 212, 190, 212, 142, 159, 212, 212, 106, 126, 159, 169, 190,
                        119, 119, 126, 159, 142, 159, 0};

//生日快乐歌节拍表，节拍决定每个音符的演奏长短
const u8 SONG_LONG[] = {9, 3, 12, 12, 12, 24, 9, 3, 12, 12, 12, 24, 9, 3, 12, 12, 12, 12, 12, 9, 3, 12, 12, 12, 24, 0};

void delay_ms(u16 x)//毫秒延时函数
{
    u16 i, j;
    for (i = 0; i < x; i++)
        for (j = 0; j < 115; j++);
}

void delay(u8 us) {
    while (us--);
}

void sound(unsigned int freq, unsigned int time) {
    u16 i;
    for (i = 0; i < time * 18; i++) {
        beep = 1;
        delay(freq);
        beep = 0;
        delay(freq);
    }
}

void PlayMusic() {
//    beep = 0;
    u16 i = 0;
    while (SONG_LONG[i] != 0 || SONG_TONE[i] != 0) {    //播放各个音符，SONG_LONG为拍子长度
        sound(SONG_TONE[i], SONG_LONG[i]);
//        delay(100);
        delay_ms(10);
        i++;
    }
}

int main() {
    PlayMusic();    //播放生日快乐
//    delay_ms(10000);
    while (1);        //播放完后停止
}
