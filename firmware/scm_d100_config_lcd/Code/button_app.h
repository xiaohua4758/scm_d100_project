/***************************************  File Info **********************************************
** File name:			button.c
** Descriptions:        按键处理函数
** Created by:
** Created date:
*************************************************************************************************/

#define CHECK_KEY_LOW(a)		((!a)?(delay(10),((!a)?1:0)):0)	//检查低电平,防抖
#define CHECK_KEY_HIGH(a)		((a)?(delay(10),(a?1:0)):0)		//检查高电平,防抖

#define KEY_OFF			0		//高电平,空闲
#define KEY_ONCE_ON		1		//下降沿
#define KEY_LONG_ON		2		//低电平
#define KEY_ONCE_UP		5		//上升沿

typedef struct STRUCT_KEY
{
    u8 confirm;
    u8 up;
    u8 down;
    u8 select;		//
    u8 mode;		//read back or write
}struct_key_status;

extern struct_key_status key;

void key_init_sta(void);
void key_detect(void);
void key_proc(void);

