/*
 * @Author: your name
 * @Date: 2021-05-18 17:29:54
 * @LastEditTime: 2021-05-19 15:44:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \electromagnetic_launch\lib\diancitanshe.h
 */
#ifndef __ELECTRONIC_H__
#define __ELECTRONIC_H__

#include "main.h"
#include "olediic.h"

#define CHARGE_VALUE 		HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)
#define DISCHARGE_VALUE		HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)
#define SHOOT_VALUE		HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)

#define CONTROLED_BY_MCU 1
#define CONTROLED_BY_EXTI 0

#define TX_HEAD_CHANNEL 0
#define TX_STATE_CHANNEL 1
#define TX_VOLTAGE_CHANNEL 2
#define TX_BCC_CHANNEL 4
#define TX_END_CHANNEL 5

#define RX_HEAD_CHANNEL 0
#define RX_STATE_CHANNEL 1
#define RX_VOLTAGE_CHANNEL 2
#define RX_BCC_CHANNEL 4
#define RX_END_CHANNEL 5

#define HEAD 0x74
#define END 0xb8

#define MAX_CAP_VALUE 400
#define MIN_CAP_VALUE 0

void START_CHARGING(void);
void STOP_CHARGING(void);
void START_DISCHARGING(void);
void STOP_DISCHARGING(void);
void START_SHOOTING(void);
void STOP_SHOOTING(void);
void mcu_control(void);
void exti_control(void);
void electronic_dispatch(void);
void set_actual_cap_value(void);

extern uint8_t received_data[6];
extern uint8_t transmit_data[6];
extern uint8_t control_source;
extern int16_t expert_cap_value;
extern int16_t actual_cap_value;
#endif
