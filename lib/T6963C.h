/*T6963C指令*/
#ifndef __T6963C_H__
#define __T6963C_H__

/*一、指针设置指令		D1 D2 0 0 1 0 0 N2 N1 N0*/
#define CUR_POS 0x21    /*光标指针设置 D1=水平位置(低7位有效) D2=垂直位置(低5位有效) N0=1*/
#define CGR_POS 0x22    /*CGRAM偏置地址设置 D1=地址(低5位有效) D2=00H N1=1*/
#define ADR_POS 0x24    /*地址指针位置 D1=低字节 D2=高字节 N2=1*/

/*二、显示区域设置		D1 D2 0 1 0 0 0 0 N1 N0*/
#define TXT_STP 0x40    /*文本区首址 D1=低字节 D2=高字节 N1=0 N0=0*/
#define TXT_WID 0x41    /*文本区宽度(字节数/行) D1=字节数 D2=00H N1=0 N0=1*/
#define GRH_STP 0x42    /*图形区首址 D1=低字节 D2=高字节 N1=1 N0=0*/
#define GRH_WID 0x43    /*图形区宽度(字节数/行) D1=字节数 D2=00H N1=1 N0=1*/

/*三、显示方式设置		无参数 1 0 0 0 N3 N2 N1 N0*/
/*N3字符发生器选择位：N3=1为外部字符发生器有效 N3=0为CGROM即内部字符发生器有效*/
#define MOD_OR  0x80    /*逻辑“或”合成 N2=0 N1=0 N0=0*/
#define MOD_XOR 0x81    /*逻辑“异或”合成 N2=0 N1=0 N0=1*/
#define MOD_AND 0x83    /*逻辑“与”合成 N2=0 N1=1 N0=1*/
#define MOD_TCH 0x84    /*文本特征 N2=1 N1=0 N0=0*/

/*四、显示开关			无参数 1 0 0 1 N3 N2 N1 N0*/
#define DIS_SW  0x90    /*显示开关*/
/*N0=1/0 光标闪烁启用/禁用*/
/*N1=1/0 光标显示启用/禁用*/
/*N2=1/0 文本显示启用/禁用*/
/*N3=1/0 图形显示启用/禁用*/

/*五、光标形状选择		无参数 1 0 1 0 0 N2 N1 N0*/
#define    CUR_SHP 0xA0    /*光标形状选择：0xA0-0xA7表示光标占的行数*/

/*六、数据自动读、写方式设置	无参数 1 0 1 1 00 N1 N0*/
#define AUT_WR  0xB0    /*自动写设置 N1=0 N0=0*/
#define AUT_RD  0xB1    /*自动读设置 N1=0 N0=1*/
#define AUT_WO  0xB2    /*自动写结束 N1=1 N0=0*/
#define AUT_RO  0xB3    /*自动读结束 N1=1 N0=1*/

/*七、数据一次读、写方式	D1 1 1 0 0 0 N2 N1 N0*/
/*D1为需要写的数据，读时无此数据*/
#define    INC_WR  0xC0    /*数据写，地址加1	N2=0 N1=0 N0=0*/
#define    INC_RD  0xC1    /*数据读，地址加1	N2=0 N1=0 N0=1*/
#define    DEC_WR  0xC2    /*数据写，地址减1	N2=0 N1=1 N0=0*/
#define    DEC_RD  0xC3    /*数据读，地址减1	N2=0 N1=1 N0=1*/
#define    NOC_WR  0xC4    /*数据写，地址不变	N2=1 N1=0 N0=0*/
#define    NOC_RD  0xC5    /*数据读，地址不变	N2=1 N1=0 N0=1*/

/*八、屏读				无参数 1 1 1 0 0 0 0 0*/
#define    SCN_RD  0xE0    /*屏读*/

/*九、屏拷贝			无参数 1 1 1 0 1 0 0 0*/
#define    SCN_CP  0xE8    /*屏拷贝*/

/*十、位操作			无参数 1 1 1 1 N3 N2 N1 N0*/
/*N3=1置1 N3=0清零*/
#define    BIT_OP  0xF0    /*位操作*/
#endif