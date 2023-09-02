/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       remote_control.c/h
  * @brief      遥控器处理，遥控器是通过类似SBUS的协议传输，利用DMA传输方式节约CPU
  *             资源，利用串口空闲中断来拉起处理函数，同时提供一些掉线重启DMA，串口
  *             的方式保证热插拔的稳定性。
  * @note       该任务是通过串口中断启动，不是freeRTOS任务
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.0.0     Nov-11-2019     RM              1. support development board tpye c
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "remote_control.h"
#include "main.h"
#include "bsp_usart.h"
#include "string.h"
#include "detect_task.h"

#include "unpack_task.h"
#include "chassis_task.h"
#include "catch_task.h"


//遥控器出错数据上限
#define RC_CHANNAL_ERROR_VALUE 700

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;


/* 实际储存信息的位置 */
extern message_n command_message[1];
extern message_s car_real_message[1];
//上下位机通信过程使用的结构体
extern protocol_unpack protocol_unpack_obj;
//握手标志位
extern volatile uint8_t handshake_flag;
//底盘，持球结构体
extern chassis_move_t chassis_move;
extern catch_move_t catch_move;

/**
 * @brief                   取正函数
 * @param  value            My Param doc
 * @return int16_t
 */
static int16_t RC_abs(int16_t value);

/**
  * @brief          遥控器协议解析
  * @param[in]      sbus_buf: 原生数据指针
  * @param[out]     rc_ctrl: 遥控器数据指
  * @retval         none
  */
static void sbus_to_rc(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl);


/**
 * @brief                   上位机与遥控器数据调度
 * @param  ever_reset       My Param doc
 */
void remote_data_dispatch(int ever_reset);

/**
 * @brief                   Get the switch right value object
 * @param  sbus_buf         My Param doc
 * @return char
 */
char get_switch_right_value(volatile const uint8_t *sbus_buf);

/**
 * @brief                   又上位机的数据决定车辆的运动状态
 * @param  p_obj            My Param doc
 * @param  command_message  My Param doc
 * @param  car_real_message 上下位机通讯结构体
 * @param  rc_ctrl          车辆控制控制结构体
 */
static void usb_to_rc(protocol_unpack* p_obj,message_n* command_message, RC_ctrl_t *rc_ctrl);


static void rc_to_usb(message_s* car_real_message, chassis_move_t* chassis_move, catch_move_t* catch_move);

//remote control data
//遥控器控制变量
RC_ctrl_t rc_ctrl;
//接收原始数据，为18个字节，给了36个字节长度，防止DMA传输越界
static uint8_t sbus_rx_buf[2][SBUS_RX_BUF_NUM];


/**
  * @brief          remote control init
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          遥控器初始化
  * @param[in]      none
  * @retval         none
  */
void remote_control_init(void)
{
    RC_Init(sbus_rx_buf[0], sbus_rx_buf[1], SBUS_RX_BUF_NUM);
}
/**
  * @brief          get remote control data point
  * @param[in]      none
  * @retval         remote control data point
  */
/**
  * @brief          获取遥控器数据指针
  * @param[in]      none
  * @retval         遥控器数据指针
  */
const RC_ctrl_t *get_remote_control_point(void)
{
    return &rc_ctrl;
}

//判断遥控器数据是否出错，
uint8_t RC_data_is_error(void)
{
    //使用了go to语句 方便出错统一处理遥控器变量数据归零
    if (RC_abs(rc_ctrl.rc.ch[0]) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch[1]) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch[2]) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_abs(rc_ctrl.rc.ch[3]) > RC_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (rc_ctrl.rc.s[0] == 0)
    {
        goto error;
    }
    if (rc_ctrl.rc.s[1] == 0)
    {
        goto error;
    }
    return 0;

error:
    rc_ctrl.rc.ch[0] = 0;
    rc_ctrl.rc.ch[1] = 0;
    rc_ctrl.rc.ch[2] = 0;
    rc_ctrl.rc.ch[3] = 0;
    rc_ctrl.rc.ch[4] = 0;
    rc_ctrl.rc.s[0] = RC_SW_DOWN;
    rc_ctrl.rc.s[1] = RC_SW_DOWN;
    rc_ctrl.mouse.x = 0;
    rc_ctrl.mouse.y = 0;
    rc_ctrl.mouse.z = 0;
    rc_ctrl.mouse.press_l = 0;
    rc_ctrl.mouse.press_r = 0;
    rc_ctrl.key.v = 0;
    return 1;
}

void slove_RC_lost(void)
{
    RC_restart(SBUS_RX_BUF_NUM);
}
void slove_data_error(void)
{
    RC_restart(SBUS_RX_BUF_NUM);
}

//串口中断
void USART3_IRQHandler(void)
{
    if(huart3.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_PEFLAG(&huart3);
    }
    else if(USART3->SR & UART_FLAG_IDLE)
    {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart3);

        if ((hdma_usart3_rx.Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

            //set memory buffer 1
            //设定缓冲区1
            hdma_usart3_rx.Instance->CR |= DMA_SxCR_CT;

            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if(this_time_rx_len == RC_FRAME_LENGTH)
            {
                //上位机与遥控器数据调度,并记录数据接收时间
                remote_data_dispatch(0);
                // //处理遥控器数据
                // sbus_to_rc(sbus_rx_buf[0], &rc_ctrl);
                // //记录数据接收时间
                // detect_hook(DBUS_TOE);
                //sbus_to_usart1(sbus_rx_buf[0]);
            }
        }
        else
        {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

            //set memory buffer 0
            //设定缓冲区0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);

            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if(this_time_rx_len == RC_FRAME_LENGTH)
            {
                //上位机与遥控器数据调度,并记录数据接收时间
                remote_data_dispatch(1);
                // //处理遥控器数据
                // sbus_to_rc(sbus_rx_buf[1], &rc_ctrl);
                // //记录数据接收时间
                // detect_hook(DBUS_TOE);
                //sbus_to_usart1(sbus_rx_buf[1]);
            }
        }
    }

}
/**
 * @brief                   上位机与遥控器数据调度,并记录数据接收时间
 * @param  ever_reset       My Param doc
 */
void remote_data_dispatch(int ever_reset)
{
    if(ever_reset == 0 || ever_reset == 1 )
    {
        //未握手时，车辆控制权交给遥控器
        if( handshake_flag == NOT_HANDSHAKE)
        {
            sbus_to_rc(sbus_rx_buf[ever_reset], &rc_ctrl);
					rc_to_usb(car_real_message, &chassis_move, &catch_move);
        }
        //握手时，又遥控器拨杆决定控制权
        else if(handshake_flag == HAS_HANDSHAKE)
        {
            char switch_right = get_switch_right_value(sbus_rx_buf[ever_reset]);//遥控器右拨杆的值
					rc_to_usb(car_real_message, &chassis_move, &catch_move);
            //拨杆在最下面，上位机全权控制
            if(switch_right==2)
            {
                /**
                 * @brief
                 */
               // if(protocol_unpack_obj.command_message != NULL && protocol_unpack_obj.car_real_message != NULL)
             //   {
                    usb_to_rc(&protocol_unpack_obj,command_message,&rc_ctrl);
                    rc_to_usb(car_real_message, &chassis_move, &catch_move);
              // }
            }
            //否则，遥控器全权控制
            else {
							rc_to_usb(car_real_message, &chassis_move, &catch_move);
                sbus_to_rc(sbus_rx_buf[ever_reset], &rc_ctrl);
            }
        }
        //记录数据接收时间
        detect_hook(DBUS_TOE);
    }

}
/**
 * @brief                   Get the switch right value object
 * @param  sbus_buf         My Param doc
 * @return char
 */
char get_switch_right_value(volatile const uint8_t *sbus_buf)
{
    return ((sbus_buf[5] >> 4) & 0x0003);//遥控器右拨杆的值
}

//取正函数
static int16_t RC_abs(int16_t value)
{
    if (value > 0)
    {
        return value;
    }
    else
    {
        return -value;
    }
}/* 实际储存信息的位置 */

/**
 * @brief                   又上位机的数据决定车辆的运动状态
 * @param  p_obj            上下位机通讯结构体
 * @param  rc_ctrl          车辆控制控制结构体
 */


/**
 * @brief                   又上位机的数据决定车辆的运动状态
 * @param  p_obj            My Param doc
 * @param  command_message  My Param doc
 * @param  car_real_message 上下位机通讯结构体
 * @param  rc_ctrl          车辆控制控制结构体
 */
static void usb_to_rc(protocol_unpack* p_obj,message_n* command_message, RC_ctrl_t *rc_ctrl)
{

    rc_ctrl->rc.ch[0] = command_message->car_command[0];     //y轴速度
    rc_ctrl->rc.ch[1] = command_message->car_command[1];     //x轴速度
    rc_ctrl->rc.ch[2] = command_message->car_command[2];     //w轴速度
    rc_ctrl->rc.ch[3] = command_message->shoot_command[0];   //击球杆高度
    rc_ctrl->rc.ch[4] = command_message->shoot_command[1]; 	 //电容充放电
    rc_ctrl->rc.s[0] = command_message->status_command[0];   //!< Switch right	2:无力3：底盘1：工程
    rc_ctrl->rc.s[1] = command_message->status_command[0];   //!< Switch left		2:无力3：持球1：射击
}

/**
 * @brief
 * @param  car_real_message My Param doc
 * @param  chassis_move     My Param doc
 * @param  catch_move       My Param doc
 */
static void rc_to_usb(message_s* car_real_message, chassis_move_t* chassis_move, catch_move_t* catch_move)
{
	 
    car_real_message->real_velocity_msg[0]=chassis_move->vx;
    car_real_message->real_velocity_msg[1]=chassis_move->vy;
    car_real_message->real_velocity_msg[2]=chassis_move->wz;
    car_real_message->ball_msg[1] = (catch_move->is_catched)<<8;
}
/**
  * @brief          remote control protocol resolution
  * @param[in]      sbus_buf: raw data point
  * @param[out]     rc_ctrl: remote control data struct point
  * @retval         none
  */
/**
  * @brief          遥控器协议解析
  * @param[in]      sbus_buf: 原生数据指针
  * @param[out]     rc_ctrl: 遥控器数据指
  * @retval         none
  */
static void sbus_to_rc(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl)
{
    if (sbus_buf == NULL || rc_ctrl == NULL)
    {
        return;
    }

    rc_ctrl->rc.ch[0] = (sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff;        //!< Channel 0
    rc_ctrl->rc.ch[1] = ((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff; //!< Channel 1
    rc_ctrl->rc.ch[2] = ((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) |          //!< Channel 2
                         (sbus_buf[4] << 10)) &0x07ff;
    rc_ctrl->rc.ch[3] = ((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff; //!< Channel 3
    rc_ctrl->rc.s[0] = ((sbus_buf[5] >> 4) & 0x0003);                  //!< Switch left
    rc_ctrl->rc.s[1] = ((sbus_buf[5] >> 4) & 0x000C) >> 2;                       //!< Switch right
    rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);                    //!< Mouse X axis
    rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);                    //!< Mouse Y axis
    rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);                  //!< Mouse Z axis
    rc_ctrl->mouse.press_l = sbus_buf[12];                                  //!< Mouse Left Is Press ?
    rc_ctrl->mouse.press_r = sbus_buf[13];                                  //!< Mouse Right Is Press ?
    rc_ctrl->key.v = sbus_buf[14] | (sbus_buf[15] << 8);                    //!< KeyBoard value
    rc_ctrl->rc.ch[4] = sbus_buf[16] | (sbus_buf[17] << 8);                 //NULL

    rc_ctrl->rc.ch[0] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[1] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[2] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[3] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[4] -= RC_CH_VALUE_OFFSET;
}

/**
  * @brief          send sbus data by usart1, called in usart3_IRQHandle
  * @param[in]      sbus: sbus data, 18 bytes
  * @retval         none
  */
/**
  * @brief          通过usart1发送sbus数据,在usart3_IRQHandle调用
  * @param[in]      sbus: sbus数据, 18字节
  * @retval         none
  */
void sbus_to_usart1(uint8_t *sbus)
{
    static uint8_t usart_tx_buf[20];
    static uint8_t i =0;
    usart_tx_buf[0] = 0xA6;
    memcpy(usart_tx_buf + 1, sbus, 18);
    for(i = 0, usart_tx_buf[19] = 0; i < 19; i++)
    {
        usart_tx_buf[19] += usart_tx_buf[i];
    }
    usart1_tx_dma_enable(usart_tx_buf, 20);
}

