/***************************************  File Info **********************************************
** File name:			main.c
** Descriptions:
** Created by:			ENEN TECH.TAOBAO
** Created date:		2020.01.03
*************************************************************************************************/
#include <stdio.h>
#include "string.h"
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "myfunction.h"
#include "timer.h"
#include "lcd.h"
#include "button_app.h"

system_parameter sys_para={0};
lcd_show_flags lcd_show;		//系统显示标志

void u32_to_num(u32 num_in, u8 *num_out_buff, u8 len)
{
	u8 i;

	for(i=0; i<len; i++)
	{
        num_out_buff[len-1-i] = num_in%10;
        num_in = num_in/10;
	}
}

//dot_flag是否显示小数点, high_light_bit高亮从左数起的第n位 0-16,255时无
void show_num(u8 x, u8 y, u32 num_in, u8 dec_num_len, u8 dot_flag, u16 color, u8 high_light_bit, u16 hl_color)
{
	u8 num[16];
	u8 xtmp, i;
	u16 bl_col;

    if(dec_num_len>(sizeof(num)-2))
    	dec_num_len = sizeof(num)-2;

    u32_to_num(num_in, num, dec_num_len);

	if(dot_flag)
	{
		dec_num_len=dec_num_len+1;

        num[dec_num_len]=num[dec_num_len-1];
        num[dec_num_len-1]=num[dec_num_len-2];
        num[dec_num_len-2]='.';
	}

    bl_col = BACK_COLOR;
	for(i=0; i<dec_num_len; i++)
	{
		xtmp = x+8*i;
        if(xtmp<(160-8))
        {
        	if(high_light_bit==i)
                BACK_COLOR = hl_color;
			if(num[i]!='.')
				num[i]=num[i]+'0';

            LCD_ShowChar(xtmp, y, num[i], 0, color);
            BACK_COLOR = bl_col;
		}
	}
}


void disp_flash_pro(void)
{
	static u32 blank_tick=0;
	static u8 pre_mode=0xff, turn=0;
	u8 show_now=0;

    if(sys_para.set_mode != pre_mode)
    {
        lcd_show.delay_unit=1;
        lcd_show.delay_val=1;
        lcd_show.shock_val=1;
    }

	if(sys_para.set_mode)
	{
		if(lcd_show.global_refresh)
		{
			lcd_show.global_refresh=0;
            timeout_chk(&blank_tick, 0);
			show_now=1;
		}

        if((show_now)||(SUCCESS==timeout_chk(&blank_tick, 200)))
        {
			switch(sys_para.set_mode)
			{
				case 1:
					lcd_show.shock_val=(turn)?0x01:0xff;
					break;

				case 2:
					lcd_show.delay_unit=(turn)?0x01:0xff;
					break;

                case 3:
                    lcd_show.delay_val=(turn)?0x01:0xff;
                    break;
			}
        	turn = !turn;
        }
	}

	pre_mode = sys_para.set_mode;
}


//一行 160/16 = 10字符, 4+5=9
void show_shock_delay(u32 dly_time, u8 logo_on, u16 color, u8 high_light_bit, u16 hl_color)
{
	static u8 pre_unit=0xff;
    u8 x = 10;
    //u8 y = 16+4;
    u8 y = 20+16+4;
    u16 color_tmp = BACK_COLOR;

	if(logo_on)
	{
		LCD_ShowChinese(x,y,32,16,CYAN);   	//保32
		x+=16;
		LCD_ShowChinese(x,y,33,16,CYAN);   	//持33
		x+=16;
        LCD_ShowChar(x, y, '(', 0, CYAN);
		x+=(16+8+16);
        LCD_ShowChar(x, y, ')', 0, CYAN);
    }

    if(lcd_show.delay_unit)
    {
	    x=10+16+16+8;

		if(lcd_show.delay_unit==0xff)		//选择
    		BACK_COLOR = RED;

        switch(sys_para.delay_time_unit)
        {
            case 0:
                LCD_ShowChinese(x,y,2,16,color);			//秒2
                LCD_ShowChinese(x+16,y,24,16,color);		//钟24
                break;

            case 1:
                LCD_ShowChinese(x,y,21,16,color);			//分21
                LCD_ShowChinese(x+16,y,24,16,color);		//钟24
                break;

            case 2:
                LCD_ShowChinese(x,y,23,16,color);			//小23
                LCD_ShowChinese(x+16,y,22,16,color);		//时22
                break;
        }
        BACK_COLOR = color_tmp;
    }

    if(pre_unit != sys_para.delay_time_unit)
    {
		lcd_show.delay_val=1;
        LCD_ShowChar(10+16+16+16+8+16+8+4+24, y, ' ', 0, color);
        LCD_ShowChar(10+16+16+16+8+16+8+4+32, y, ' ', 0, color);
	}

    if(lcd_show.delay_val)
	{
		u8 dot_falg, no_cnt;
        x = 10+16+16+8+16+8+16+4;

		if(lcd_show.delay_val==0xff)		//选择
    		BACK_COLOR = RED;

        if(sys_para.delay_time_unit==0)
        {
        	no_cnt=4;
        	dot_falg=1;
        }
        else
        {
        	no_cnt=3;
        	dot_falg=0;
		}
        show_num(x, y, dly_time, no_cnt, dot_falg, color, high_light_bit, hl_color);
		BACK_COLOR = color_tmp;
	}
    pre_unit = sys_para.delay_time_unit;

}

void show_shock_level(u8 shock_level, u8 logo_on, u16 color, u8 high_light_bit, u16 hl_color)
{
    u8 x = 10;
    u8 y = 16+4;
    //u8 y = 20+16+4;
    u16 color_tmp = BACK_COLOR;

	if(logo_on)
	{
		LCD_ShowChinese(x,y,27,16,CYAN); x+=16;	//灵27
		LCD_ShowChinese(x,y,28,16,CYAN); x+=16;	//敏28
		LCD_ShowChinese(x,y,34,16,CYAN); x+=16;	//度34
        x = 10+16+16+20+16;
        LCD_ShowChar(x, y, '(', 0, CYAN);x+=8;
        LCD_ShowChar(x, y, '0', 0, CYAN);x+=8;
        LCD_ShowChar(x, y, '1', 0, CYAN);x+=8;
        LCD_ShowChinese(x,y,30,16,CYAN); x+=16;	//最30
        LCD_ShowChinese(x,y,31,16,CYAN); x+=16;	//高31
        LCD_ShowChar(x, y, ')', 0, CYAN);x+=8;
    }

    x = 10+16+16+20;

    if(lcd_show.shock_val)
    {
		if(lcd_show.shock_val==0xff)	//选择
    		BACK_COLOR = RED;

	    show_num(x, y, shock_level, 2, 0, color, high_light_bit, hl_color);
        BACK_COLOR = color_tmp;
    }
}

void lcd_disp(void)
{
	u8 x,y;
	u8 tmp[4];
	u16 color;

    show_shock_delay(sys_para.delay_time, lcd_show.delay_name, YELLOW, 0xff, RED);
    show_shock_level(sys_para.shock_level, lcd_show.delay_name, YELLOW, 0xff, RED);

	if(lcd_show.sys_mode_name)
	{
        x=(160-2*16)/2-4;
        y=0;            //line 1

        if(sys_para.sys_mode==0)
        {
        	tmp[0]=16;	//设16
        	tmp[1]=11;	//置11
        }
        else
        {
        	tmp[0]=25;	//读25
        	tmp[1]=26;	//出26
        }
        LCD_ShowChinese(x,y,tmp[0],16,CYAN); x+=16;
        LCD_ShowChinese(x,y,tmp[1],16,CYAN); x+=16;
	}

	if(lcd_show.connect_name)
	{
		x=2;
		y=4*16;         //line 5
        if(0==sys_para.is_connect)
        {
			tmp[0]=12;	//未12
			color = BLUE;
		}
		else
		{
			tmp[0]=13;	//已13
			color = GREEN;
		}
		LCD_ShowChinese(x,y,tmp[0],16,color); x+=16;
		LCD_ShowChinese(x,y,14,16,color); x+=16; //连14
		LCD_ShowChinese(x,y,15,16,color); x+=16; //接15
	}

	if(lcd_show.config_name)
	{
        x=(160-4*16);
        y=4*16;         //line 5

        if(0==sys_para.is_config)
        {
        	LCD_ShowChar(x, y, ' ', 0, RED);x+=8;
	        LCD_ShowChinese(x,y,12,16,RED); x+=16;      	//未12
            LCD_ShowChinese(x,y,16,16,RED); x+=16;      	//设16
            LCD_ShowChinese(x,y,11,16,RED); x+=16;      	//置11
            LCD_ShowChar(x, y, ' ', 0, RED);
        }
        else
        {
            if(1==sys_para.is_config)
			{
                tmp[0]=16;  //设16
                tmp[1]=11;  //置11
			}
			else if(2==sys_para.is_config)
			{
                tmp[0]=25;  //读25
                tmp[1]=26;  //出26
			}

            LCD_ShowChinese(x,y,tmp[0],16,MAGENTA); x+=16;
            LCD_ShowChinese(x,y,tmp[1],16,MAGENTA); x+=16;
            LCD_ShowChinese(x,y,17,16,MAGENTA); x+=16;      //成17
            LCD_ShowChinese(x,y,18,16,MAGENTA); x+=16;      //功18
        }
	}

	memset((u8 *)&lcd_show, 0, sizeof(lcd_show));
}

void warnning_unconnect(void)
{
	u8 x,y, i;

	for(i=0; i<3; i++)
	{
		x=2;
		y=4*16;         //line 5

        LCD_ShowString(x, y, "        ", RED);
        delay(1000);
		LCD_ShowChinese(x,y,12,16,RED); x+=16;	//未12
		LCD_ShowChinese(x,y,14,16,RED); x+=16;	//连14
		LCD_ShowChinese(x,y,15,16,RED); x+=16;	//接15
        delay(1000);
	}
}

void io_init(void)
{
	P12_Quasi_Mode;P12=1;
	P13_Quasi_Mode;P13=1;
	P14_Quasi_Mode;P14=1;
	P16_Quasi_Mode;P16=1;
	P02_Quasi_Mode;P02=1;

	P11_PushPull_Mode;P11=1;		//scm rst
	P05_PushPull_Mode;P05=0;		//scm link

	P30_PushPull_Mode;P30=0;        //lcd sda
	P17_PushPull_Mode;P17=0;        //lcd scl

}

// TFT LCD 80 x 160 Dots
int main(void)
{
	io_init();
	delay(100);
	t2_start();
	Lcd_Init();						//初始化LCD
    init_uart0(9600,3);
	readout_data();

	memset((u8 *)&lcd_show, 1, sizeof(lcd_show));
	sys_para.set_mode=1;
    enableInterrupts();
    while(1)
    {
        connect_service();
        key_proc();
        disp_flash_pro();
		lcd_disp();
        save_proc();
	}
}
