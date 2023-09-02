#pragma once

/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       protocol_send_task.c/h
  * @brief      communication task,
  *             协议通讯任务
  * @note
  * @history
  *  Version    Date            Author				        	Modification
  *  V1.0.0     Apr-11-2021     Yang-Zhengzhong         1. unfinished
  *  V1.0.0     Apr-22-2021     yzz                     beta finished
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  */
#include "string.h"
#include "cmsis_os.h"
#include "usb_task.h"
#include "fifo.h"
#include "protocol.h"
#include "protocol_receive_task.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

extern void protocol_send_task(void const * argument);

/*
数据包class number注册表
*/

#define PACKET_NUM 35					//注册表内消息类型数量 the number of message type,vary from device to device~
#define PROTOCOL_FIFO_BUF_LENGTH 1024	//fifo缓存区长度

#ifndef GLOBAL_H
#define GLOBAL_H


#define IIC_ADDR        0xae


#define NONE_MODE           0x00
#define SWITCH_TO_CONFIG    0x01	//配置模式
#define SET_DATA_PACKET     0x02	//规定返回那些数据
#define SET_ODR             0x03	//设置返回速度
#define GET_DATA_PACKET     0x04	//下位机返回的数据包
#define SWITCH_TO_MEASURE   0x05	//切换到连续数据模式
#define MEASURE_MODE        0x06	//测量模式


#define NONE_SSCP           0X00
#define SEND_MESSAGE        0X01
#define GET_FIFO_STATUS     0X02
#define GET_RESPONSE        0X03
#define GET_MEASURE         0X04


#define SESSION_NAME_TEMPERATURE                0x0000
#define SESSION_NAME_RAW_ACC                    0x0400
#define SESSION_NAME_RAW_GYRO                   0x0401
#define SESSION_NAME_RAW_MAG                    0x0402
#define SESSION_NAME_CAL_ACC                    0x0800
#define SESSION_NAME_KAL_ACC                    0x0801
#define SESSION_NAME_LINEAR_ACC                 0x0802
#define SESSION_NAME_HEAVE_MOTION               0x0803
#define SESSION_NAME_DELTA_V_ACC                0x0804
#define SESSION_NAME_CAL_GYRO                   0x0C00
#define SESSION_NAME_KAL_GYRO                   0x0C01
#define SESSION_NAME_DELTA_Q_GYRO               0x0C02

#define SESSION_NAME_CAL_MAG                    0x1000
#define SESSION_NAME_KAL_MAG                    0x1001
#define SESSION_NAME_MAG_DEV                    0x1002
#define SESSION_NAME_MAG_DIS_PCT                0x1003

#define SESSION_NAME_CAL_BARO                   0x1400
#define SESSION_NAME_KAL_BARO                   0x1401


#define SESSION_NAME_QUAT                       0x3000
#define SESSION_NAME_EULER                      0x3001
#define SESSION_NAME_ROTATION_M                 0x3002

#define SESSION_NAME_POSITION_ALTITUDE          0x3400
#define SESSION_NAME_POSITION_ECEF              0x3401
#define SESSION_NAME_POSITION_LATLON            0x3402
#define SESSION_NAME_POSITION_VELOCITY          0x3800
#define SESSION_NAME_POSITION_DISTANCE          0x3403


#define SESSION_NAME_PACKET_COUNTER             0x4000
#define SESSION_NAME_UTC_TIME                   0x4001
#define SESSION_NAME_OS_TIME                    0x4002
#define SESSION_NAME_SAMPLE_TIME_FINE           0x4003
#define SESSION_NAME_SAMPLE_TIME_COARSE         0x4004
#define SESSION_NAME_ITOW                       0x4005
#define SESSION_NAME_DELTA_T                    0x4006

#define SESSION_NAME_STATUS_WORD                0x4400
#define SESSION_NAME_RSSI                       0x4401
#define SESSION_NAME_CPU_USAGE                  0x4402



#define TEMPERATURE                0
#define RAW_ACC                    1
#define RAW_GYRO                   2
#define RAW_MAG                    3
#define CAL_ACC                    4
#define KAL_ACC                    5
#define LINEAR_ACC                 6
#define HEAVE_MOTION               7
#define DELTA_V_ACC                8
#define CAL_GYRO                   9
#define KAL_GYRO                   10
#define DELTA_Q_GYRO               11

#define CAL_MAG                    12
#define KAL_MAG                    13
#define MAG_DEV                    14
#define MAG_DIS_PCT                15

#define CAL_BARO                   16
#define KAL_BARO                   17

#define GNSS_PVT                   18
#define GNSS_SATELITTE             19

#define GPS_DOP                    20
#define GPS_SOL                    21
#define GPS_TIME                   22
#define GPS_SV                     23

#define QUAT                       24
#define EULER                      25
#define ROTATION_M                 26

#define POSITION_ALTITUDE          27
#define POSITION_ECEF              28
#define POSITION_LATLON            29
#define POSITION_VELOCITY          30
#define POSITION_DISTANCE          31


#define PACKET_COUNTER             32
#define UTC_TIME                   33
#define OS_TIME                    34
#define SAMPLE_TIME_FINE           35
#define SAMPLE_TIME_COARSE         36
#define ITOW                       37
#define DELTA_T                    38

#define STATUS_WORD                39
#define RSSI                       40
#define CPU_USAGE                  41





#define RETURN_CODE_OK                          0x00
#define RETURN_CODE_ERROR_CLASS                 0x01
#define RETURN_CODE_ERROR_CMD                   0x02
#define RETURN_CODE_ERROR_CRC                   0x03
#define RETURN_CODE_ERROR_FOOTER                0x04
#define RETURN_CODE_ERROR_PAYLOAD_LEN           0x05
#define RETURN_CODE_ERROR_OPERATION             0x06
#define RETURN_CODE_ERROR_UNSUPPORT             0x07
#define RETURN_CODE_ERROR_TIME_OUT              0x08
#define RETURN_CODE_ERROR_FLASH_OPERATION       0x09
#define RETURN_CODE_ERROR_INVALID_PARA          0x0A
#define RETURN_CODE_ERROR_FIRMWARE_UPDATE       0x0B
#define RETURN_CODE_ERROR_STATE                 0x0C
#define RETURN_CODE_ERROR_UART_BANDWIDTH        0x0D








#include "main.h"
#include "stm32f4xx_hal.h"
//#include "ATOM_i2c.h"
//#include "ATOM_gpio.h"
//#include "ATOM_dma.h"
//#include "ATOM_systick.h"
#include <string.h>











#endif

