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


#define CHECK_KEY(a)			((a==0)?(delay(10),(a==0?1:0)):0)	//����

/* ���ݴ�ŵ�ַ */
/* SYS1 PARA EEPROM ADDRESS */
#define SYS_TIME_UNIT				(0x4780)    //ȫAPROMʱ�����һҳ
#define SYS_TIME_VALUE_H			(SYS_TIME_UNIT+1)
#define SYS_TIME_VALUE_L			(SYS_TIME_VALUE_H+1)
#define SYS_SHOCK_VALUE				(SYS_TIME_VALUE_L+1)

typedef struct SYSTEM_PARAMETER_STR
{
    u16 delay_time;			// ȡֵ 1-6000
    u8 shock_level;			//
    u8 delay_time_unit;		// 0-second, 1-min, 2-hour

    u8 is_config;			// 0-no config, 1-be config, 2-read out success

    u8 set_mode;			// 1-unit ,2-delay time value, 3-shock level
    u8 sys_mode;			// 0-set mode, 1-read back mode

    u8 is_connect;			// 0-no connet, 1-connect

	//����
	u16 backup_delay_time;         // ȡֵ 1-6000
	u8 backup_shock_level;         //
	u8 backup_delay_time_unit;     // 0-second, 1-min, 2-hour

	//����
	u16 rd_delay_time;         // ȡֵ 1-6000
	u8 rd_shock_level;         //
	u8 rd_delay_time_unit;     // 0-second, 1-min, 2-hour
}system_parameter;

extern system_parameter sys_para;
// lcd��ʾ���ؽṹ��,��Ϊ��ʾ��־λ,��Ϊһλ����,��ϵͳ�������Ӧ
typedef struct LCD_DISPLAY_FLAG
{
	u8 sys_mode_name;
	u8 config_name;
	u8 connect_name;

	u8 delay_unit;			//��λ
	u8 delay_name;
	u8 shock_name;

	u8 delay_val;
	u8 shock_val;

	u8 global_refresh;
}lcd_show_flags;
extern lcd_show_flags lcd_show;		//ϵͳ��ʾ��־

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
#define UCOM_RCV_NULL	    0x00	//������
#define UCOM_RCV_OK		    0x01	//����OK
#define UCOM_RCV_TIMEOUT	0x02	//���ճ�ʱ
#define UCOM_RCV_ERR	    0x03	//����ERR
typedef struct UART_RCV_STRUCT
{
	u8 sta;			//״̬��״̬
	u8 flag;		//һ֡�������־
	u8 exp_len;		//��������

	//֡���
	u8 cnt;			//����ʵ���յ������ݸ���
	u8 dat[RCV_BUF_LEN];

	u16 out_time;
}uart_rcv_str;
extern uart_rcv_str uart_rcv;	//���������Ĵ������ݽ���

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

