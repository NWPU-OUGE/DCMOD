/**
 * @file bsp_servo_pwm.c
 * @brief 
 * @author LuYidan (luyidan81192@163.com)
 * @version 1.0
 * @date 2021-04-03
 * 
 * @copyright Copyright (c) 2021  西北工业大学足球机器人基地轮式组
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-04-03 <td>1.0     <td>wangh     <td>内容
 * </table>
 */
#include "bsp_servo_pwm.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;

void servo_pwm_set(uint16_t pwm, uint8_t i)
{
    switch(i)
    {
        case 0:
        {
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, pwm);
        }break;
        case 1:
        {
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, pwm);
        }break;
        case 2:
        {
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, pwm);
        }break;
        case 3:
        {
            __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, pwm);
        }break;
    }
}
