/*
 * @Author: your name
 * @Date: 2021-05-06 21:09:19
 * @LastEditTime: 2021-05-15 08:34:38
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\protocol_receive_task.c
 */
/**
 * @file protocol_receive_task.c
 * @author yzz 
 * @brief 
 * @version 0.1
 * @date 2021-04-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "protocol_receive_task.h"

/* fifo协议接收队列 */
fifo_s_t protocol_receive_fifo;
uint8_t protocol_receive_fifo_buf[PROTOCOL_FIFO_BUF_LENGTH];
/* 数据接收缓存区 */
uint8_t usb_rcv_buf[256] = {0};

uint32_t payload_len[1] = {0};



/**
 * @brief 不断接受usb信息并存储到接收fifo中
 * 
 */
void protocol_receive_task(void const * argument)
{
    fifo_s_init(&protocol_receive_fifo, protocol_receive_fifo_buf, PROTOCOL_FIFO_BUF_LENGTH);
//    protocol_unpack* protocol_obj = &protocol_unpack_obj;
	payload_len[0] = 60;

    /* 等待其他线程初始化结束 */
    osDelay(2000);


    while (1)
    {
        receive_to_fifo(usb_rcv_buf, payload_len, &protocol_receive_fifo);
        
        // if(CDC_Receive_FS(usb_rcv_buf, payload_len) == USBD_OK)
        //     if(usb_rcv_buf[0])
        //     {
        //         fifo_s_puts(&protocol_receive_fifo, (char*)usb_rcv_buf, payload_len[0]);

        //         /* 清空接受缓存区 */
		// 		memset(usb_rcv_buf, 0, 128);
        //     }
		//osDelay(1);
    }
}


