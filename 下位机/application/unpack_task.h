/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       unpack_task.c/h
  * @brief      communication task,
  *             协议解包任务
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

/* 解包过程宏 */
#define HEADER_CHECK      0
#define MADDR__MODE       1
#define MESSAGE_ID        2
#define PACKET_MESSAGE    3
#define BCC_CHECK         4
#define TAIL_CHECK        5
#define MESSAGE_OVERWRITE 6


/*  使结构体按照一字节对齐  */
#pragma pack(push, 1)

#ifndef USB_H

#define USB_H

#define HAS_HANDSHAKE 0
#define NOT_HANDSHAKE 1

typedef struct
{
  /* 车体控制信息 */
  int16_t car_command[3];

  /* 击球信息 */
  int16_t shoot_command[2];

  /* 模式信息 */
  uint8_t status_command[2];
}message_n;



typedef struct
{
  /* 实际车速信息 */
  float real_velocity_msg[3];

  /* 轮速信息 */
  int32_t wheel_speed_msg[2];

  /* 持球击球信息 */
  uint16_t ball_msg[2];}message_s;



/**
 * @brief USB解包任务总结构体，上下位机通信用
 * 
 */
typedef struct 
{
  /* 包头 */
  uint16_t header;
  /* 协议地址 */
  uint8_t maddr;
  /* 模式ID */
  uint8_t class_id;
  /* 信息ID */
  uint8_t message_id;
  /* 数据包长度 */
  uint8_t payload_length;
  /* bcc校验 */
  uint32_t check_bcc;
  /* 包尾 */
  uint8_t protocol_tail;

  /* 上位机对下位机的控制信息 */
  const message_n *command_message;

  /* 下位机返回的车体信息 */
  const message_s *car_real_message;
	

  /* 速度信息 */
  int16_t velocity_message[3];
	/* 角度信息(偏航角) */
  int16_t yaw_message;


  /* 配置信息  采取寄存器形式，每一位分别代表某个数据的接发与否  (未实装...) */
  uint32_t config_message[2];

  /* 返回速度设置  (功能开发中...) */
  uint8_t __i_dont_understand;

  /* 发送用的数据包 */
  uint8_t raw_buf[128];

  /* 解包缓存区 */
  uint8_t unpack_buf[128];
} protocol_unpack;


#endif


/*  恢复对齐方式  */
#pragma pack(pop)

extern const message_n *get_command_message_point(void);
extern const message_s *get_car_real_message_point(void);

extern unsigned int check_bcc(unsigned char * data,unsigned int sum);

