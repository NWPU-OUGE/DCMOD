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
 * @author ֣һ��
 * @version 1.0
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021  �����������ʽ��
 * 
 * @par �ļ�˵��:��ŵ���
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

//���ݰ�������
#define HEAD 0x74
#define END  0xb8
#define SHOOT 0x01
#define UNSHOOT 0x00

//���ݰ���ͨ��
#define HEAD_CHANNEL 0
#define STATE_CHANNEL 1
#define CAP_CHANNEL 2
#define BCC_CHANNEL 4
#define END_CHANNEL  5
#define TRANSMIT_DATA_LENGTH 6

extern UART_HandleTypeDef huart6;
/**
 * @brief ���
 */
typedef struct
{
	uint16_t expect_voltage;//������ѹ������λ�����ƣ�
	uint16_t now_voltage;//��ȡ���ĵ�ѹ
	uint8_t buf[7];//��ŵ������յ�����
	uint8_t state;//״̬������λ������ 0Ϊͣ  1Ϊ��  2Ϊ��   3Ϊ��
}DIANCI;

typedef struct
{
	uint16_t expect_voltage;//������ѹ������λ�����ƣ�
	uint16_t now_voltage;//��ȡ���ĵ�ѹ
	uint8_t transmit_data[TRANSMIT_DATA_LENGTH];
	uint8_t received_data[7];
	uint8_t state;
	bool_t Rc_Flag;
	int16_t rc_cap_value;//ң������ֵ(���Ƶ��ݵ���)
	int8_t rc_shoot_value;//ң������ֵ(���Ƶ�ŵ���״̬)
	int16_t cap_expert_value;//��ŵ����������ѹֵ
	int16_t cap_change_value;//��ŵ������ʱ�ı�ĵ�ѹֵ
}electronic_t;



void electronic_task(void const * argument);  //�������
#endif
