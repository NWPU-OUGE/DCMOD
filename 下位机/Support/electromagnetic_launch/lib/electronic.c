/*
 * @Author: your name
 * @Date: 2021-05-18 17:29:54
 * @LastEditTime: 2021-05-19 15:49:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \electromagnetic_launch\lib\diancitanshe.c
 */
#include "electronic.h"


void START_CHARGING(void)
{
    HAL_GPIO_WritePin(CHARGE_GPIO_Port,CHARGE_Pin,GPIO_PIN_SET);
    OLED_ShowString(80,2," ",16);
    OLED_ShowString(80,2,"Y",16);
}

void STOP_CHARGING(void)
{
    HAL_GPIO_WritePin(CHARGE_GPIO_Port,CHARGE_Pin,GPIO_PIN_RESET);
    OLED_ShowString(80,2," ",16);
    OLED_ShowString(80,2,"N",16);
}

void START_DISCHARGING(void)
{
    HAL_GPIO_WritePin(FANGDIAN_GPIO_Port,FANGDIAN_Pin,GPIO_PIN_SET);
    OLED_ShowString(80,4," ",16);
    OLED_ShowString(80,4,"Y",16);
}

void STOP_DISCHARGING(void)
{
    HAL_GPIO_WritePin(FANGDIAN_GPIO_Port,FANGDIAN_Pin,GPIO_PIN_RESET);
    OLED_ShowString(80,4," ",16);
    OLED_ShowString(80,4,"N",16);
}

void START_SHOOTING(void)
{
    HAL_GPIO_WritePin(IGBT_GPIO_Port,IGBT_Pin,GPIO_PIN_SET);
    OLED_ShowString(80,6," ",16);
    OLED_ShowString(80,6,"Y",16);
}

void STOP_SHOOTING(void)
{
    HAL_GPIO_WritePin(IGBT_GPIO_Port,IGBT_Pin,GPIO_PIN_RESET);
    OLED_ShowString(80,6," ",16);
    OLED_ShowString(80,6,"N",16);
}

void set_actual_cap_value()
{
    if(actual_cap_value<expert_cap_value-8 && actual_cap_value < MAX_CAP_VALUE)
    {   STOP_SHOOTING();
        START_CHARGING();
        STOP_DISCHARGING();
    }
    else if(actual_cap_value > expert_cap_value-8 && actual_cap_value < expert_cap_value+8)
    {   STOP_SHOOTING();
        STOP_CHARGING();
        STOP_DISCHARGING();
    }
    else if(actual_cap_value  > expert_cap_value+8  && actual_cap_value > MIN_CAP_VALUE)
    {   STOP_SHOOTING();
        STOP_CHARGING();
        START_DISCHARGING();
    }
}

void mcu_control()
{
    set_actual_cap_value();
    HAL_Delay(5);
    if(received_data[1]==0x01)
    {
        STOP_CHARGING();
        STOP_DISCHARGING();
        HAL_Delay(10);
        START_SHOOTING();
        HAL_Delay(100);
        STOP_SHOOTING();
        HAL_Delay(2500);
    }
    else
    {
        STOP_SHOOTING();
    }
    HAL_Delay(20);
}

void exti_control()
{
    if(CHARGE_VALUE==GPIO_PIN_RESET)
    {
        START_CHARGING();
    }
    else
    {
        STOP_CHARGING();
    }

    if(DISCHARGE_VALUE==GPIO_PIN_RESET)
    {
        START_DISCHARGING();
    }
    else
    {
        STOP_DISCHARGING();
    }

    if(SHOOT_VALUE==GPIO_PIN_RESET)
    {
        START_SHOOTING();
    }
    else
    {
        STOP_SHOOTING();
    }
}


void electronic_dispatch()
{
    if(control_source==CONTROLED_BY_MCU)
    {
				OLED_ShowString(100,6," ",16);
				OLED_ShowString(100,6,"M",16);
        mcu_control();
    }

    else if(control_source==CONTROLED_BY_EXTI)
    {
				OLED_ShowString(100,6," ",16);
				OLED_ShowString(100,6,"E",16);
        exti_control();
    }
}
