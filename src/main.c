#include "mcs51/reg51.h"

//typedef unsigned long u32;
typedef unsigned int u16;
typedef unsigned char u8;

//LCD1602数据口4位和8位定义，若为1，则为LCD1602四位数据口驱动，反之为8位
#define LCD1602_4OR8_DATA_INTERFACE    0    //默认使用8位数据口LCD1602
//管脚定义
#define LCD1602_RS P2_6   //数据命令选择
#define LCD1602_RW P2_5   //读写选择
#define LCD1602_E P2_7   //使能信号
#define LCD1602_DATAPORT P3      //宏定义LCD1602数据端口
#define KEY_PORT P1   // keypad 引脚
#define BUZZER P0_7   // 蜂鸣器控制
#define motor P0   // 电机引脚

const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};

u8 keyValue[] = "0123456789LRBDCE\0";
u8 keypad = 0;   // 当前键位
u8 haveKey = 0;   // 是否有按键按下
u8 password[] = "123456\0";   // 密码
u8 input[7], idx = 0;
u8 lock = 0;   // 开关状态   0 锁未开启，即开门   1 锁开启，即锁门

// 延时，单位大致是 10 微秒，每传入 1，大约延时 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// 延时函数 单位约为毫秒
void delay_ms(u16 t) {
    for (u16 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}


/*******************************************************************************
* 函 数 名       : lcd1602_write_cmd
* 函数功能		 : LCD1602写命令
* 输    入       : cmd：指令
* 输    出    	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8位LCD

void lcd1602_write_cmd(u8 cmd) {
    LCD1602_RS = 0;//选择命令
    LCD1602_RW = 0;//选择写
    LCD1602_E = 0;
    LCD1602_DATAPORT = cmd;//准备命令
    delay_ms(1);
    LCD1602_E = 1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E = 0;//使能脚E后负跳变完成写入
}

#else	//4位LCD
void lcd1602_write_cmd(u8 cmd)
{
    LCD1602_RS=0;//选择命令
    LCD1602_RW=0;//选择写
    LCD1602_E=0;
    LCD1602_DATAPORT=cmd;//准备命令
    delay_ms(1);
    LCD1602_E=1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E=0;//使能脚E后负跳变完成写入

    LCD1602_DATAPORT=cmd<<4;//准备命令
    delay_ms(1);
    LCD1602_E=1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E=0;//使能脚E后负跳变完成写入
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_write_data
* 函数功能		 : LCD1602写数据
* 输    入       : dat：数据
* 输    出    	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8位LCD

void lcd1602_write_data(u8 dat) {
    LCD1602_RS = 1;//选择数据
    LCD1602_RW = 0;//选择写
    LCD1602_E = 0;
    LCD1602_DATAPORT = dat;//准备数据
    delay_ms(1);
    LCD1602_E = 1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E = 0;//使能脚E后负跳变完成写入
}

#else
void lcd1602_write_data(u8 dat)
{
    LCD1602_RS=1;//选择数据
    LCD1602_RW=0;//选择写
    LCD1602_E=0;
    LCD1602_DATAPORT=dat;//准备数据
    delay_ms(1);
    LCD1602_E=1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E=0;//使能脚E后负跳变完成写入

    LCD1602_DATAPORT=dat<<4;//准备数据
    delay_ms(1);
    LCD1602_E=1;//使能脚E先上升沿写入
    delay_ms(1);
    LCD1602_E=0;//使能脚E后负跳变完成写入
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_init
* 函数功能		 : LCD1602初始化
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8位LCD

void lcd1602_init(void) {
    lcd1602_write_cmd(0x38);//数据总线8位，显示2行，5*7点阵/字符
    lcd1602_write_cmd(0x0e);//显示功能开，有光标，光标闪烁
    lcd1602_write_cmd(0x06);//写入新数据后光标右移，显示屏不移动
    lcd1602_write_cmd(0x01);//清屏
}

#else
void lcd1602_init(void)
{
    lcd1602_write_cmd(0x28);//数据总线4位，显示2行，5*7点阵/字符
    lcd1602_write_cmd(0x0c);//显示功能开，无光标，光标闪烁
    lcd1602_write_cmd(0x06);//写入新数据后光标右移，显示屏不移动
    lcd1602_write_cmd(0x01);//清屏
}
#endif

/*******************************************************************************
* 函 数 名       : lcd1602_clear
* 函数功能		 : LCD1602清屏
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd1602_clear(void) {
    lcd1602_write_cmd(0x01);
}

/*******************************************************************************
* 函 数 名       : lcd1602_show_string
* 函数功能		 : LCD1602显示字符
* 输    入       : x,y：显示坐标，x=0~15，y=0~1;
				   str：显示字符串
* 输    出    	 : 无
*******************************************************************************/
void lcd1602_show_string(u8 x, u8 y, u8 *str) {
    u8 i = 0;

    if (y > 1 || x > 15)return;//行列参数不对则强制退出

    if (y < 1)    //第1行显示
    {
        while (*str != '\0')//字符串是以'\0'结尾，只要前面有内容就显示
        {
            if (i < 16 - x)//如果字符长度超过第一行显示范围，则在第二行继续显示
            {
                lcd1602_write_cmd(0x80 + i + x);//第一行显示地址设置
            } else {
                lcd1602_write_cmd(0x40 + 0x80 + i + x - 16);//第二行显示地址设置
            }
            lcd1602_write_data(*str);//显示内容
            str++;//指针递增
            i++;
        }
    } else    //第2行显示
    {
        while (*str != '\0') {
            if (i < 16 - x) //如果字符长度超过第二行显示范围，则在第一行继续显示
            {
                lcd1602_write_cmd(0x80 + 0x40 + i + x);
            } else {
                lcd1602_write_cmd(0x80 + i + x - 16);
            }
            lcd1602_write_data(*str);
            str++;
            i++;
        }
    }
}

// 蜂鸣器响
void buzzer_on() {
    BUZZER = 1;
}

// 蜂鸣器灭
void buzzer_off() {
    BUZZER = 0;
}

void init() {
    for (int i = 0; i < 7; ++i) {
        input[i] = '\0';
    }
    lcd1602_init();
    lcd1602_show_string(0, 1, keyValue);
    buzzer_off();
}

// 开启锁，即锁门
void turn_on() {
    if (lock) {   // 锁已经打开，即已上锁
        buzzer_on();
    } else {   // 未上锁，需上锁
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            delay_ms(50);
        }
        motor = ~0;
        buzzer_off();
    }
}

// 关闭锁，即开门
void turn_off() {
    if (lock) {    // 已上锁，需要解锁
        lock = 0;
        for (u8 i = 0; i < 8; i++) {
            motor = fan[i];
            delay_ms(50);
        }
        motor = ~0;
    } else {   // 未上锁，无需解锁

    }
}

//光标左移
void left() {
    lcd1602_write_cmd(0x10);
    idx = (idx - 1 + 6) % 6;
}

// 光标右移
void right() {
    lcd1602_write_cmd(0x14);
    idx = (idx + 1) % 6;
}

// 清空
void clear() {
    for (int i = 0; i < 6; ++i) {
        input[i] = '\0';
    }
    idx = 0;
}

// 判断并操作
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

// 电脑键盘 delete 删掉当前位置及以右的所有内容，并将光标留在当前位置（相当于清空后面的内容）
void delete() {
    for (int i = idx; i < 6; ++i) {
        input[i] = '\0';
    }
//    input[input[7]] = 10;
}

// 电脑键盘 backspace 删掉当前位置的内容，并将光标回到结尾（相当于内容整体左移一位）
void backspace() {
    for (int i = idx; i < 6; ++i) {
        input[i] = input[i + 1];
    }
//    input[input[7]] = 10;
//    left();
}

// 完全显示
void display() {
    lcd1602_clear();
    lcd1602_show_string(0, 1, keyValue);
    lcd1602_show_string(0, 0, input);
}

// 按键读取后进行行为
void op() {
//    if (!haveKey)return;// 无按键按下，不操作
//    haveKey = 0;
    switch (keypad) {
        case 10:   // 光标左移
            left();
            break;
        case 11:   // 光标右移
            right();
            break;
        case 12:   // 电脑键盘 backspace
            backspace();
            display();
            break;
        case 13:   // 电脑键盘 delete
            delete();
            display();
            break;
        case 14:   // 清空
            clear();
            display();
            break;
        case 15:   // 回车
            judge();
            display();
            break;
        default:   // 0-9 密码输入
            input[idx++] = keyValue[keypad];
            idx %= 6;
            display();
            break;
    }
}

/**
 * 按键读取函数，直接赋值到 x,y
 * 我的单片机高位是行(x) 低位是列(y)   7-4 对应 0-3 行   4-0 对应 0-3 行
 * 低电平接通
 */
void keyScan() {
    u8 key_num = 0;

    KEY_PORT = 0xf7;
    if (KEY_PORT != 0xf7) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x77:
                key_num = 13;
                break;
            case 0xb7:
                key_num = 14;
                break;
            case 0xd7:
                key_num = 15;
                break;
            case 0xe7:
                key_num = 16;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xf7);

    KEY_PORT = 0xfb;
    if (KEY_PORT != 0xfb) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7b:
                key_num = 9;
                break;
            case 0xbb:
                key_num = 10;
                break;
            case 0xdb:
                key_num = 11;
                break;
            case 0xeb:
                key_num = 12;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xfb);

    KEY_PORT = 0xfd;
    if (KEY_PORT != 0xfd) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7d:
                key_num = 5;
                break;
            case 0xbd:
                key_num = 6;
                break;
            case 0xdd:
                key_num = 7;
                break;
            case 0xed:
                key_num = 8;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xfd);

    KEY_PORT = 0xfe;
    if (KEY_PORT != 0xfe) {
        delay(1000);
        switch (KEY_PORT) {
            case 0x7e:
                key_num = 1;
                break;
            case 0xbe:
                key_num = 2;
                break;
            case 0xde:
                key_num = 3;
                break;
            case 0xee:
                key_num = 4;
                break;
            default:
                break;
        }
    }
    while (KEY_PORT != 0xfe);

    if (key_num) {
        buzzer_off();
        haveKey = 1;
        keypad = key_num - 1;
    }
}

int main() {
    init();
    while (1) {
        keyScan();
        // 仅有按键按下，才操作
        if (haveKey) {
            haveKey = 0;
            op();
//            display();
        }

    }
}
