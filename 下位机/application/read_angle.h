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
 * @author 郑一帆
 * @version 1.0
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021  足球机器人轮式组
 * 
 * @par 文件说明:读取角度传感器的ADC值，并以串口的形式发送到USART1
 * *******************************************************
 */
#ifndef __ANGLE_H__
#define __ANGLE_H__
#include "main.h"

/**
 * @brief 何影萌超级无敌读取ADC的结构体
 */
typedef struct
{
  uint16_t adc1;  //ADC1值
  uint16_t adc2;  //ADC2值
  uint16_t adc3;  //ADC3值
  uint16_t adc4;  //ADC4值
  uint8_t Buf[30];//缓存
  uint8_t num;//当前记录的数据位
  uint8_t flag;//接收完成数据位
}angle_read_t;

void angle_usart_task(void const * argument);
#endif
