//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2018-03-29
//  最近修改   : 
//  功能描述   : OLED 4接口演示例程(STM32F0系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  接3.3v电源
//              SCL  接PA0（SCL）
//              SDA  接PA1（SDA）
//              RES  接PA2
//              DC   接PA3
//              CS   接PA4
//              BLK  接PA5
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/
#include "stm32f0xx.h"
#include "delay.h"
#include "lcd.h"


/*************************************************************************
演示功能1.LED灯D3闪烁
**************************************************************************/

int main(void)
{
	 float t=0;
	 delay_init();	    	 //延时函数初始化	  
	 Lcd_Init();			//初始化OLED  
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;
	 LED_ON;
	 while(1)
	 {
		 	LCD_ShowChinese(10,0,0,32,RED);   //中
			LCD_ShowChinese(45,0,1,32,RED);   //景
			LCD_ShowChinese(80,0,2,32,RED);   //园
//			LCD_ShowChinese(115,0,3,32,RED);  //电
//			LCD_ShowChinese(150,0,4,32,RED);  //子

		 	LCD_ShowChinese(10,75,0,16,RED);   //中
			LCD_ShowChinese(45,75,1,16,RED);   //景
			LCD_ShowChinese(80,75,2,16,RED);   //园
			LCD_ShowChinese(115,75,3,16,RED);  //电
			LCD_ShowChinese(150,75,4,16,RED);  //子
			LCD_ShowString(10,35,"1.14 TFT SPI 135*240",RED);
			LCD_ShowPicture(0,92,39,131);
			while(1)
			{
				LCD_ShowNum1(80,95,t,4,RED);
		    t+=0.01;
      }
	 }
}
