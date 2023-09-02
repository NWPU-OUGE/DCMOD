/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       CATCH.c/h
  * @brief      CATCH control task,
  *             底盘控制任务
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
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
//左右的遥控器通道号码
#define CATCH_Y_CHANNEL 0

//the channel num of controlling vertial speed 
//前后的遥控器通道号码
#define CATCH_X_CHANNEL 1

//in some mode, can use remote control to control rotation speed
//在特殊模式下，可以通过遥控器控制旋转
#define CATCH_WZ_CHANNEL 2

//the channel num of controlling vertial speed 
//左上角的遥控器通道号码
#define CATCH_INVOLUTION_CHANNEL 4

//in the beginning of task ,wait a time
//任务开始空闲一段时间
#define CATCH_TASK_INIT_TIME 201

//the channel of choosing catch mode,
//选择持球状态 开关通道号
#define CATCH_MODE_CHANNEL 1

//CATCH task control time  2ms
//底盘任务控制间隔 2ms
#define CATCH_CONTROL_TIME_MS 2


//m3508 rmp change to CATCH speed,
//m3508转化成底盘速度(m/s)的比例，
#define M3508_MOTOR_RPM_TO_VECTOR 0.000415809748903494517209f
#define CATCH_MOTOR_RPM_TO_VECTOR_SEN M3508_MOTOR_RPM_TO_VECTOR

//single CATCH motor max speed
//单个底盘电机最大速度
#define MAX_WHEEL_SPEED 4.0f

//CATCH motor speed PID
//持球电机速度环PID
#define M3505_MOTOR_SPEED_PID_KP 15000.0f
#define M3505_MOTOR_SPEED_PID_KI 10.0f
#define M3505_MOTOR_SPEED_PID_KD 0.0f
#define M3505_MOTOR_SPEED_PID_MAX_OUT MAX_MOTOR_CAN_CURRENT
#define M3505_MOTOR_SPEED_PID_MAX_IOUT 2000.0f

//角度传感器角度环PID
#define ANGLE_TO_MOTOR_SPEED_PID_KP 0.03f	//0.03	
#define ANGLE_TO_MOTOR_SPEED_PID_KI 0.00f	//0
#define ANGLE_TO_MOTOR_SPEED_PID_KD 0.00015f	//0.00015
#define ANGLE_TO_MOTOR_PID_MAX_OUT 1.9f
#define ANGLE_TO_MOTOR_PID_MAX_IOUT 0.2f

//初始化持球角度时采样的次数
#define SAMPLING_TIME 10
//完全无力和达到期望角度的角传之差
#define NO_CATCH_TO_AIM_CATCH_OFFSET 50.0f
//持球时，期望的角度传感器的值411-350 942-1005//420
#ifdef CAR_ONE
#define SET_LEFT_AIM_ANGLE 718.0f    //原本应该是645
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
  CATCH_VECTOR_NO_FOLLOW_YAW,       //CATCH will have rotation speed control. 持球有旋转速度控制
  CATCH_VECTOR_FOLLOW_CHASSIS,            //持球会跟随底盘状态
  CATCH_VECTOR_FOLLOW_ANGLE,              //持球会跟随角度传感器状态
  CATCH_VECTOR_FOLLOW_CHASSIS_AND_ANGLE,  //持球会跟随底盘和角度传感器的状态
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
   const RC_ctrl_t *catch_RC;               //持球使用的遥控器指针, the point to remote control
   catch_mode_e catch_mode;               //state machine. 持球控制状态机
   catch_mode_e last_catch_mode;          //last state machine.持球上次控制状态机
   catch_motor_t motor_catch[2];          //CATCH motor data.持球电机数据
   pid_type_def motor_speed_pid[2];             //motor speed PID.持球电机速度pid
   pid_type_def angle_speed_pid[2];              //follow angle PID.底盘跟随角度pid

   const angle_read_t * p_angle_read;
   fp32 catch_angle[2];             //持球电机左侧和右侧的角度

   fp32 vx;                          //CATCH vertical speed, positive means forward,unit m/s. 底盘速度 前进方向 前为正，单位 m/s
   fp32 vy;                          //CATCH horizontal speed, positive means letf,unit m/s.底盘速度 左右方向 左为正  单位 m/s
   fp32 wz;                          //CATCH rotation speed, positive means counterclockwise,unit rad/s.底盘旋转角速度，逆时针为正 单位 rad/s

   //并不是拿到底盘各个分量的速度就要直接拿来用的
   //在某些控制模式下，比如持球静止模式，即使我拿到了一个合适的底盘速度，我也要把它当成0去用
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
  * @brief          底盘任务，间隔 CATCH_CONTROL_TIME_MS 2ms
  * @param[in]      pvParameters: 空
  * @retval         none
  */
extern void catch_task(void const *pvParameters);

//返回x轴方向的速度
extern fp32 get_vx(void);

//返回y轴方向的速度
extern fp32 get_vy(void);

//返回z轴方向的角速度
extern fp32 get_wz(void);

//extern const chassis_move_t *get_remote_chassis_point(void);

#endif
