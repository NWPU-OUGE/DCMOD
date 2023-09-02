/**
 * @file protocol_receive_task.h
 * @author yzz
 * @brief 
 * @version 0.1
 * @date 2021-04-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "fifo.h"
#include "main.h"
#include "cmsis_os.h"
#include "protocol_send_task.h"
#include "usb_task.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "unpack_task.h"


extern int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
extern protocol_unpack protocol_unpack_obj;
extern message_n command_message[1];
extern message_s car_real_message[1];
extern void protocol_receive_task(void const * argument);


