/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       unpack_task.c/h
  * @brief      communication task,
  *             Э��������
  * @note
  * @history
  *  Version    Date            Author					        Modification
  *  V1.0.0     Apr-16-2021     Yang-Zhengzhong         1. unfinished
  *  V1.0.0     Apr-18-2021     Yang-Zhengzhong         1. finish the frame
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  */


#include "cmsis_os.h"
#include "usb_task.h"
#include "fifo.h"
#include "protocol.h"
#include "protocol_send_task.h"

#define UNHANDSHAKE 1
#define HANDSHAKE   0

extern void unpack_task(void const * argument);
extern volatile uint8_t handshake_flag;

/* ������̺� */
#define HEADER_CHECK      0
#define MADDR__MODE       1
#define MESSAGE_ID        2
#define PACKET_MESSAGE    3
#define BCC_CHECK         4
#define TAIL_CHECK        5
#define MESSAGE_OVERWRITE 6


/*  ʹ�ṹ�尴��һ�ֽڶ���  */
#pragma pack(push, 1)

#ifndef USB_H

#define USB_H

#define HAS_HANDSHAKE 0
#define NOT_HANDSHAKE 1

typedef struct
{
  /* ���������Ϣ */
  int16_t car_command[3];

  /* ������Ϣ */
  int16_t shoot_command[2];

  /* ģʽ��Ϣ */
  uint8_t status_command[2];
}message_n;



typedef struct
{
  /* ʵ�ʳ�����Ϣ */
  float real_velocity_msg[3];

  /* ������Ϣ */
  int32_t wheel_speed_msg[2];

  /* ���������Ϣ */
  uint16_t ball_msg[2];}message_s;



/**
 * @brief USB��������ܽṹ�壬����λ��ͨ����
 * 
 */
typedef struct 
{
  /* ��ͷ */
  uint16_t header;
  /* Э���ַ */
  uint8_t maddr;
  /* ģʽID */
  uint8_t class_id;
  /* ��ϢID */
  uint8_t message_id;
  /* ���ݰ����� */
  uint8_t payload_length;
  /* bccУ�� */
  uint32_t check_bcc;
  /* ��β */
  uint8_t protocol_tail;

  /* ��λ������λ���Ŀ�����Ϣ */
  const message_n *command_message;

  /* ��λ�����صĳ�����Ϣ */
  const message_s *car_real_message;
	

  /* �ٶ���Ϣ */
  int16_t velocity_message[3];
	/* �Ƕ���Ϣ(ƫ����) */
  int16_t yaw_message;


  /* ������Ϣ  ��ȡ�Ĵ�����ʽ��ÿһλ�ֱ����ĳ�����ݵĽӷ����  (δʵװ...) */
  uint32_t config_message[2];

  /* �����ٶ�����  (���ܿ�����...) */
  uint8_t __i_dont_understand;

  /* �����õ����ݰ� */
  uint8_t raw_buf[128];

  /* ��������� */
  uint8_t unpack_buf[128];
} protocol_unpack;


#endif


/*  �ָ����뷽ʽ  */
#pragma pack(pop)

extern const message_n *get_command_message_point(void);
extern const message_s *get_car_real_message_point(void);

extern unsigned int check_bcc(unsigned char * data,unsigned int sum);

