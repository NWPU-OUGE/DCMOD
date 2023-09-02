/*
 * @Author: your name
 * @Date: 2021-05-10 18:37:58
 * @LastEditTime: 2021-05-19 07:48:24
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\read_angle.h
 */
/**
 * @file diancitanshe.h
 * @brief 
 * @author 郑一帆
 * @version 1.0
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021  足球机器人轮式组
 * 
 * @par 文件说明:电磁弹射
 * *******************************************************
 */
#ifndef __ELECTRONIC_H__
#define __ELECTRONIC_H__
#include "main.h"
#include "struct_typedef.h"

#define RC_SHOOT_CHANNEL 1
#define RC_CAP_CHANNEL 3

#define ELECTRONIC_CAP_CHANNEL 4
#define ELECTRONIC_SHOOT_CHANNEL 1
#define MAX_ELECTRONIC_VALUE 400
#define MIN_ELECTRONIC_VALUE 0

//数据包的内容
#define HEAD 0x74
#define END  0xb8
#define SHOOT 0x01
#define UNSHOOT 0x00

//数据包的通道
#define HEAD_CHANNEL 0
#define STATE_CHANNEL 1
#define CAP_CHANNEL 2
#define BCC_CHANNEL 4
#define END_CHANNEL  5
#define TRANSMIT_DATA_LENGTH 6

extern UART_HandleTypeDef huart6;
/**
 * @brief 电磁
 */
typedef struct
{
	uint16_t expect_voltage;//期望电压（由上位机控制）
	uint16_t now_voltage;//读取到的电压
	uint8_t buf[7];//电磁弹射板接收的数据
	uint8_t state;//状态，由上位机控制 0为停  1为冲  2为放   3为弹
}DIANCI;

typedef struct
{
	uint16_t expect_voltage;//期望电压（由上位机控制）
	uint16_t now_voltage;//读取到的电压
	uint8_t transmit_data[TRANSMIT_DATA_LENGTH];
	uint8_t received_data[7];
	uint8_t state;
	bool_t Rc_Flag;
	int16_t rc_cap_value;//遥控器的值(控制电容电量)
	int8_t rc_shoot_value;//遥控器的值(控制电磁弹射状态)
	int16_t cap_expert_value;//电磁弹射的期望电压值
	int16_t cap_change_value;//电磁弹射的临时改变的电压值
}electronic_t;



void electronic_task(void const * argument);  //电磁任务
#endif
