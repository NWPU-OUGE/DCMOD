/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       protocol_send_task.c/h
  * @brief      communication task,
  *             协议通讯任务
  * @note
  * @history
  *  Version    Date            Author					        Modification
  *  V1.0.0     Apr-11-2021     Yang-Zhengzhong         1. unfinished
  *  V1.0.0     Apr-22-2021     yzz                     beta finished
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  */



#include"protocol_send_task.h"
static uint8_t usb_buf[256];
extern volatile uint8_t handshake_flag;
extern message_s car_real_message[1];
/* 挥手标志位，1标志上位机在线，0标志上位机不在线 */
volatile uint8_t wave_flag = 1;

/* 数据包总长度，说不定就用到了 */
uint8_t fullpacket_lengh;

//41 78 FF 01 82 00 45 6D
uint8_t handshake_buf[10] = { 0x41,0x78,0xff,0x01,0x82,0x00,0x45,0x6d,0x00,0x00 };


/**
 * @brief   初始化发送缓冲区
 * 
 * @param   buf  即发送缓冲区
 * @return  数据包总长度
 */
uint8_t raw_buf_init(uint8_t *buf);

/**
  * @brief          bcc异或校验
  * @param[in]      数据地址指针
  * @param[in]      校验数据个数
  * @retval         校验结果
  */
extern unsigned int check_bcc(unsigned char* data, unsigned int sum);

/**
 * @brief usb在线状态检测函数
 * 
 */
void usb_status_check(void);


/**
 * @brief 重定向printf函数
 * 
 * @param fmt 
 * @param ... 
 */
static void usb_printf(const char *fmt,...)
{
    static va_list ap;
    uint16_t len = 0;

    va_start(ap, fmt);

    len = vsprintf((char *)usb_buf, fmt, ap);

    va_end(ap);


    CDC_Transmit_FS(usb_buf, len);
}


/**
  * @brief          协议通讯任务
  * @param[in]      void
  * @retval         none
  */
void protocol_send_task(void const * argument)
{
    protocol_unpack* p_obj = &protocol_unpack_obj;

    /* 以下为初始化发送缓冲区 */
    fullpacket_lengh = raw_buf_init(p_obj->raw_buf);

    p_obj->command_message = get_command_message_point();
    p_obj->car_real_message = get_car_real_message_point();

    /* 调试过程中试运行的 */
    command_message->car_command[0] = 1;
    command_message->car_command[1] = 2;
    command_message->car_command[2] = 3;


    /* 进行握手 */
//    usb_printf("Waiting for handshake...");
   // while(handshake_flag){}
    CDC_Transmit_FS(handshake_buf, 8);
    osDelay(2000);
    

    while(1)
    {
        /* 检测usb是否在线 */
     //   usb_status_check();

        /* 上位机掉线后需再次握手才能重连 */
     //   if(wave_flag == 0)
      //  {
            /* 进行握手 */
//            usb_printf("Waiting for handshake...");
        //    while(handshake_flag){}
       //     CDC_Transmit_FS(handshake_buf, 8);
      //      osDelay(2000);
      //  }
        /*  判断上位机在线时才发送数据包和进行数据迁移  */
            *(int16_t*)(p_obj->raw_buf + 8) = p_obj->command_message->car_command[0];
            *(int16_t*)(p_obj->raw_buf + 10) = p_obj->command_message->car_command[1];
            *(int16_t*)(p_obj->raw_buf + 12) = p_obj->command_message->car_command[2];
            *(int16_t*)(p_obj->raw_buf + 14) = p_obj->command_message->shoot_command[0];
            *(int16_t*)(p_obj->raw_buf + 16) = p_obj->command_message->shoot_command[1];
            *(int16_t*)(p_obj->raw_buf + 17) = p_obj->command_message->status_command[0];
            *(int16_t*)(p_obj->raw_buf + 18) = p_obj->command_message->status_command[1];

            *(float*)(p_obj->raw_buf + 22) = p_obj->car_real_message->real_velocity_msg[0];
            *(float*)(p_obj->raw_buf + 26) = p_obj->car_real_message->real_velocity_msg[1];
            *(float*)(p_obj->raw_buf + 30) = p_obj->car_real_message->real_velocity_msg[2];

            *(int32_t*)(p_obj->raw_buf + 36) = p_obj->car_real_message->wheel_speed_msg[0];
            *(int32_t*)(p_obj->raw_buf + 40) = p_obj->car_real_message->wheel_speed_msg[1];

            *(int16_t*)(p_obj->raw_buf + 46) = p_obj->car_real_message->ball_msg[0];
            *(int16_t*)(p_obj->raw_buf + 48) = p_obj->car_real_message->ball_msg[1];
            CDC_Transmit_FS(p_obj->raw_buf, fullpacket_lengh);
        

        

        /* 调试中，当前发送频率100Hz */
        osDelay(10);
    }
}


/**
 * @brief usb在线状态检测函数
 * 
 */
void usb_status_check(void)
{
  if(hUsbDeviceFS.dev_state != 0)
  {
    /* USB在线 */
    if(hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
    {
		wave_flag = 1;
		return;
    }
    /* USB离线时挥手标志位置0 */
    else if (hUsbDeviceFS.dev_state == USBD_STATE_SUSPENDED)
    {
		wave_flag = 0;
		handshake_flag = 1;
    }
  } 
}




/**
 * @brief   初始化发送缓冲区
 * 
 * @param   buf  即发送缓冲区
 * @return  数据包总长度
 */
uint8_t raw_buf_init(uint8_t *buf)
{
  static uint8_t index;
  index = 0;

  buf[index++] = 0x41;
  buf[index++] = 0x78;
  buf[index++] = 0xff;
  buf[index++] = 0x05;
  buf[index++] = 0x01;
  buf[index++] = 0x2c;
    
  buf[index++] = 0x1e;
  buf[index++] = 0x0c;
  for(uint8_t i = 0; i < 0x0c; i++)
  {
    buf[index++] = i;
  }

  buf[index++] = 0x19;
  buf[index++] = 0x0c;
  for(uint8_t i = 0; i < 0x0c; i++)
  {
    buf[index++] = i;
  }

  buf[index++] = 0x22;
  buf[index++] = 0x08;
  for(uint8_t i = 0; i < 8; i++)
  {
    buf[index++] = i;
  }

  buf[index++] = 0x31;
  buf[index++] = 0x04;
  for(uint8_t i = 0; i < 4; i++)
  {
    buf[index++] = i;
  }

  buf[index] = check_bcc(buf, index);
  index++;
  buf[index++] = 0x6d;

  return index;
}
