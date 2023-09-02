/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       unpack_task.c/h
  * @brief      communication task,
  *             Э��������
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

/* ʵ�ʴ�����Ϣ��λ�� */
message_n command_message[1];
message_s car_real_message[1];

/* ����λ��ͨ�Ź���ʹ�õĽṹ�� */
protocol_unpack protocol_unpack_obj;


/* ���ֳɹ�����־λ, ���ֳɹ�����0�����򷵻�1  */
volatile uint8_t handshake_flag = 1;

/**
 * @brief ����߳�����
 * 
 */
void unpack_task(void const * argument);

/**
 * @brief ����bccУ�飬����У����
 * 
 * @param data ҪУ��������ַ
 * @param sum У�������λ������λ���ֽڣ�
 * @return unsigned int 
 */
unsigned int check_bcc(unsigned char * data,unsigned int sum);

/**
 * @brief usb�������
 * 
 */
void usb_unpack(void);

/**
 * @brief ���ּ�⺯��
 * 
 * @return ���ֳɹ�����0�����򷵻�1 
 */
uint8_t handshake_check(void);




/**
 * @brief ����߳�����
 * 
 */
void unpack_task(void const * argument)
{
    /* �ȴ������̳߳�ʼ�� */
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
 * @brief ����bccУ�飬����У����
 * 
 * @param data ҪУ��������ַ
 * @param sum У�������λ������λ���ֽڣ�
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
 * @brief ���ּ�⺯��
 * 
 * @return ���ֳɹ�����0�����򷵻�1 
 */
uint8_t handshake_check(void)
{
    /* ���������fifo����ȡ������ */
    static uint8_t byte = 0;

    /* ������� */
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
 * @brief usb�������
 * 
 */
void usb_unpack(void)
{
    /* ���������fifo����ȡ������ */
    static uint8_t byte = 0;

    /* ������� */
    static int step = 0;

    /* ָ�����ݰ��е�λ�� */
    static int index = 0;

    /* ��¼���ݰ����� */
    uint8_t length = 0x2c;

    protocol_unpack* p_obj = &protocol_unpack_obj;

    while(fifo_s_used(&protocol_receive_fifo))
    {
        switch(step)
        {
            /* �ܰ�ͷУ�� */
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

            /* IDУ�� */
            case MADDR__MODE:{
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0xff)
                {
                    p_obj->unpack_buf[index++] = byte;
                    byte = fifo_s_get(&protocol_receive_fifo);
                    switch (byte)
                    {
                        /* ����ģʽ(δʵװ) */
                        case 0x00:{
                            step = HEADER_CHECK;
                        }break;

                        /* ����ģʽ(δʵװ) */
                        case 0x01:{
                            step = HEADER_CHECK;
                        }break;
                        
                        /* ����ѡ��ģʽ(δʵװ) */
                        case 0x02:{
                            step = HEADER_CHECK;
                        }break;

                        /* �����ٶ�����(δʵװ) */
                        case 0x03:{
                            step = HEADER_CHECK;
                        }break;

                        /* ���η���(δʵװ) */
                        case 0x04:{
                            step = HEADER_CHECK;
                        }break;

                        /* �����շ���Ĭ��ģʽ�� */
                        case 0x05:{
                            p_obj->unpack_buf[index++] = byte;
                            step++;

                            /* ��ʱ�ģ���³��ʦ����ͻ���˸߶�д����ȷ�� */
//                            p_obj->protocol_struct->payload_length = length;
                        }break;

                        /* ����ģʽ(δʵװ) */
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

            /* ��Ϣ���ͼ���(Ŀǰ��֧��0x01) */
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

            /* �������� */
            case PACKET_MESSAGE:
            {
                byte = fifo_s_get(&protocol_receive_fifo);
                switch(byte)
                {
                    /* ֱ�ӿ�����Ϣ */
                    case 0x1e:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* ���ݰ����ȱ�־ */
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

                    /* ����ʵ���ٶ���Ϣ */
                    case 0x19:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* ���ݰ����ȱ�־ */
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

                    /* ������Ϣ */
                    case 0x22:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* ���ݰ����ȱ�־ */
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

                    /* ����˸߶���Ϣ */
                    case 0x31:{
                        p_obj->unpack_buf[index++] = byte;
                        byte = fifo_s_get(&protocol_receive_fifo);
                        /* ���ݰ����ȱ�־ */
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

            /* bccУ�� */
            case BCC_CHECK:{
              // if(byte == check_bcc(p_obj->unpack_buf, index))
                //{
                    p_obj->unpack_buf[index++] = byte;
                    step++;
                //}
               // else step = 0;
            }break;

            /* ��β���� */
            case TAIL_CHECK:{
                byte = fifo_s_get(&protocol_receive_fifo);
                if(byte == 0x6d)
                {
                    p_obj->unpack_buf[index++] = byte;
                    step++;
                }
                else step = 0;
            }break;

            /* ����Ǩ�ƣ�������������е�������Ϣ�����ڽṹ���Ա��ַ�� */
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
 * @brief ��ȡcommand_message�ĵ�ֵַ
 * 
 * @return const fp32* 
 */
const message_n *get_command_message_point(void)
{
    return command_message;
}


/**
 * @brief ��ȡcar_real_message�ĵ�ֵַ
 * 
 * @return const fp32* 
 */
const message_s *get_car_real_message_point(void)
{
    return car_real_message;
}
