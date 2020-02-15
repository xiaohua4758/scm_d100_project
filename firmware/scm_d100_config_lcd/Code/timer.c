/***************************************  File Info **********************************************
** File name:			timer.c
** Descriptions:        定时器相关函数
** Created by:
** Created date:
*************************************************************************************************/
#include "N76E003.h"          //包含用到的外设的头文件
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "myfunction.h"
#include "timer.h"

//t2 init 0.5khz intetrrupt
void t2_start(void)
{
    u16 const pried=65535-32000;        // 16mhz/32000=0.5khz=2ms
    TIMER2_DIV_1;                       // sys=2mhz, 2mhz/1=2mhz
    TIMER2_Auto_Reload_Capture_Mode;    //
    //TIMER2_Auto_Reload_Delay_Mode;
    RCMP2L = pried;
    RCMP2H = pried>>8;
    set_ET2;                            // Enable Timer2 interrupt
    set_TR2;                            // Timer2 run
}

//停止定时器4
void t2_stop(void)
{
    clr_TR2;        // Timer2 stop
}

void t2_go_on(void)
{
    set_TR2;        // Timer2 run
}

volatile uint32_t g_dwSysTimeTick=0;
uint32_t dwGetTimeTick(void)
{
	uint32_t l_dwTimeTick = 0;

	l_dwTimeTick = g_dwSysTimeTick; // 获取当前值

	return (l_dwTimeTick); // 返回当前值
}

uint16_t timeout_chk(uint32_t *ptrTimer, uint32_t dwTimeOutVal)
{
	uint32_t l_dwDiff = 0;
	uint32_t l_dwTimeTick = 0;

	if (0 == dwTimeOutVal)
	{
		*ptrTimer = dwGetTimeTick();
		return SUCCESS;
	}
	else
	{
		l_dwTimeTick = dwGetTimeTick();
		l_dwDiff = l_dwTimeTick - *ptrTimer;
		if (l_dwDiff >= dwTimeOutVal)
		{
			*ptrTimer = l_dwTimeTick;
			return SUCCESS;
		}
		else
		{
			return ((uint16_t)FAILED);
		}
	}
}

void Timer2_ISR (void) interrupt 5
{
  	// 5.04khz intetrrupt,2ms
    clr_TF2;                                //Clear Timer2 Interrupt Flag

    g_dwSysTimeTick++;

    if(uart_rcv.out_time<0xffff)
    {
        if(uart_rcv.out_time)
        	uart_rcv.out_time++;
    }
}

