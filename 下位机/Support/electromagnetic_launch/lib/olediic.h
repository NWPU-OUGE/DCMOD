/************************************************************************************ 
*            ll rights reserved.
* 
* ��������
* 
*
* File name: olediic.c
* Project  : OLED_IIC.uvprojx
* Processor: STM32H750VBT6
* Compiler : MDK5 STM32CUBEMX
* 
* Author : ��Ӱ��
* Version: 2.00
* Date   : 2020.2.28
* Email  : û��
* Modification: none
* 
* Description:128*64�����OLED��ʾ�������ļ�����������SSD1306����IICͨ�ŷ�ʽ��ʾ��
*
*
* History: none;
*
*************************************************************************************/

#ifndef __OLEDIIC_H__
#define __OLEDIIC_H__
#include "main.h"

void WriteCmd(void);
void OLED_WR_CMD(uint8_t cmd);
void OLED_WR_DATA(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_On(void);
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);







#endif
