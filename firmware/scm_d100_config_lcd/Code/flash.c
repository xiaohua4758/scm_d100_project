#include "N76E003.h"          //包含用到的外设的头文件
#include "string.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "myfunction.h"

bit BIT_TMP;

/**************************************************************************
*function:  unsigned char read_APROM_BYTE(unsigned long addr)
*input:     eeprom地址
*output:    数据
*describe:  读取存放在EERPOM中地址值为addr的数据
**************************************************************************/
u8 read_APROM_BYTE(u16 code *u16_addr)
{
    u8 rdata;
    rdata = *u16_addr>>8;
    return rdata;
}


//!!!!!!   注意此文件不能开最大优化,否则99%机会都读写FLASH失败

/*****************************************************************************************************************
write_DATAFLASH_BYTE :
user can copy all this subroutine into project, then call this function in main.
******************************************************************************************************************/
volatile void write_DATAFLASH_BYTE(u16 u16_addr,u8 *u8_data, u8 len)
{
	volatile u8 u8_addrl_r;
	volatile u8 xdata i;

    //擦除需要整页(512 byte)探险 编程按字节编程
    disableInterrupts();
    //u16_addr=SYS1_LOCK_EPROM_ADDR;  //地址对齐
    //Erase APROM DATAFLASH page
    IAPAL = u16_addr;
    IAPAH = u16_addr>>8;
    IAPFD = 0xFF;
    set_IAPEN;
    set_APUEN;
    IAPCN = 0x22;
    set_IAPGO;
    //Save changed RAM data to APROM DATAFLASH
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		u8_addrl_r =0;
	}
	else
	{
		u8_addrl_r = 0x80;
	}
    IAPAL = u8_addrl_r;
    IAPAH = u16_addr>>8;
    set_IAPEN;
    set_APUEN;
    IAPCN = 0x21;
    for(i=0;i<len;i++)
    {
        IAPFD = u8_data[i];
        set_IAPGO;
        IAPAL++;
    }
    clr_APUEN;
    clr_IAPEN;

    enableInterrupts();
}

