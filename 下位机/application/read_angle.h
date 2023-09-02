/*
 * @Author: your name
 * @Date: 2021-05-10 18:37:58
 * @LastEditTime: 2021-05-10 19:41:41
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\read_angle.h
 */
/**
 * @file read_triangle.h
 * @brief 
 * @author ֣һ��
 * @version 1.0
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021  �����������ʽ��
 * 
 * @par �ļ�˵��:��ȡ�Ƕȴ�������ADCֵ�����Դ��ڵ���ʽ���͵�USART1
 * *******************************************************
 */
#ifndef __ANGLE_H__
#define __ANGLE_H__
#include "main.h"

/**
 * @brief ��Ӱ�ȳ����޵ж�ȡADC�Ľṹ��
 */
typedef struct
{
  uint16_t adc1;  //ADC1ֵ
  uint16_t adc2;  //ADC2ֵ
  uint16_t adc3;  //ADC3ֵ
  uint16_t adc4;  //ADC4ֵ
  uint8_t Buf[30];//����
  uint8_t num;//��ǰ��¼������λ
  uint8_t flag;//�����������λ
}angle_read_t;

void angle_usart_task(void const * argument);
#endif
