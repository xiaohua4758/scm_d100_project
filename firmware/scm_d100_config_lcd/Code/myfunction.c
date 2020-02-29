/***************************************  File Info **********************************************
** File name:			myfunction.c
** Descriptions:        按键与信号处理函数,系统处理函数
** Created by:			CSH
** Created date:		2012.02.19
*************************************************************************************************/
#include "N76E003.h"          //包含用到的外设的头文件
#include "string.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "myfunction.h"
#include "timer.h"
#include "flash.h"

volatile u8 usent_ok=0;			//uart
unsigned int save_delay=0;		//保存延时
uart_rcv_str uart_rcv={0};
u8 g_m_connect_sta=0;

/**************************************************************************
*function:  void delay(unsigned int t)
*input:     NULL
*output:    NULL
*describe:  稍长延时
**************************************************************************/
void delay(unsigned int t)
{
	volatile unsigned int i;
	while(t--){
		i=30;
		while(i--); //做一些没有用的事，打发时间
	}
}

//uart sent
void uart0_sent(u8 sdat)
{
    TI = 0;
    usent_ok=0;
    SBUF = sdat;
    while(usent_ok==0);
}

void uart0_sent_buff(const u8 *sdat, u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
		uart0_sent(sdat[i]);
}

void sent_frame(u8 *dat,u8 len)
{
    static u8 buff[3];
    u8 i,lrc;

    if(len==0)
        return;

	//frame: HEAD(1byte) + LEN(1byte) + DATA(nbyte) + LRC
	if(len>90)len=90;
    buff[0]=FRAME_STX;
    buff[1]=len;
    lrc=buff[0]+buff[1];
    for(i=0;i<len;i++)
    {
        lrc+=dat[i];
    }

	uart0_sent_buff(buff,2);	// head+ len
	uart0_sent_buff(dat,len);	// sdata
	uart0_sent(lrc);			// lrc

	uart_rcv.out_time = 1;		// 开始计时
}

void uart_rcv_data(u8 rdat)
{
	u8 i;

    //timeout_chk(&ucom_time_out, 0);
	//frame: HEAD(1bit) + LEN(1bit) + DATA(nbit) + LRC
	switch(uart_rcv.sta)
	{
		case U_WAIT_HEAD:
		{
			if(rdat==FRAME_STX)
				uart_rcv.sta=U_WAIT_LEN;	//to len
			else
				uart_rcv.sta=U_WAIT_HEAD;	//self
			break;
		}

		case U_WAIT_LEN:
		{
			uart_rcv.exp_len=rdat;			//expect dat len
			uart_rcv.sta=U_WAIT_DATA;
			uart_rcv.cnt=0;					//reset
			break;
		}

		case U_WAIT_DATA:
		{
			uart_rcv.dat[uart_rcv.cnt]=rdat;
			if(uart_rcv.cnt<(RCV_BUF_LEN-1))
                uart_rcv.cnt++;
            else
            {
                uart_rcv.flag=UCOM_RCV_ERR;
                uart_rcv.sta=U_WAIT_HEAD;
                break;
            }

			if(uart_rcv.cnt==uart_rcv.exp_len)
				uart_rcv.sta=U_WAIT_LRC;
			else
				uart_rcv.sta=U_WAIT_DATA;
			break;
		}

		case U_WAIT_LRC:
		{
			uart_rcv.dat[uart_rcv.cnt]=rdat;	//lrc byte
			//------- calculate the lrc: lrc=head+len+dat1+dat2..+datn --------
			rdat=FRAME_STX+uart_rcv.exp_len;
			for(i=0;i<uart_rcv.exp_len;i++)
				rdat+=uart_rcv.dat[i];
			if(rdat==uart_rcv.dat[uart_rcv.cnt])
			{
				uart_rcv.out_time = 0;		// STOP计时
				uart_rcv.flag=UCOM_RCV_OK;
			}
			else
				uart_rcv.flag=UCOM_RCV_ERR;
			//-----------------------------------------------------------------
			uart_rcv.sta=U_WAIT_HEAD;
			break;
		}

        default:
        {
			uart_rcv.sta=U_WAIT_HEAD;
			break;
        }
	}
}

u8 sent_cmd(u8 *out_buf,u8 tlen, u8 *rcv_buf, u8 *rlen, u16 outtime)
{
    u8 tmp=0;

	outtime = outtime;
    sent_frame(out_buf,tlen);

    *rlen=0;
    while(1)
    {
        tmp=uart_rcv.flag;

        if(tmp==UCOM_RCV_OK)
        {
            if(out_buf[0]==uart_rcv.dat[0])
            {
                for(tmp=0;tmp<uart_rcv.cnt;tmp++)
                    rcv_buf[tmp]=uart_rcv.dat[tmp+1];
                *rlen=tmp;
            }
            else
            {
            	return UCOM_RCV_ERR;
            }

            break;
        }

		outtime = 30;              			// 0.3S 超时

        if(uart_rcv.out_time > outtime)   	// 0.3S 超时
        {
            return UCOM_RCV_TIMEOUT;
        }
    }
    uart_rcv.flag=UCOM_RCV_NULL;

    return UCOM_RCV_OK;
}


//----------------------------------------------------------------------------------
// use timer1 as Baudrate generator, Maxmum Baudrate can ???? bps @ 16MHZ
//----------------------------------------------------------------------------------
void init_uart0(u32 bdr , u8 tx_or_rx)    //T1M = 1, SMOD = 1
{
    const u32 sys_osc = 16000000;    //16mhz

    const u32 div=(sys_osc)/(16);   // 1000000为us节拍,16为模式1的双速率的固定分频因子,3为系统时钟为1/3mhz

    P07_Quasi_Mode;     //rx en
    P07=1;

    SCON=BIT6;          //MODE1 异步10位(1起始+8数据+1停止)

    TMOD |= 0x20;       //Timer1 Mode1 8位定时器模式,TH1自动装载
    set_SMOD;           //UART0 Double Rate Enable
    set_T1M;            //time1 为系统时钟
    clr_BRCK;           //Serial port 0 baud rate clock source = Timer1

	if(tx_or_rx==1)
	{
        ;
    }
	else if(tx_or_rx==2)
	{
        P06_Quasi_Mode;     //tx
        set_REN;set_RI;     //uart0 rx_en,rx_int_en
    }
	else if(tx_or_rx==3)
    {
        P06_Quasi_Mode;     //tx
        set_REN;set_RI;     //uart0 rx_en,rx_int_en

    }

    if(div/bdr>254)
    {
        clr_T1M;            // 系统时钟 1/12
        TH1 = 256 - (div/(12*bdr)+1);        //
    }
    else
    {
        TH1 = 256 - (div/bdr+1);            //
    }

    set_ES;             //enable UART interrupt
    set_TR1;
}

void serial_isr(void) interrupt 4
{
    if(RI)
    {                                       /* if reception occur */
        clr_RI;                             /* clear reception flag for next reception */
        uart_rcv_data(SBUF);
    }

    if(TI)
    {
		usent_ok=1;
        clr_TI;                             // if emission occur
    }
}


/**************************************************************************
*function:  void iwgd_init(void)
*input:     NULL
*output:    NULL
*describe:  看门狗初始化
**************************************************************************/
void iwgd_init(void)
{
	;
}

/**************************************************************************
*function:  void readout_data(void)
*input:     NULL
*output:    NULL
*describe:  读出保存数据处理函数
**************************************************************************/
u8 readout_data(void)
{
    u8 tmp1, tmp2;

	/*system1 para*/
	sys_para.backup_delay_time_unit=read_APROM_BYTE(SYS_TIME_UNIT);
	if(sys_para.backup_delay_time_unit>2)
		sys_para.backup_delay_time_unit=0;

	tmp1=read_APROM_BYTE(SYS_TIME_VALUE_H);
	tmp2=read_APROM_BYTE(SYS_TIME_VALUE_L);
	sys_para.backup_delay_time = (u16)(tmp1<<8)|tmp2;
	if(sys_para.backup_delay_time>6000)
		sys_para.backup_delay_time=1;

	sys_para.backup_shock_level=read_APROM_BYTE(SYS_SHOCK_VALUE);
	if(sys_para.backup_shock_level>30)
		sys_para.backup_shock_level=1;
	if(0==sys_para.backup_delay_time_unit)
	{
		if(sys_para.backup_delay_time<5)		//最少为0.5秒
			sys_para.backup_delay_time=10;
	}
	
	sys_para.delay_time_unit=sys_para.backup_delay_time_unit;
	sys_para.delay_time=sys_para.backup_delay_time;
	sys_para.shock_level=sys_para.backup_shock_level;

    return 1;
}

/**************************************************************************
*function:  void hook_save_data(void)
*input:     NULL
*output:    NULL
*describe:  延时保存数据
**************************************************************************/
void hook_save_100ms(u8 time)
{
	save_delay=time;
}

void save_data(void)
{
    volatile u8 xdata wr_dat[10];

    wr_dat[0]=sys_para.backup_delay_time_unit;
    wr_dat[1]=sys_para.backup_delay_time>>8;
    wr_dat[2]=(u8)sys_para.backup_delay_time;
    wr_dat[3]=sys_para.shock_level;
    write_DATAFLASH_BYTE(SYS_TIME_UNIT, wr_dat, 4);
}


/**************************************************************************
*function:  void save_proc(void)
*input:     NULL
*output:    NULL
*describe:  保存数据处理函数
**************************************************************************/
void save_proc(void)
{
    static u32 f100ms_save=0;
	if(save_delay>0)
	{
        if(timeout_chk(&f100ms_save, 50)==SUCCESS)            //定义节拍50ms
        {
            timeout_chk(&f100ms_save, 0);
    		save_delay--;
    		if(save_delay==0)
    		{
                save_data();
    		}
        }
	}
}

void reset_mcu(void)
{
    iwgd_init();
    while(1);                       //wait watch dog reset
}

void clear_uart_cmd(void)
{
	if(uart_rcv.out_time)
	{
		delay(1000);
	}
	uart_rcv.sta=U_WAIT_HEAD;
	uart_rcv.flag=UCOM_RCV_NULL;
	g_m_connect_sta = 0;
}

void connect_service(void)
{
	static u32 service_tick=0;
    u8 u_buff[10];
	u8 tmp;

	if(SUCCESS==timeout_chk(&service_tick, 100))
	{
		switch(g_m_connect_sta)
		{
			case 0:
				u_buff[0]=CMD_GET_INFO;
                sent_frame(u_buff,1);	//query
				g_m_connect_sta=1;
				break;

			case 1:
				if(uart_rcv.flag==UCOM_RCV_OK)
				{
					uart_rcv.flag=UCOM_RCV_NULL;

					if(CMD_GET_INFO==uart_rcv.dat[0])						//ok
					{
                        sys_para.rd_delay_time_unit=uart_rcv.dat[1]>>6;		//unit
                        tmp=uart_rcv.dat[1]&0x3f;
                        sys_para.rd_delay_time=(u16)(tmp<<8)|uart_rcv.dat[2];   //delay value
                        sys_para.rd_shock_level=uart_rcv.dat[3];    		//level

                        if(1==sys_para.sys_mode)        					//read mode
                        {
                            sys_para.delay_time_unit=sys_para.rd_delay_time_unit;
                            sys_para.delay_time=sys_para.rd_delay_time;
                            sys_para.shock_level=sys_para.rd_shock_level;

							sys_para.is_config=2;			//读出成功

							lcd_show.config_name=1;
                            lcd_show.delay_unit=1;
                            lcd_show.delay_val=1;
                            lcd_show.shock_val=1;
                        }

                        sys_para.is_connect=1;
                        lcd_show.connect_name=1;
                        g_m_connect_sta=0;
					}
				}
                if(uart_rcv.out_time>20)
                {
                	uart_rcv.sta=U_WAIT_HEAD;
					//fail
					if(sys_para.is_connect)
					{
                        sys_para.is_connect=0;
                        sys_para.rd_delay_time_unit=0;
                        sys_para.rd_delay_time=0;
                        sys_para.rd_shock_level=0;
                        lcd_show.connect_name=1;
					}

					if(sys_para.is_config)
					{
                        sys_para.is_config=0;
                        lcd_show.config_name=1;
					}

					g_m_connect_sta=0;
                }
                break;
		}
	}
}

