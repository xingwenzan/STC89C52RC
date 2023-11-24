#include "mcs51/reg51.h"

//typedef unsigned long u32;
typedef unsigned int u16;
typedef unsigned char u8;

//LCD1602���ݿ�4λ��8λ���壬��Ϊ1����ΪLCD1602��λ���ݿ���������֮Ϊ8λ
#define LCD1602_4OR8_DATA_INTERFACE    0    //Ĭ��ʹ��8λ���ݿ�LCD1602
//�ܽŶ���
#define LCD1602_RS P2_6   //��������ѡ��
#define LCD1602_RW P2_5   //��дѡ��
#define LCD1602_E P2_7   //ʹ���ź�
#define LCD1602_DATAPORT P3      //�궨��LCD1602���ݶ˿�
#define KEY_PORT P1   // keypad ����
#define BUZZER P0_7   // ����������
#define motor P0   // �������

const u8 fan[] = {0x09, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01};
const u8 zheng[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};

u8 keyValue[] = "0123456789LRBDCE\0";
u8 keypad = 0;   // ��ǰ��λ
u8 haveKey = 0;   // �Ƿ��а�������
u8 password[] = "123456\0";   // ����
u8 input[7], idx = 0;
u8 lock = 0;   // ����״̬   0 ��δ������������   1 ��������������

// ��ʱ����λ������ 10 ΢�룬ÿ���� 1����Լ��ʱ 10us
void delay(u16 ten_us) {
    while (ten_us--);
}

// ��ʱ���� ��λԼΪ����
void delay_ms(u16 t) {
    for (u16 i = 0; i < t; ++i)
        for (u8 j = 0; j < 110; ++j);
}


/*******************************************************************************
* �� �� ��       : lcd1602_write_cmd
* ��������		 : LCD1602д����
* ��    ��       : cmd��ָ��
* ��    ��    	 : ��
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8λLCD

void lcd1602_write_cmd(u8 cmd) {
    LCD1602_RS = 0;//ѡ������
    LCD1602_RW = 0;//ѡ��д
    LCD1602_E = 0;
    LCD1602_DATAPORT = cmd;//׼������
    delay_ms(1);
    LCD1602_E = 1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E = 0;//ʹ�ܽ�E���������д��
}

#else	//4λLCD
void lcd1602_write_cmd(u8 cmd)
{
    LCD1602_RS=0;//ѡ������
    LCD1602_RW=0;//ѡ��д
    LCD1602_E=0;
    LCD1602_DATAPORT=cmd;//׼������
    delay_ms(1);
    LCD1602_E=1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E=0;//ʹ�ܽ�E���������д��

    LCD1602_DATAPORT=cmd<<4;//׼������
    delay_ms(1);
    LCD1602_E=1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E=0;//ʹ�ܽ�E���������д��
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_write_data
* ��������		 : LCD1602д����
* ��    ��       : dat������
* ��    ��    	 : ��
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8λLCD

void lcd1602_write_data(u8 dat) {
    LCD1602_RS = 1;//ѡ������
    LCD1602_RW = 0;//ѡ��д
    LCD1602_E = 0;
    LCD1602_DATAPORT = dat;//׼������
    delay_ms(1);
    LCD1602_E = 1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E = 0;//ʹ�ܽ�E���������д��
}

#else
void lcd1602_write_data(u8 dat)
{
    LCD1602_RS=1;//ѡ������
    LCD1602_RW=0;//ѡ��д
    LCD1602_E=0;
    LCD1602_DATAPORT=dat;//׼������
    delay_ms(1);
    LCD1602_E=1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E=0;//ʹ�ܽ�E���������д��

    LCD1602_DATAPORT=dat<<4;//׼������
    delay_ms(1);
    LCD1602_E=1;//ʹ�ܽ�E��������д��
    delay_ms(1);
    LCD1602_E=0;//ʹ�ܽ�E���������д��
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_init
* ��������		 : LCD1602��ʼ��
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
#if (LCD1602_4OR8_DATA_INTERFACE == 0)//8λLCD

void lcd1602_init(void) {
    lcd1602_write_cmd(0x38);//��������8λ����ʾ2�У�5*7����/�ַ�
    lcd1602_write_cmd(0x0e);//��ʾ���ܿ����й�꣬�����˸
    lcd1602_write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
    lcd1602_write_cmd(0x01);//����
}

#else
void lcd1602_init(void)
{
    lcd1602_write_cmd(0x28);//��������4λ����ʾ2�У�5*7����/�ַ�
    lcd1602_write_cmd(0x0c);//��ʾ���ܿ����޹�꣬�����˸
    lcd1602_write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
    lcd1602_write_cmd(0x01);//����
}
#endif

/*******************************************************************************
* �� �� ��       : lcd1602_clear
* ��������		 : LCD1602����
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd1602_clear(void) {
    lcd1602_write_cmd(0x01);
}

/*******************************************************************************
* �� �� ��       : lcd1602_show_string
* ��������		 : LCD1602��ʾ�ַ�
* ��    ��       : x,y����ʾ���꣬x=0~15��y=0~1;
				   str����ʾ�ַ���
* ��    ��    	 : ��
*******************************************************************************/
void lcd1602_show_string(u8 x, u8 y, u8 *str) {
    u8 i = 0;

    if (y > 1 || x > 15)return;//���в���������ǿ���˳�

    if (y < 1)    //��1����ʾ
    {
        while (*str != '\0')//�ַ�������'\0'��β��ֻҪǰ�������ݾ���ʾ
        {
            if (i < 16 - x)//����ַ����ȳ�����һ����ʾ��Χ�����ڵڶ��м�����ʾ
            {
                lcd1602_write_cmd(0x80 + i + x);//��һ����ʾ��ַ����
            } else {
                lcd1602_write_cmd(0x40 + 0x80 + i + x - 16);//�ڶ�����ʾ��ַ����
            }
            lcd1602_write_data(*str);//��ʾ����
            str++;//ָ�����
            i++;
        }
    } else    //��2����ʾ
    {
        while (*str != '\0') {
            if (i < 16 - x) //����ַ����ȳ����ڶ�����ʾ��Χ�����ڵ�һ�м�����ʾ
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

// ��������
void buzzer_on() {
    BUZZER = 1;
}

// ��������
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

// ��������������
void turn_on() {
    if (lock) {   // ���Ѿ��򿪣���������
        buzzer_on();
    } else {   // δ������������
        lock = 1;
        for (u8 i = 0; i < 8; i++) {
            motor = zheng[i];
            delay_ms(50);
        }
        motor = ~0;
        buzzer_off();
    }
}

// �ر�����������
void turn_off() {
    if (lock) {    // ����������Ҫ����
        lock = 0;
        for (u8 i = 0; i < 8; i++) {
            motor = fan[i];
            delay_ms(50);
        }
        motor = ~0;
    } else {   // δ�������������

    }
}

//�������
void left() {
    lcd1602_write_cmd(0x10);
    idx = (idx - 1 + 6) % 6;
}

// �������
void right() {
    lcd1602_write_cmd(0x14);
    idx = (idx + 1) % 6;
}

// ���
void clear() {
    for (int i = 0; i < 6; ++i) {
        input[i] = '\0';
    }
    idx = 0;
}

// �жϲ�����
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

// ���Լ��� delete ɾ����ǰλ�ü����ҵ��������ݣ�����������ڵ�ǰλ�ã��൱����պ�������ݣ�
void delete() {
    for (int i = idx; i < 6; ++i) {
        input[i] = '\0';
    }
//    input[input[7]] = 10;
}

// ���Լ��� backspace ɾ����ǰλ�õ����ݣ��������ص���β���൱��������������һλ��
void backspace() {
    for (int i = idx; i < 6; ++i) {
        input[i] = input[i + 1];
    }
//    input[input[7]] = 10;
//    left();
}

// ��ȫ��ʾ
void display() {
    lcd1602_clear();
    lcd1602_show_string(0, 1, keyValue);
    lcd1602_show_string(0, 0, input);
}

// ������ȡ�������Ϊ
void op() {
//    if (!haveKey)return;// �ް������£�������
//    haveKey = 0;
    switch (keypad) {
        case 10:   // �������
            left();
            break;
        case 11:   // �������
            right();
            break;
        case 12:   // ���Լ��� backspace
            backspace();
            display();
            break;
        case 13:   // ���Լ��� delete
            delete();
            display();
            break;
        case 14:   // ���
            clear();
            display();
            break;
        case 15:   // �س�
            judge();
            display();
            break;
        default:   // 0-9 ��������
            input[idx++] = keyValue[keypad];
            idx %= 6;
            display();
            break;
    }
}

/**
 * ������ȡ������ֱ�Ӹ�ֵ�� x,y
 * �ҵĵ�Ƭ����λ����(x) ��λ����(y)   7-4 ��Ӧ 0-3 ��   4-0 ��Ӧ 0-3 ��
 * �͵�ƽ��ͨ
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
        // ���а������£��Ų���
        if (haveKey) {
            haveKey = 0;
            op();
//            display();
        }

    }
}
