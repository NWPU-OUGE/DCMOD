/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       CATCH.c/h
  * @brief      CATCH control task,
  *             ���̿�������
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *  V1.1.0     Nov-11-2019     RM              1. add CATCH power control
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#ifndef CATCH_TASK_H
#define CATCH_TASK_H
#include "struct_typedef.h"
#include "CAN_receive.h"
#include "gimbal_task.h"
#include "pid.h"
#include "remote_control.h"
#include "user_lib.h"
#include "chassis_task.h"
#include "read_angle.h"
#include "car_id.h"

//the channel num of controlling horizontal speed
//���ҵ�ң����ͨ������
#define CATCH_Y_CHANNEL 0

//the channel num of controlling vertial speed 
//ǰ���ң����ͨ������
#define CATCH_X_CHANNEL 1

//in some mode, can use remote control to control rotation speed
//������ģʽ�£�����ͨ��ң����������ת
#define CATCH_WZ_CHANNEL 2

//the channel num of controlling vertial speed 
//���Ͻǵ�ң����ͨ������
#define CATCH_INVOLUTION_CHANNEL 4

//in the beginning of task ,wait a time
//����ʼ����һ��ʱ��
#define CATCH_TASK_INIT_TIME 201

//the channel of choosing catch mode,
//ѡ�����״̬ ����ͨ����
#define CATCH_MODE_CHANNEL 1

//CATCH task control time  2ms
//����������Ƽ�� 2ms
#define CATCH_CONTROL_TIME_MS 2


//m3508 rmp change to CATCH speed,
//m3508ת���ɵ����ٶ�(m/s)�ı�����
#define M3508_MOTOR_RPM_TO_VECTOR 0.000415809748903494517209f
#define CATCH_MOTOR_RPM_TO_VECTOR_SEN M3508_MOTOR_RPM_TO_VECTOR

//single CATCH motor max speed
//�������̵������ٶ�
#define MAX_WHEEL_SPEED 4.0f

//CATCH motor speed PID
//�������ٶȻ�PID
#define M3505_MOTOR_SPEED_PID_KP 15000.0f
#define M3505_MOTOR_SPEED_PID_KI 10.0f
#define M3505_MOTOR_SPEED_PID_KD 0.0f
#define M3505_MOTOR_SPEED_PID_MAX_OUT MAX_MOTOR_CAN_CURRENT
#define M3505_MOTOR_SPEED_PID_MAX_IOUT 2000.0f

//�Ƕȴ������ǶȻ�PID
#define ANGLE_TO_MOTOR_SPEED_PID_KP 0.03f	//0.03	
#define ANGLE_TO_MOTOR_SPEED_PID_KI 0.00f	//0
#define ANGLE_TO_MOTOR_SPEED_PID_KD 0.00015f	//0.00015
#define ANGLE_TO_MOTOR_PID_MAX_OUT 1.9f
#define ANGLE_TO_MOTOR_PID_MAX_IOUT 0.2f

//��ʼ������Ƕ�ʱ�����Ĵ���
#define SAMPLING_TIME 10
//��ȫ�����ʹﵽ�����ǶȵĽǴ�֮��
#define NO_CATCH_TO_AIM_CATCH_OFFSET 50.0f
//����ʱ�������ĽǶȴ�������ֵ411-350 942-1005//420
#ifdef CAR_ONE
#define SET_LEFT_AIM_ANGLE 718.0f    //ԭ��Ӧ����645
#define SET_RIGHT_AIM_ANGLE 125.0f
#endif

#ifdef CAR_TWO
#define SET_LEFT_AIM_ANGLE 342.0f
#define SET_RIGHT_AIM_ANGLE 294.0f
#endif

#define CATCH_OFFSET	30.0f

extern chassis_move_t chassis_move;


typedef enum
{
  CATCH_VECTOR_NO_FOLLOW_YAW,       //CATCH will have rotation speed control. ��������ת�ٶȿ���
  CATCH_VECTOR_FOLLOW_CHASSIS,            //�����������״̬
  CATCH_VECTOR_FOLLOW_ANGLE,              //��������Ƕȴ�����״̬
  CATCH_VECTOR_FOLLOW_CHASSIS_AND_ANGLE,  //����������̺ͽǶȴ�������״̬
  CATCH_VECTOR_FOLLOW_INVOLUTION
} catch_mode_e;

typedef struct
{
  const motor_measure_t *catch_motor_measure;
  fp32 accel;
  fp32 speed;
  fp32 speed_set;
  int16_t give_current;
} catch_motor_t;

typedef struct
{
	fp32 left_vx_param;
	fp32 left_vy_param;
	fp32 left_wz_param;
	fp32 right_vx_param;
	fp32 right_vy_param;
	fp32 right_wz_param;

	fp32 dynamic_param;

	fp32 vx_decrease;
	fp32 vx_increase;

	fp32 vy_decrease;
	fp32 vy_increase;

	fp32 wz_decrease;
	fp32 wz_increase;
}dynamic_catch_param_t;

typedef struct
{
   const chassis_move_t * p_chassis_move;
   const RC_ctrl_t *catch_RC;               //����ʹ�õ�ң����ָ��, the point to remote control
   catch_mode_e catch_mode;               //state machine. �������״̬��
   catch_mode_e last_catch_mode;          //last state machine.�����ϴο���״̬��
   catch_motor_t motor_catch[2];          //CATCH motor data.����������
   pid_type_def motor_speed_pid[2];             //motor speed PID.�������ٶ�pid
   pid_type_def angle_speed_pid[2];              //follow angle PID.���̸���Ƕ�pid

   const angle_read_t * p_angle_read;
   fp32 catch_angle[2];             //�����������Ҳ�ĽǶ�

   fp32 vx;                          //CATCH vertical speed, positive means forward,unit m/s. �����ٶ� ǰ������ ǰΪ������λ m/s
   fp32 vy;                          //CATCH horizontal speed, positive means letf,unit m/s.�����ٶ� ���ҷ��� ��Ϊ��  ��λ m/s
   fp32 wz;                          //CATCH rotation speed, positive means counterclockwise,unit rad/s.������ת���ٶȣ���ʱ��Ϊ�� ��λ rad/s

   //�������õ����̸����������ٶȾ�Ҫֱ�������õ�
   //��ĳЩ����ģʽ�£��������ֹģʽ����ʹ���õ���һ�����ʵĵ����ٶȣ���ҲҪ��������0ȥ��
   fp32 vx_used;
   fp32 vy_used;
   fp32 wz_used; 

   bool_t is_catched;
	 
	 dynamic_catch_param_t dynamic_catch_param;

   fp32 left_aim_angle;
   fp32 right_aim_angle;

} catch_move_t;


/**
  * @brief          CATCH task, osDelay CATCH_CONTROL_TIME_MS (2ms) 
  * @param[in]      pvParameters: null
  * @retval         none
  */
/**
  * @brief          �������񣬼�� CATCH_CONTROL_TIME_MS 2ms
  * @param[in]      pvParameters: ��
  * @retval         none
  */
extern void catch_task(void const *pvParameters);

//����x�᷽����ٶ�
extern fp32 get_vx(void);

//����y�᷽����ٶ�
extern fp32 get_vy(void);

//����z�᷽��Ľ��ٶ�
extern fp32 get_wz(void);

//extern const chassis_move_t *get_remote_chassis_point(void);

#endif
