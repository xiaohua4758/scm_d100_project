/***************************************  File Info **********************************************
** File name:			button.c
** Descriptions:        按键处理函数
** Created by:
** Created date:
*************************************************************************************************/
#include <stdio.h>
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "myfunction.h"
#include "timer.h"
#include "lcd.h"
#include "button_app.h"

struct_key_status key;

unsigned char pre_key=0;	//按键前一个状态
unsigned char cur_key=0;	//按键当前状态

#define SET_KEY		(P1&BIT2)
#define RETURN_KEY	(P1&BIT3)
#define UP_KEY		(P1&BIT4)
#define DOWN_KEY	(P1&BIT6)
#define OK_KEY		(P0&BIT2)

//初始化按键状态
void key_init_sta(void)
{
    u8 i;
    for(i=0; i<5; i++)
        key_detect();
}

/**************************************************************************
*function:  void key_detect(void)
*input:     NULL
*output:    NULL
*describe:  按键检测
**************************************************************************/
void key_detect(void)
{
	if(CHECK_KEY_LOW(SET_KEY)) cur_key|=BIT0;		//BIT0----SET_KEY
	else cur_key &= NEG_BIT0;

	if(CHECK_KEY_LOW(RETURN_KEY)) cur_key|=BIT1;	//BIT1----RETURN_KEY
	else cur_key &= NEG_BIT1;

	if(CHECK_KEY_LOW(UP_KEY)) cur_key|=BIT2;		//BIT2----UP_KEY
	else cur_key &= NEG_BIT2;

	if(CHECK_KEY_LOW(DOWN_KEY)) cur_key|=BIT3;		//BIT3----DOWN_KEY
	else cur_key &= NEG_BIT3;

	if(CHECK_KEY_LOW(OK_KEY)) cur_key|=BIT4;		//BIT4----OK_KEY
	else cur_key &= NEG_BIT4;


	if(cur_key&BIT0)
		key.mode=(pre_key&BIT0)?KEY_LONG_ON:KEY_ONCE_ON;
	else
        key.mode=(pre_key&BIT0)?KEY_ONCE_UP:KEY_OFF;

    if(cur_key&BIT1)
        key.select=(pre_key&BIT1)?KEY_LONG_ON:KEY_ONCE_ON;
    else
        key.select=(pre_key&BIT1)?KEY_ONCE_UP:KEY_OFF;

	if((cur_key&BIT2))
		key.up=(pre_key&BIT2)?KEY_LONG_ON:KEY_ONCE_ON;
	else
		key.up=(pre_key&BIT2)?KEY_ONCE_UP:KEY_OFF;

	if((cur_key&BIT3))
		key.down=(pre_key&BIT3)?KEY_LONG_ON:KEY_ONCE_ON;
	else
		key.down=(pre_key&BIT3)?KEY_ONCE_UP:KEY_OFF;

	if((cur_key&BIT4))
		key.confirm=(pre_key&BIT4)?KEY_LONG_ON:KEY_ONCE_ON;
	else
		key.confirm=(pre_key&BIT4)?KEY_ONCE_UP:KEY_OFF;

	pre_key = cur_key;	//保存当前按键
}


void do_with_key(void)
{
	u8 s_buff[6],r_buff[6];
	u8 cnt_up=0, cnt_down=0;
	u16 step_tmp;
	static u16 hold_time=0;

	if(key.mode==KEY_ONCE_ON)
	{
		if(sys_para.sys_mode==0)
		{
			sys_para.sys_mode=1;		//read back
            sys_para.set_mode=0;
			sys_para.delay_time=0;
			sys_para.shock_level=0;
			sys_para.delay_time_unit=0;
		}
		else
		{
			sys_para.sys_mode=0;		//setting
            sys_para.set_mode=1;
			sys_para.delay_time=sys_para.backup_delay_time;
			sys_para.shock_level=sys_para.backup_shock_level;
			sys_para.delay_time_unit=sys_para.backup_delay_time_unit;
		}

        sys_para.is_config=0;

		lcd_show.config_name=1;
		lcd_show.sys_mode_name=1;
		lcd_show.delay_unit=1;
	    lcd_show.delay_val=1;
	    lcd_show.shock_val=1;
	}

	if(sys_para.sys_mode==1)
		return;

    //key.confirm=KEY_OFF;

	if(key.confirm==KEY_ONCE_ON)
	{
        clear_uart_cmd();

        if(sys_para.is_connect==0)
        {
	        warnning_unconnect();
        }
		else
		{
            s_buff[0]=CMD_SET_INFO;
            if(sys_para.delay_time>8000)
                sys_para.delay_time=8000;
            s_buff[1]=sys_para.delay_time>>8;               //delay time
            s_buff[2]=(u8)sys_para.delay_time;

            if(sys_para.delay_time_unit>7)
                sys_para.delay_time_unit=7;
            s_buff[1]|=(sys_para.delay_time_unit<<6);       //unit

            s_buff[3]=sys_para.shock_level;                 //shock level

            sys_para.is_config=0;
            lcd_show.config_name=1;
            if(UCOM_RCV_OK==sent_cmd(s_buff, 4, r_buff, sizeof(r_buff), 300))
            {
                sys_para.is_config=1;
                //sys_para.set_mode=0;
            }
		}
	}

	if(key.select==KEY_ONCE_ON)
	{
		sys_para.set_mode=(sys_para.set_mode<3)?(sys_para.set_mode+1):1;
	}

    if(key.up==KEY_ONCE_ON)
    {
		cnt_up=1;			//单次增
	}
	else if(key.up==KEY_LONG_ON)
	{
		if(hold_time<2000)
			hold_time++;

		if(hold_time>200)
			cnt_up=1;		//自增
	}

    //key.down = KEY_OFF;

    if(key.down==KEY_ONCE_ON)
    {
		cnt_down=1;
	}
	else if(key.down==KEY_LONG_ON)
	{
		if(hold_time<2000)
			hold_time++;

		if(hold_time>200)
			cnt_down=1;		//自减
	}

	if((key.up!=KEY_LONG_ON)&&(key.down!=KEY_LONG_ON))
	{
		hold_time=0;
	}

	switch(sys_para.set_mode)
	{
        case 1:		//level
            if(cnt_up)
            {
				//step_tmp = ((hold_time/300)+1);                   		//step
                step_tmp = 1;

                if(sys_para.shock_level+step_tmp>30)
                    sys_para.shock_level=30;
                else
                    sys_para.shock_level=sys_para.shock_level+step_tmp;   //

                lcd_show.global_refresh=1;
            }
            if(cnt_down)
            {
                //step_tmp = ((hold_time/300)+1);                    		//step
                step_tmp = 1;

                if(sys_para.shock_level<step_tmp)
                    sys_para.shock_level=1;
                else
                    sys_para.shock_level=sys_para.shock_level-step_tmp;   //

				if(sys_para.shock_level<1)
					sys_para.shock_level=1;

                lcd_show.global_refresh=1;
            }
            break;

		case 2:		// unit
	        if(cnt_up)
	        {
	        	sys_para.delay_time_unit=(sys_para.delay_time_unit<2)?(sys_para.delay_time_unit+1):0;
	        	lcd_show.delay_unit=1;
	        }
	        if(cnt_down)
	        {
	        	sys_para.delay_time_unit=(sys_para.delay_time_unit>0)?(sys_para.delay_time_unit-1):2;
	        	lcd_show.delay_unit=1;
			}

            if(sys_para.delay_time_unit==0)
            {
				s_buff[0]=sys_para.delay_time%10;	// 个位清零
				if((s_buff[0]!=0)&&(s_buff[0]!=5))
				{
                    s_buff[0]=10-s_buff[0];
                    sys_para.delay_time+=s_buff[0];
				}

	            if(sys_para.delay_time>6000)
	                sys_para.delay_time=6000;
            }
            else
            {
				if(sys_para.delay_time>600)
					sys_para.delay_time=600;
            }
	        break;

		case 3:		//hold time value
	        if(cnt_up)
	        {
	        	u16 max_num;

				if(sys_para.delay_time_unit==0)
				{
					max_num = 6000;
					step_tmp = 5*((hold_time/200)+1);					//step, second
				}
				else if((sys_para.delay_time_unit==1)||(sys_para.delay_time_unit==2))
				{
					max_num = 600;
					step_tmp = 1*((hold_time/200)+1);					//step, minitues hour
				}

				if(sys_para.delay_time+step_tmp>max_num)
					sys_para.delay_time=max_num;
				else
                	sys_para.delay_time=sys_para.delay_time+step_tmp;	//

                lcd_show.delay_val=1;
			}
	        if(cnt_down)
	        {
                if(sys_para.delay_time_unit==0)
                    step_tmp = 5*((hold_time/200)+1);                    //step, second
                else if((sys_para.delay_time_unit==1)||(sys_para.delay_time_unit==2))
                    step_tmp = 1*((hold_time/200)+1);                    //step, minitues hour

				if(sys_para.delay_time<step_tmp)
					sys_para.delay_time=0;
				else
                	sys_para.delay_time=sys_para.delay_time-step_tmp;	//

				if(sys_para.delay_time<1)
				{
                    if(sys_para.delay_time_unit==0)
						sys_para.delay_time=5;
					else
						sys_para.delay_time=1;
				}

                lcd_show.delay_val=1;
	        }
			break;
	}

	if(sys_para.set_mode>0)
	{
		if(cnt_up||cnt_down)
		{
            hook_save_100ms(10);
            if(sys_para.is_config)
            {
                sys_para.is_config=0;
                lcd_show.config_name=1;
            }
		}
	}

	sys_para.backup_delay_time=sys_para.delay_time;
	sys_para.backup_shock_level=sys_para.shock_level;
	sys_para.backup_delay_time_unit=sys_para.delay_time_unit;
}

/* 处理OK键*/
/* 处理 */
void key_proc(void)
{
	key_detect();
	do_with_key();
}

