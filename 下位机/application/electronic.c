/*
 * @Author: your name
 * @Date: 2021-05-18 21:40:58
 * @LastEditTime: 2021-07-14 16:17:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\electronic.c
 */
#include "electronic.h"
#include "struct_typedef.h"
#include "cmsis_os.h"

#include "bsp_usart.h"

#include "detect_task.h"
#include "remote_control.h"
#include "unpack_task.h"

const RC_ctrl_t *electronic_rc;


electronic_t electronic;
/**
 * @brief                   初始化电磁弹射
 * @param  electronic       My Param doc
 */
void electronic_init(electronic_t* electronic);

/**
 * @brief                   获取期望电压值
 * @param  electronic       My Param doc
 */
void read_exepert_voltage(electronic_t* electronic);

/**
 * @brief                   设置期望电压值
 * @param  electronic       My Param doc
 */
void set_exepert_voltage(electronic_t* electronic);

/**
 * @brief 读取电磁弹射板子的状态
 */
void read_actual_electronic_state(void);

/**
 * @brief                   设置电磁弹射板子的状态
 * @param  electronic       My Param doc
 */
void set_actual_electronic_state(electronic_t* electronic);

void electronic_task(void const * argument)  //电磁任务
{
    //wait a time
    //空闲一段时间
    vTaskDelay(500);
    //电磁弹射初始化
    electronic_init(&electronic);
    //electronic.Rc_Flag=0;
    while(1)
    {
        read_exepert_voltage(&electronic);
        set_exepert_voltage(&electronic);
        read_actual_electronic_state();
        set_actual_electronic_state(&electronic);
        osDelay(20);
    }
}

/**
 * @brief                   初始化电磁弹射
 * @param  electronic       My Param doc
 */
void electronic_init(electronic_t* electronic)
{
    //HAL_UART_Receive_DMA(&huart6,electronic->received_data,TRANSMIT_DATA_LENGTH);// 启动DMA接收
    //获取遥控器指针
    electronic_rc = get_remote_control_point();
    //初始化要发送的数据报的内容
    electronic->transmit_data[HEAD_CHANNEL] = HEAD;
    electronic->transmit_data[END_CHANNEL] = END;
}

/**
 * @brief                   获取期望电压值
 * @param  electronic       My Param doc
 */
void read_exepert_voltage(electronic_t* electronic)
{
//读取遥控器的通道值
//！！！这里的注释千万不能取消！！！
//我的本意是电磁弹射电容值和射击值只有在右拨杆在中上时才可以设置，但是我发现这样写会产生致命bug	
//    if(electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL] == RC_CAP_CHANNEL || 
//			 electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL] == RC_SHOOT_CHANNEL)
//    {
        electronic->rc_cap_value = electronic_rc->rc.ch[ELECTRONIC_CAP_CHANNEL];
        electronic->rc_shoot_value = electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL];
//    }
}
/**
 * @brief                   设置期望电压值
 * @param  electronic       My Param doc
 */
void set_exepert_voltage(electronic_t* electronic)
{
    //更据遥控器的值改变期望的电磁弹射电压值
    if(handshake_flag == UNHANDSHAKE)
    {
        electronic->cap_change_value = electronic->rc_cap_value>>5;
        electronic->cap_expert_value += electronic->cap_change_value;
    }
    //上位机发送的数据直接代表期望的电容电压值
    else if(handshake_flag == HANDSHAKE)
    {
        electronic->cap_expert_value =electronic->rc_cap_value/1.65;//0-400
    }
    //电磁弹射电压限幅
    if(electronic->cap_expert_value<MIN_ELECTRONIC_VALUE)
    {
        electronic->cap_expert_value = MIN_ELECTRONIC_VALUE;
    }
    if(electronic->cap_expert_value>MAX_ELECTRONIC_VALUE)
    {
        electronic->cap_expert_value = MAX_ELECTRONIC_VALUE;
    }
}

/**
 * @brief 读取电磁弹射板子的状态
 */
void read_actual_electronic_state()
{
    //启动电磁弹射板子的DMA接收
    //HAL_UART_Receive_DMA(&huart6,dianci.buf,7);// 启动DMA接收
}

/**
 * @brief                   设置电磁弹射板子的状态
 * @param  electronic       My Param doc
 */
void set_actual_electronic_state(electronic_t* electronic)
{
    //if(electronic->Rc_Flag==0) return ;
    //控制射击状态
    if(electronic->rc_shoot_value == SHOOT)
    {
        electronic->transmit_data[STATE_CHANNEL] = SHOOT;
    }
    else
    {
        electronic->transmit_data[STATE_CHANNEL] = UNSHOOT;
    }
    //发送期望电容电量
    electronic->transmit_data[CAP_CHANNEL] = electronic->cap_expert_value;
    electronic->transmit_data[CAP_CHANNEL+1] = (electronic->cap_expert_value)>>8;
    //bcc校验
    electronic->transmit_data[BCC_CHANNEL] = check_bcc(electronic->transmit_data,BCC_CHANNEL);
    //串口发送
    HAL_UART_Transmit(&huart6,electronic->transmit_data,sizeof(electronic->transmit_data),0xffff);
    //DMA发送
    //HAL_UART_Transmit_DMA(&huart6,electronic->transmit_data,sizeof(electronic->transmit_data));
    //electronic->Rc_Flag=0;
}

//void USART6_IRQHandler(void);
