/***************************************  File Info **********************************************
** File name:			timer.h
** Descriptions:        ��ʱ����غ���
** Created by:			CSH
** Created date:		2012.04.03
*************************************************************************************************/


void t2_start(void);
void t2_stop(void);

void reset_blink_tick(void);		//����˸������ʾ
uint16_t timeout_chk(uint32_t *ptrTimer, uint32_t dwTimeOutVal);

