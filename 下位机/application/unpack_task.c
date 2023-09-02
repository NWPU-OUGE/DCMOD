/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       unpack_task.c/h
  * @brief      communication task,
  *             协议解包任务
  * @note
  * @history
  *  Version    Date            Author					Modification
  *  V1.0.0     Apr-16-2021     Yang-Zhengzhong         1. unfinished
  *  V1.0.0     Apr-18-2021     Yang-Zhengzhong         1. finish the frame
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  */
#include "unpack_task.h"

extern fifo_s_t protocol_receive_fifo;
extern uint8_t protocol_receive_fifo_buf[PROTOCOL_FIFO_BUF_LENGTH];

/* 实际储存信息的位置 */
message_n command_message[1];
message_s car_real_message[1];

/* 上下位机通信过程使用的结构体 */
protocol_unpack protocol_unpack_obj;


/* 握手成功与否标志位, 握手成功返回0，否则返回1  */
volatile uint8_t handshake_flag = 1;

/**
 * @brief 解包线程任务
 * 
 */
void unpack_task(void const * argument);

/**
 * @brief 进行bcc校验，返回校验结果
 * 
 * @param data 要校验的数组地址
 * @param sum 校验的数据位数（单位：字节）
 * @return unsigned int 
 */
unsigned int check_bcc(unsigned char * data,unsigned int sum);

/**
 * @brief usb解包任务
 * 
 */
void usb_unpack(void);

/**
 * @brief 握手检测函数
 * 
 * @return 握手成功返回0，否则返回1 
 */
uint8_t handshake_check(void);




/**
 * @brief 解包线程任务
 * 
 */
void unpack_task(void const * argument)
{
    /* 等待其他线程初始化 */
    osDelay(2000);

    while(1)
    {
		while(handshake_flag)
		{
			handshake_flag = handshake_check();
		}

        usb_unpack();
    }

}



/**
 * @brief 进行bcc校验，返回校验结果
 * 
 * @param data 要校验的数组地址
 * @param sum 校验的数据位数（单位：字节）
 * @return unsigned int 
 */
unsigned int check_bcc(unsigned char * data,unsigned int sum)
{
  unsigned char bcc_num = 0;
  unsigned int i = 0;
  while(i < sum)
  {
    bcc_num ^= *data;
    data++;
    i++;
  }
  return bcc_num;
}


/**
 * @brief 握手检测函数
 * 
 * @return 握手成功返回0，否则返回1 
 */
uint8_t handshake_check(void)
{
    /* 用来储存从fifo中提取的数据 */
    static uint8_t byte = 0;

    /* 解包步骤 */
    int step = 0;

    while(fifo_s_used(&protocol_receive_fifo))
    {
        switch(step)
        {
            case 0:{
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0x41)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 1:{
                if(byte == 0x78)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 2:{
                if(byte == 0xff)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 3:{
                if(byte == 0x01)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 4:{
                if(byte == 0x02)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 5: {
                if (byte == 0x00)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 6: {
                if (byte == 0xc5)
                {
                    byte = fifo_s_get(&protocol_receive_fifo);
                    step++;
                }
                else return 1;
            }

            case 7:{
                if(byte == 0x6d)
                {
                    step = 0;
                    return 0;
                }
                else return 1;
            }
        }

    }
    return 1;
}



/**
 * @brief usb解包任务
 * 
 */
void usb_unpack(void)
{
    /* 用来储存从fifo中提取的数据 */
    static uint8_t byte = 0;

    /* 解包步骤 */
    static int step = 0;

    /* 指向数据包中的位置 */
    static int index = 0;

    /* 记录数据包长度 */
    uint8_t length = 0x2c;

    protocol_unpack* p_obj = &protocol_unpack_obj;

    while(fifo_s_used(&protocol_receive_fifo))
    {
        switch(step)
        {
            /* 总包头校验 */
            case HEADER_CHECK:{
                byte = fifo_s_get(&protocol_receive_fifo);
                index = 0;
                if (byte == 0x41)
                {
                    p_obj->unpack_buf[index++] = byte;
                    byte = fifo_s_get(&protocol_receive_fifo);
                    if (byte == 0x78)
                    {
                        p_obj->unpack_buf[index++] = byte;
                        step++;
                    }
                    else step = HEADER_CHECK;
                }
                else step = HEADER_CHECK;
            }break;

            /* ID校验 */
            case MADDR__MODE:{
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0xff)
                {
                    p_obj->unpack_buf[index++] = byte;
                    byte = fifo_s_get(&protocol_receive_fifo);
                    switch (byte)
                    {
                        /* 空置模式(未实装) */
                        case 0x00:{
                            step = HEADER_CHECK;
                        }break;

                        /* 配置模式(未实装) */
                        case 0x01:{
                            step = HEADER_CHECK;
                        }break;
                        
                        /* 数据选择模式(未实装) */
                        case 0x02:{
                            step = HEADER_CHECK;
                        }break;

                        /* 返回速度设置(未实装) */
                        case 0x03:{
                            step = HEADER_CHECK;
                        }break;

                        /* 单次发送(未实装) */
                        case 0x04:{
                            step = HEADER_CHECK;
                        }break;

                        /* 连续收发（默认模式） */
                        case 0x05:{
                            p_obj->unpack_buf[index++] = byte;
                            step++;

                            /* 暂时的，等鲁大师持球和击球杆高度写完再确定 */
//                            p_obj->protocol_struct->payload_length = length;
                        }break;

                        /* 测量模式(未实装) */
                        case 0x06:{
                            step = HEADER_CHECK;
                        }break;

                        default:{
                            step = HEADER_CHECK;
                        }break;
                    }
                }
                else step = HEADER_CHECK;
            }break;

            /* 信息类型检验(目前仅支持0x01) */
            case MESSAGE_ID:
            {
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0x01)
                {
                    p_obj->unpack_buf[index++] = byte;
                    byte = fifo_s_get(&protocol_receive_fifo);
                    if(byte == length)
                    {
                        p_obj->unpack_buf[index++] = byte;
                        step++;
                    }
                }
                else step = HEADER_CHECK;
            }break;

            /* 数据内容 */
            case PACKET_MESSAGE:
            {
                byte = fifo_s_get(&protocol_receive_fifo);
                switch(byte)
                {
                    /* 直接控制信息 */
                    case 0x1e:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* 数据包长度标志 */
                        if(byte == 0x0c)
                        {
                            p_obj->unpack_buf[index++] = byte;
                            for (int i = 0; i < 2 * 6; i++)
                            {
                                byte = fifo_s_get(&protocol_receive_fifo);
                                p_obj->unpack_buf[index++] = byte;
                            }
                        }
                        else step = HEADER_CHECK;
                    }break;

                    /* 车体实际速度信息 */
                    case 0x19:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* 数据包长度标志 */
                        if(byte == 0x0c)
                        {
                            p_obj->unpack_buf[index++] = byte;
                        
                            for(int i = 0; i < 4 * 3; i++)
                            {
                                byte = fifo_s_get(&protocol_receive_fifo);
                                p_obj->unpack_buf[index++] = byte;
                            }
                        }
                        else step = HEADER_CHECK;
                    }break;

                    /* 轮速信息 */
                    case 0x22:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* 数据包长度标志 */
                        if(byte == 0x08)
                        {
                            p_obj->unpack_buf[index++] = byte;
                        
                            for(int i = 0; i < 4 * 2; i++)
                            {
                                byte = fifo_s_get(&protocol_receive_fifo);
                                p_obj->unpack_buf[index++] = byte;
                            }
                        }
                        else step = HEADER_CHECK;
                    }break;

                    /* 击球杆高度信息 */
                    case 0x31:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* 数据包长度标志 */
                        if(byte == 0x04)
                        {
                            p_obj->unpack_buf[index++] = byte;
                        
                            for(int i = 0; i < 4; i++)
                            {
                                byte = fifo_s_get(&protocol_receive_fifo);
                                p_obj->unpack_buf[index++] = byte;
                            }
                        }
                        else step = HEADER_CHECK;
                    }break;


                    default:{
                        step++;
                    }break;
                }
            }break;

            /* bcc校验 */
            case BCC_CHECK:{
              // if(byte == check_bcc(p_obj->unpack_buf, index))
                //{
                    p_obj->unpack_buf[index++] = byte;
                    step++;
                //}
               // else step = 0;
            }break;

            /* 包尾检验 */
            case TAIL_CHECK:{
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0x6d)
                {
                    p_obj->unpack_buf[index++] = byte;
                    step++;
                }
                else step = 0;
            }break;

            /* 数据迁移，将解包缓存区中的数据信息储存在结构体成员地址中 */
            case MESSAGE_OVERWRITE: {
                command_message->car_command[0] = *(int16_t*)(p_obj->unpack_buf + 8);
                command_message->car_command[1] = *(int16_t*)(p_obj->unpack_buf + 10);
                command_message->car_command[2] = *(int16_t*)(p_obj->unpack_buf + 12);
                command_message->shoot_command[0] = *(int16_t*)(p_obj->unpack_buf + 14);
                command_message->shoot_command[1] = *(int16_t*)(p_obj->unpack_buf + 16);
                command_message->status_command[0] = p_obj->unpack_buf[18];
                command_message->status_command[1] = p_obj->unpack_buf[19];

                car_real_message->real_velocity_msg[0] = *(int32_t*)(p_obj->raw_buf + 22);
                car_real_message->real_velocity_msg[1] = *(int32_t*)(p_obj->raw_buf + 26);
                car_real_message->real_velocity_msg[2] = *(int32_t*)(p_obj->raw_buf + 30);

                car_real_message->wheel_speed_msg[0] = *(int32_t*)(p_obj->raw_buf + 36);
                car_real_message->wheel_speed_msg[1] = *(int32_t*)(p_obj->raw_buf + 40);

                car_real_message->ball_msg[0] = *(int16_t*)(p_obj->unpack_buf + 46);
                car_real_message->ball_msg[1] = *(int16_t*)(p_obj->unpack_buf + 48);

                step = 0;
            }
        }
    }

}


/**
 * @brief 获取command_message的地址值
 * 
 * @return const fp32* 
 */
const message_n *get_command_message_point(void)
{
    return command_message;
}


/**
 * @brief 获取car_real_message的地址值
 * 
 * @return const fp32* 
 */
const message_s *get_car_real_message_point(void)
{
    return car_real_message;
}
