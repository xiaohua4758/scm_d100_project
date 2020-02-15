/***************************************  File Info **********************************************
** File name:			myfunction.h
** Descriptions:
** Created by:
** Created date:
** Change date:
*************************************************************************************************/

#define enableInterrupts()      (set_EA)
#define disableInterrupts()     (clr_EA)


/* bit define */
#define BIT0  	(0X01)
#define BIT1  	(0X02)
#define BIT2  	(0X04)
#define BIT3  	(0X08)
#define BIT4  	(0X10)
#define BIT5  	(0X20)
#define BIT6  	(0X40)
#define BIT7  	(0X80)
#define BIT8  	(0X100)

/* bit define */
#define NEG_BIT0  	(0xfe)
#define NEG_BIT1  	(0xfd)
#define NEG_BIT2  	(0xfb)
#define NEG_BIT3  	(0xf7)
#define NEG_BIT4  	(0xef)
#define NEG_BIT5  	(0xdf)
#define NEG_BIT6  	(0xbf)
#define NEG_BIT7  	(0x7f)


#define CHECK_KEY(a)			((a==0)?(delay(10),(a==0?1:0)):0)	//防抖

/* 数据存放地址 */
/* SYS1 PARA EEPROM ADDRESS */
#define SYS_TIME_UNIT				(0x4780)    //全APROM时的最后一页
#define SYS_TIME_VALUE_H			(SYS_TIME_UNIT+1)
#define SYS_TIME_VALUE_L			(SYS_TIME_VALUE_H+1)
#define SYS_SHOCK_VALUE				(SYS_TIME_VALUE_L+1)

typedef struct SYSTEM_PARAMETER_STR
{
    u16 delay_time;			// 取值 1-6000
    u8 shock_level;			//
    u8 delay_time_unit;		// 0-second, 1-min, 2-hour

    u8 is_config;			// 0-no config, 1-be config, 2-read out success

    u8 set_mode;			// 1-unit ,2-delay time value, 3-shock level
    u8 sys_mode;			// 0-set mode, 1-read back mode

    u8 is_connect;			// 0-no connet, 1-connect

	//备份
	u16 backup_delay_time;         // 取值 1-6000
	u8 backup_shock_level;         //
	u8 backup_delay_time_unit;     // 0-second, 1-min, 2-hour

	//读回
	u16 rd_delay_time;         // 取值 1-6000
	u8 rd_shock_level;         //
	u8 rd_delay_time_unit;     // 0-second, 1-min, 2-hour
}system_parameter;

extern system_parameter sys_para;
// lcd显示开关结构体,均为显示标志位,均为一位数据,与系统参数相对应
typedef struct LCD_DISPLAY_FLAG
{
	u8 sys_mode_name;
	u8 config_name;
	u8 connect_name;

	u8 delay_unit;			//单位
	u8 delay_name;
	u8 shock_name;

	u8 delay_val;
	u8 shock_val;

	u8 global_refresh;
}lcd_show_flags;
extern lcd_show_flags lcd_show;		//系统显示标志

#define FAILED      (0)
#define SUCCESS     (1)

#define CMD_GET_INFO	0x01
#define CMD_SET_INFO	0x02

//frame: HEAD(1bit) + LEN(1bit) + DATA(nbit) + LRC
#define	FRAME_STX		0xaa			//head
//--------------- state -----------------
#define U_WAIT_HEAD		0
#define U_WAIT_LEN		1
#define U_WAIT_DATA		2
#define U_WAIT_LRC		3
//-------------- buf len ----------------
#define RCV_BUF_LEN		10
//--------------- flag  -----------------
#define UCOM_RCV_NULL	    0x00	//无数据
#define UCOM_RCV_OK		    0x01	//接收OK
#define UCOM_RCV_TIMEOUT	0x02	//接收超时
#define UCOM_RCV_ERR	    0x03	//接收ERR
typedef struct UART_RCV_STRUCT
{
	u8 sta;			//状态机状态
	u8 flag;		//一帧接收完标志
	u8 exp_len;		//期望长度

	//帧组成
	u8 cnt;			//串口实际收到的数据个数
	u8 dat[RCV_BUF_LEN];

	u16 out_time;
}uart_rcv_str;
extern uart_rcv_str uart_rcv;	//向外声明的串口数据接收

#define SCM_RST_LOW		(P11=0)
#define SCM_RST_HIGH	(P11=1)

void delay(unsigned int t);
void init_uart0(u32 bdr , u8 tx_or_rx);
void io_init(void);
void sent_frame(u8 *dat,u8 len);
u8 sent_cmd(u8 *out_buf,u8 tlen, u8 *rcv_buf, u8 *rlen, u16 outtime);
void hook_save_100ms(u8 time);
void save_proc(void);
void clear_uart_cmd(void);
void connect_service(void);
void warnning_unconnect(void);
u8 readout_data(void);

