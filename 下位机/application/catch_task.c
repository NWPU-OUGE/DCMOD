/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       catch.c/h
  * @brief      catch control task,
  *             持球控制任务
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     LuYidan         1. done
  *  V1.1.0     Nov-11-2019     LuYidan         1. add catch power control
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#include "catch_task.h"
#include "chassis_behaviour.h"

#include "cmsis_os.h"

#include "arm_math.h"
#include "pid.h"
#include "remote_control.h"
#include "CAN_receive.h"
#include "detect_task.h"
#include "INS_task.h"
#include "chassis_power_control.h"

#include "catch_behaviour.h"
#include "read_angle.h"
#include "struct_typedef.h"

#define rc_deadband_limit(input, output, dealine)        \
    {                                                    \
        if ((input) > (dealine) || (input) < -(dealine)) \
        {                                                \
            (output) = (input);                          \
        }                                                \
        else                                             \
        {                                                \
            (output) = 0;                                \
        }                                                \
    }

/**
 * @brief           Get the remote chassis point object
 * @param           none
 * @return          const chassis_move_t*
 */
const chassis_move_t *get_remote_chassis_point(void);

/**
 * @brief Get the angle point object
 * @return const angle_read_t*
 */
const angle_read_t * get_angle_point(void);
/**
  * @brief          "catch_move" valiable initialization, include pid initialization, remote control data point initialization, 3508 catch motors
  *                 data point initialization, gimbal motor data point initialization, and gyro sensor angle point initialization.
  * @param[out]     catch_move_init: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          初始化"catch_move"变量，包括pid初始化， 遥控器指针初始化，3508底盘电机指针初始化，云台电机初始化，陀螺仪角度指针初始化
  * @param[out]     catch_move_init:"catch_move"变量指针.
  * @retval         none
  */
static void catch_init(catch_move_t *catch_move_init);

/**
 * @brief                   车辆在不同的运动状态下，仅仅凭借角度环pid是不能可靠持球的
 *                          需要在车辆的不同速度下动态的匹配持球轮的速度
 * @param  catch_move_init  My Param doc
 */
static void init_dynamtic_catch_param(catch_move_t *catch_move_init);

/**
 * @brief  读取adc的值，类型转换后放入持球结构体
 * @param  catch_move_init  持球结构体
 */
static void angle_sampling(catch_move_t *catch_move_init);

/**
 * @brief                   初始化角传的目标值，（可能会由于采样率过低导致初始化时读到的数据一样，或者是角传离线导致读到的值为0，
 *                          你可以在本工程的read_angle.c文件下，设置C板采样的数据；和adc_to_uart工程里，设置adc转串板的发送数据）
 *                          思路就是默认车是不持球的，初始化时 角传读到的值 与 持球时的角传的值 做差，得到目标值
 * @param  catch_move_init  My Param doc
 */
static void init_aim_catch_angle(catch_move_t *catch_move_init);

/**
  * @brief          set catch control mode, mainly call 'catch_behaviour_mode_set' function
  * @param[out]     catch_move_mode: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          设置持球控制模式，主要在'catch_behaviour_mode_set'函数中改变
  * @param[out]     catch_move_mode:"catch_move"变量指针.
  * @retval         none
  */
static void catch_set_mode(catch_move_t *catch_move_mode);

/**
  * @brief          when catch mode change, some param should be changed, suan as catch yaw_set should be now catch yaw
  * @param[out]     catch_move_transit: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          持球模式改变，有些参数需要改变，例如底盘控制yaw角度设定值应该变成当前底盘yaw角度
  * @param[out]     catch_move_transit:"catch_move"变量指针.
  * @retval         none
  */
static void catch_mode_change_control_transit(catch_move_t *catch_move_transit);
/**
  * @brief          catch some measure data updata, such as motor speed, euler angle， robot speed
  * @param[out]     catch_move_update: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          底盘测量数据更新，包括电机速度，欧拉角度，机器人速度
  * @param[out]     catch_move_update:"catch_move"变量指针.
  * @retval         none
  */
static void catch_feedback_update(catch_move_t *catch_move_update);
/**
  * @brief          set catch control set-point, three movement control value is set by "catch_behaviour_control_set".
  *
  * @param[out]     catch_move_update: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief
  * @param[out]     catch_move_update:"catch_move"变量指针.
  * @retval         none
  */
static void catch_set_contorl(catch_move_t *catch_move_control);
/**
  * @brief          control loop, according to control set-point, calculate motor current,
  *                 motor current will be sentto motor
  * @param[out]     catch_move_control_loop: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          控制循环，根据控制设定值，计算电机电流值，进行控制
  * @param[out]     catch_move_control_loop:"catch_move"变量指针.
  * @retval         none
  */
static void catch_control_loop(catch_move_t *catch_move_control_loop);
/*=======================================================================================*/
/*=======================================================================================*/
/*============================以下是在测试过程中的各类持球模式=============================*/
/*=======================================================================================*/
/*=======================================================================================*/
//lyd add here
/**
 * @brief 
 * @param  catch_angle      My Param doc
 * @param  catch_wheel_speedMy Param doc
 * @param  angle_speed_pid  My Param doc
 * @param  catch_state      My Param doc
 * @param  involution       My Param doc
 */
static void multi_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2],
                                    pid_type_def *angle_speed_pid, catch_move_t *catch_state,                        
                                    int16_t involution);
/**
 * @brief   持球电机，不动！！！
 * @param   catch_wheel_speed  My Param doc
 */
static void catch_wheel_no_move(fp32 catch_wheel_speed[2]);
/*=======================================================================================*/
/*=======================================================================================*/
/*============================以下是在测试过程中的各类持球模式=============================*/
/*=======================================================================================*/
/*=======================================================================================*/
/**
 * @brief  在这里，我希望可以通过底盘的速度解算出持球电机的速度，底盘的速度大概不会超过3米每秒
 * @param  vx_set             底盘x轴分量的速度
 * @param  vy_set             底盘y轴分量的速度
 * @param  wz_set             底盘z轴分量的速度
 * @param  catch_wheel_speed  持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution         内卷分量
 */
static void chassis_vector_to_catch_wheel_speed(const fp32 vx_used, const fp32 vy_used, const fp32 wz_used,
        fp32 catch_wheel_speed[2], int16_t involution );

/**
 * @brief                           通过角度传感器的值决定持球电机的转速，特殊模式
 * @param  catch_angle              左右侧持球电机角度传感器的角度值[0:1]代表左右
 * @param  catch_wheel_speed        持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution               内卷分量
 * @param  angle_speed_pid          角度传感器确认的pid
 */
static void angle_vector_to_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2], int16_t involution,
        pid_type_def *angle_speed_pid );


                                    

/**
 * @brief                     更据遥控器手动施加持续性内卷风量，特殊模式
 * @param  catch_wheel_speed  持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution         内卷分量
 */
static void rc_involution_to_catch_wheel_speed(fp32 catch_wheel_speed[2], int16_t involution);

/**
 * @brief                       慢速持续内卷，特殊模式
 * @param  catch_wheel_speedMy  持球电机的的期望速度，[0:1]代表左右电机
 * @param  catch_state          适当更据车辆速度改变持球电机速度
 */
static void catch_tmp_move(fp32 catch_wheel_speed[2],catch_move_t *catch_state);
#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t catch_high_water;
#endif



//底盘运动数据
extern chassis_move_t chassis_move;
//持球角度数据
extern angle_read_t  angle_read;
//持球运动数据
catch_move_t catch_move;


/**
  * @brief          catch task, osDelay CATCH_CONTROL_TIME_MS (2ms)
  * @param[in]      pvParameters: null
  * @retval         none
  */
/**
  * @brief          持球任务，间隔 CATCH_CONTROL_TIME_MS 2ms
  * @param[in]      pvParameters: 空
  * @retval         none
  */
void catch_task(void const *pvParameters)
{
    //wait a time
    //空闲一段时间
    vTaskDelay(CATCH_TASK_INIT_TIME);
    //catch init
    //持球初始化
    catch_init(&catch_move);
    //make sure all catch motor is online,
    //判断持球电机是否都在线
    while (toe_is_error(YAW_GIMBAL_MOTOR_TOE) || toe_is_error(PITCH_GIMBAL_MOTOR_TOE) || toe_is_error(DBUS_TOE))
    {
        vTaskDelay(CATCH_CONTROL_TIME_MS);
    }

    while (1)
    {
        //vTaskSuspendAll();
        //set catch control mode
        //设置持球控制模式
        catch_set_mode(&catch_move);
        //when mode changes, some data save
        //模式切换数据保存，不过其实对于持球来说，没有什么数据是需要重新校准的
        catch_mode_change_control_transit(&catch_move);
        //catch data update
        //持球数据更新
        catch_feedback_update(&catch_move);
        //set catch control set-point
        //持球控制量设置
        catch_set_contorl(&catch_move);
        //catch control pid calculate
        //持球控制PID计算
        catch_control_loop(&catch_move);

        //make sure  one motor is online at least, so that the control CAN message can be received
        //确保至少一个电机在线， 这样CAN控制包可以被接收到
        if (!(toe_is_error(YAW_GIMBAL_MOTOR_TOE) && toe_is_error(PITCH_GIMBAL_MOTOR_TOE)))
        {
            //when remote control is offline, catch motor should receive zero current.
            //当遥控器掉线的时候，发送给持球电机零电流.
            if (toe_is_error(DBUS_TOE))
            {
                CAN_cmd_catch(0, 0);
            }
            else
            {
                //send control current
                //发送控制电流
                CAN_cmd_catch(catch_move.motor_catch[0].give_current, catch_move.motor_catch[1].give_current);
            }
        }
        //os delay
        //系统延时
        //xTaskResumeAll();
        vTaskDelay(CATCH_CONTROL_TIME_MS);

#if INCLUDE_uxTaskGetStackHighWaterMark
        catch_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}

/**
 * @brief Get the remote chassis point object
 * @return const chassis_move_t*
 */
const chassis_move_t *get_remote_chassis_point(void)
{
    return &chassis_move;
}

/**
 * @brief Get the angle point object
 * @return const angle_read_t*
 */
const angle_read_t * get_angle_point(void)
{
    return &angle_read;
}


/**
  * @brief          "catch_move" valiable initialization, include pid initialization, remote control data point initialization, 3508 catch motors
  *                 data point initialization, gimbal motor data point initialization, and gyro sensor angle point initialization.
  * @param[out]     catch_move_init: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          初始化"catch_move"变量，包括pid初始化， 遥控器指针初始化，3508底盘电机指针初始化，云台电机初始化，陀螺仪角度指针初始化
  * @param[out]     catch_move_init:"catch_move"变量指针.
  * @retval         none
  */

static void catch_init(catch_move_t *catch_move_init)
{
    if (catch_move_init == NULL)
    {
        return;
    }

    catch_move_init->catch_RC = get_remote_control_point();
    catch_move_init->p_chassis_move = get_remote_chassis_point();
    catch_move_init->p_angle_read = get_angle_point();

    //catch motor speed PID
    //持球速度环pid值
    const static fp32 motor_speed_pid[3] = {M3505_MOTOR_SPEED_PID_KP, M3505_MOTOR_SPEED_PID_KI, M3505_MOTOR_SPEED_PID_KD};
    //持球角度环pid值,由角度传感器的值确定电机的速度
    const static fp32 angle_speed_pid[3] = {ANGLE_TO_MOTOR_SPEED_PID_KP, ANGLE_TO_MOTOR_SPEED_PID_KI, ANGLE_TO_MOTOR_SPEED_PID_KD};

    catch_move_init->vx=get_vx();
    catch_move_init->vy=get_vy();
    catch_move_init->wz=get_wz();


    uint8_t i;
    //get catch motor data point,  initialize motor speed PID
    //获取持球电机数据指针，初始化PID
    for (i = 0; i < 2; i++)
    {
        catch_move_init->motor_catch[i].catch_motor_measure = get_left_right_motor_measure_point(i);
        PID_init(&catch_move_init->motor_speed_pid[i], PID_POSITION, motor_speed_pid, M3505_MOTOR_SPEED_PID_MAX_OUT, M3505_MOTOR_SPEED_PID_MAX_IOUT);
        PID_init(&catch_move_init->angle_speed_pid[i], PID_POSITION, angle_speed_pid, ANGLE_TO_MOTOR_PID_MAX_OUT, ANGLE_TO_MOTOR_PID_MAX_IOUT);
    }

    init_dynamtic_catch_param(catch_move_init);

    init_aim_catch_angle(catch_move_init);
}

/**
 * @brief                   车辆在不同的运动状态下，仅仅凭借角度环pid是不能可靠持球的
 *                          需要在车辆的不同速度下动态的匹配持球轮的速度
 * @param  catch_move_init  My Param doc
 */
static void init_dynamtic_catch_param(catch_move_t *catch_move_init)
{
    catch_move_init->dynamic_catch_param.dynamic_param=10;
    catch_move_init->dynamic_catch_param.left_vx_param=2.0;
    catch_move_init->dynamic_catch_param.left_vy_param=1.0;
    catch_move_init->dynamic_catch_param.left_wz_param=0.6;
    catch_move_init->dynamic_catch_param.right_vx_param=2.0;
    catch_move_init->dynamic_catch_param.right_vy_param=1.0;
    catch_move_init->dynamic_catch_param.right_wz_param=0.6;
    catch_move_init->dynamic_catch_param.vx_decrease=0.5;
    catch_move_init->dynamic_catch_param.vx_increase=2.0;
    catch_move_init->dynamic_catch_param.vy_decrease=1.0;
    catch_move_init->dynamic_catch_param.vy_increase=1.0;
    catch_move_init->dynamic_catch_param.wz_decrease=0.6;
    catch_move_init->dynamic_catch_param.wz_increase=0.6;
}

/**
 * @brief  读取adc的值，类型转换后放入持球结构体
 * @param  catch_move_init  持球结构体
 */
static void angle_sampling(catch_move_t *catch_move_init)
{
    if(catch_move_init == NULL)
    {
        return;
    }
    if(catch_move_init->p_angle_read == NULL)
    {
        return;
    }
    uint32_t catch_angle[2]= {0,0};
    //将采样到的uint16_t转换为uint32_t
    catch_angle[0] = catch_move_init->p_angle_read->adc1;
    catch_angle[1] = catch_move_init->p_angle_read->adc2;
    //将uint32_t转换为fp32，存入结构体
    catch_move_init->catch_angle[0] = 1.0 * catch_angle[0];
    catch_move_init->catch_angle[1] = 1.0 * catch_angle[1];
}

/**
 * @brief                   初始化角传的目标值，（可能会由于采样率过低导致初始化时读到的数据一样，或者是角传离线导致读到的值为0，
 *                          你可以在本工程的read_angle.c文件下，设置C板采样的数据；和adc_to_uart工程里，设置adc转串板的发送数据）
 *                          思路就是默认车是不持球的，初始化时 角传读到的值 与 持球时的角传的值 做差，得到目标值
 * @param  catch_move_init  My Param doc
 */
static void init_aim_catch_angle(catch_move_t *catch_move_init)
{
	// int i=0;
    // for(i=0;i<SAMPLING_TIME;i++)
    // {
    //     angle_sampling(catch_move_init);
    //     catch_move_init->left_aim_angle += catch_move_init->catch_angle[0];
    //     catch_move_init->right_aim_angle += catch_move_init->catch_angle[1];
    //     osDelay(10);
    // }
    // catch_move_init->left_aim_angle = catch_move_init->left_aim_angle/SAMPLING_TIME;
    // catch_move_init->right_aim_angle = catch_move_init->right_aim_angle/SAMPLING_TIME;
    
    // catch_move_init->left_aim_angle = catch_move_init->left_aim_angle - NO_CATCH_TO_AIM_CATCH_OFFSET;
    // catch_move_init->right_aim_angle = catch_move_init->right_aim_angle + NO_CATCH_TO_AIM_CATCH_OFFSET;
		//由于read_angle.c 的问题，暂时这么操作
		catch_move_init->left_aim_angle = SET_LEFT_AIM_ANGLE;
    catch_move_init->right_aim_angle = SET_RIGHT_AIM_ANGLE;
}

/**
  * @brief          set catch control mode, mainly call 'catchs_behaviour_mode_set' function
  * @param[out]     catch_move_mode: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          设置持球控制模式，主要在'catch_behaviour_mode_set'函数中改变
  * @param[out]     catch_move_mode:"catch_move"变量指针.
  * @retval         none
  */
static void catch_set_mode(catch_move_t *catch_move_mode)
{
    if (catch_move_mode == NULL)
    {
        return;
    }
    //in file "catch_behaviour.c"
    catch_behaviour_mode_set(catch_move_mode);
}

/**
  * @brief          when catch mode change, some param should be changed, suan as catch yaw_set should be now catch yaw
  * @param[out]     catch_move_transit: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          持球模式改变，有些参数需要改变，例如底盘控制yaw角度设定值应该变成当前底盘yaw角度
  * @param[out]     catch_move_transit:"catch_move"变量指针.
  * @retval         none
  */
static void catch_mode_change_control_transit(catch_move_t *catch_move_transit)
{
    if (catch_move_transit == NULL)
    {
        return;
    }

    if (catch_move_transit->last_catch_mode == catch_move_transit->catch_mode)
    {
        return;
    }
    catch_move_transit->last_catch_mode = catch_move_transit->catch_mode;
}


/**
 * @brief  持球状态反馈
 * @param  catch_move_updateMy Param doc
 */
static void catch_feedback_update(catch_move_t *catch_move_update)
{
    if (catch_move_update == NULL)
    {
        return;
    }
    //更新底盘的运动状态
    catch_move_update->vx=get_vx();
    catch_move_update->vy=get_vy();
    catch_move_update->wz=get_wz();

    //更新持球的角度值   
    angle_sampling(catch_move_update);

    //判断是否持球
    // if(catch_angle[0]<SET_LEFT_AIM_ANGLE+CATCH_OFFSET && catch_angle[1]>SET_RIGHT_AIM_ANGLE-CATCH_OFFSET)
    if(catch_move_update->catch_angle[0]<catch_move_update->left_aim_angle + CATCH_OFFSET &&
       catch_move_update->catch_angle[1]>catch_move_update->right_aim_angle - CATCH_OFFSET)
    {
        catch_move_update->is_catched = 1;
    } else {
        catch_move_update->is_catched = 0;
    }

    uint8_t i = 0;
    for (i = 0; i < 2; i++)
    {
        //update motor speed, accel is differential of speed PID
        //更新持球电机速度，加速度是速度的PID微分
        catch_move_update->motor_catch[i].speed = CHASSIS_MOTOR_RPM_TO_VECTOR_SEN * catch_move_update->motor_catch[i].catch_motor_measure->speed_rpm;
        catch_move_update->motor_catch[i].accel = catch_move_update->motor_speed_pid[i].Dbuf[0] * CHASSIS_CONTROL_FREQUENCE;
    }
}

/**
  * @brief          set catch control set-point, three movement control value is set by "catch_behaviour_control_set".
  * @param[out]     catch_move_update: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          设置持球控制设置值, 更据不同的底盘控制模式设置不同的所需的底盘速度分量
  * @param[out]     catch_move_update:"catch_move"变量指针.
  * @retval         none
  */
static void catch_set_contorl(catch_move_t *catch_move_control)
{

    if (catch_move_control == NULL)
    {
        return;
    }

    if(catch_move_control->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS ||
            catch_move_control->catch_mode == CATCH_VECTOR_FOLLOW_ANGLE   ||
            catch_move_control->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS_AND_ANGLE)
    {
        catch_move_control->vx_used = catch_move_control->vx;
        catch_move_control->vy_used = catch_move_control->vy;
        catch_move_control->wz_used = catch_move_control->wz;
    }
    else if(catch_move_control->catch_mode == CATCH_VECTOR_NO_FOLLOW_YAW)
    {
        catch_move_control->vx_used = 0.0f;
        catch_move_control->vy_used = 0.0f;
        catch_move_control->wz_used = 0.0f;
    }
}

static void catch_control_loop(catch_move_t *catch_move_control_loop)
{
    fp32 max_vector = 0.0f, vector_rate = 0.0f;
    fp32 temp = 0.0f;
    fp32 catch_wheel_speed[2] = {0.0f, 0.0f};
    uint8_t i = 0;


    //更据底盘的速度解算出持球的速度
    //CATCH_VECTOR_NO_FOLLOW_YAW模式下， 我需要的底盘速度全部为0
    if(catch_move_control_loop->catch_mode == CATCH_VECTOR_NO_FOLLOW_YAW)
    {
        catch_wheel_no_move(catch_wheel_speed);
        //chassis_vector_to_catch_wheel_speed(0, 0, 0, catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //CATCH_VECTOR_FOLLOW_CHASSIS模式下，我需要的底盘的速度是真实的底盘速度，
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS)
    {
        chassis_vector_to_catch_wheel_speed(catch_move_control_loop->vx_used, catch_move_control_loop->vy_used,
                                            catch_move_control_loop->wz_used, catch_wheel_speed,
                                            catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //跟随角度传感器状态解算出持球的速度
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_ANGLE)
    {
        angle_vector_to_catch_wheel_speed(catch_move_control_loop->catch_angle,
                                          catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL],
                                          catch_move_control_loop->angle_speed_pid);
    }
    //跟随底盘和角度传感器的状态解算出持球的速度
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS_AND_ANGLE)
    {
			  //catch_tmp_move(catch_wheel_speed,catch_move_control_loop);
			  //catch_no_move(catch_wheel_speed);
       multi_catch_wheel_speed(catch_move_control_loop->catch_angle, catch_wheel_speed,
                               catch_move_control_loop->angle_speed_pid,catch_move_control_loop,
                               catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //更据遥控器手动施加持续性内卷分量
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_INVOLUTION)
    {
        rc_involution_to_catch_wheel_speed(catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //calculate the max speed in four wheels, limit the max speed
    //计算轮子控制最大速度，并限制其最大速度
    for (i = 0; i < 2; i++)
    {
        catch_move_control_loop->motor_catch[i].speed_set = catch_wheel_speed[i];
        temp = fabs(catch_move_control_loop->motor_catch[i].speed_set);
        if (max_vector < temp)
        {
            max_vector = temp;
        }
    }

    if (max_vector > MAX_WHEEL_SPEED)
    {
        vector_rate = MAX_WHEEL_SPEED / max_vector;
        for (i = 0; i < 2; i++)
        {
            catch_move_control_loop->motor_catch[i].speed_set *= vector_rate;
        }
    }

    //calculate pid
    //计算pid
    for (i = 0; i < 2; i++)
    {
        PID_calc(&catch_move_control_loop->motor_speed_pid[i], catch_move_control_loop->motor_catch[i].speed, catch_move_control_loop->motor_catch[i].speed_set);
    }


    //功率控制
    //chassis_power_control(chassis_move_control_loop);

    //赋值电流值
    for (i = 0; i < 2; i++)
    {
        catch_move_control_loop->motor_catch[i].give_current = (int16_t)(catch_move_control_loop->motor_speed_pid[i].out);
    }

}
/**
 * @brief   catch_wheel_no_move
 * @param   catch_wheel_speed  My Param doc
 */
static void catch_wheel_no_move(fp32 catch_wheel_speed[2])
{
    catch_wheel_speed[0] = 0;
    catch_wheel_speed[1] = 0;
}
/**
 * @brief  在这里，我希望可以通过底盘的速度解算出持球电机的速度，底盘的速度大概不会超过3米每秒
 * @param  vx_set             底盘x轴分量的速度
 * @param  vy_set             底盘y轴分量的速度
 * @param  wz_set             底盘z轴分量的速度
 * @param  catch_wheel_speed  持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution         内卷分量
 */
static void chassis_vector_to_catch_wheel_speed(const fp32 vx_used, const fp32 vy_used, const fp32 wz_used,
        fp32 catch_wheel_speed[2], int16_t involution)
{
    fp32			SQRT2_2  	=	0.7071;
    fp32 			R 				=	0.165;

    //前进时，为了保证球不被推出去，我需要电机慢一点
    //后退时，我希望可以稳固的带球，我需要电机快一点
    //这两个参数代表我希望速度变化的比例
    fp32 increase_param = 1.8;
    fp32 decrease_param = 0.5;
    //补偿参数，补偿电机转速过慢导致的速度损失
    fp32 compensate_param = 1.2;
    //在这里vparam代表电机的转速，它是一个标量
    fp32 left_vx_param = 3;
    fp32 left_vy_param = 4.8;
    fp32 left_wz_param = 6;
    fp32 right_vx_param = 3;
    fp32 right_vy_param = 4.8;
    fp32 right_wz_param = 6;


    //遥控器直接过来的数据范围为[-660,660]，这个速度对持球来说太大
    fp32 involution_param =0.01*involution;
    //x轴速度分量判断
    if(vx_used > 0) {
        left_vx_param = left_vx_param * decrease_param;
        right_vx_param = right_vx_param * decrease_param;
    }
    else if(vx_used < 0) {
        left_vx_param = left_vx_param * increase_param;
        right_vx_param = right_vx_param * increase_param;
    }

    //y轴速度分量判断
    if(vy_used > 0) {
        left_vy_param = left_vy_param * increase_param;
        right_vy_param = right_vy_param * decrease_param;
    }
    else if(vy_used < 0) {
        left_vy_param = left_vy_param * decrease_param;
        right_vy_param = right_vy_param * increase_param;
    }

    //w轴速度分量判断
    if(wz_used > 0) {
        left_wz_param = left_wz_param * increase_param;
        right_wz_param = -right_wz_param * decrease_param *compensate_param;
    }
    else if(wz_used < 0) {
        left_wz_param = - left_wz_param * decrease_param * compensate_param;
        right_wz_param = right_wz_param * increase_param;
    }

    catch_wheel_speed[0] = (SQRT2_2)*left_vx_param*vx_used - (SQRT2_2)*left_vy_param*vy_used - R*left_wz_param*wz_used - involution_param;
    catch_wheel_speed[1] = -(SQRT2_2)*right_vx_param*vx_used - (SQRT2_2)*right_vy_param*vy_used - R*right_wz_param*wz_used + involution_param;
}


/**
 * @brief                           通过角度传感器的值决定持球电机的转速
 * @param  catch_angle              左右侧持球电机角度传感器的角度值[0:1]代表左右
 * @param  catch_wheel_speed        持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution               内卷分量
 * @param  angle_speed_pid          角度传感器确认的pid
 */
fp32 out[2] = {0,0};
static void angle_vector_to_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2], int16_t involution,
        pid_type_def *angle_speed_pid )
{

    out[0] = PID_calc(angle_speed_pid, catch_angle[0], SET_LEFT_AIM_ANGLE);
    out[1] = PID_calc(angle_speed_pid, catch_angle[1], SET_RIGHT_AIM_ANGLE);

    //遥控器直接过来的数据范围为[-660,660]，这个速度对持球来说太大
    fp32 involution_param =0.01*involution;
    catch_wheel_speed[0] = out[0] - involution_param;
    catch_wheel_speed[1] = out[1] + involution_param;
}





/**
 * @brief                   
 * @param  catch_angle          角度传感器的数值，[0:1]代表左右角传
 * @param  catch_wheel_speed    持球电机的的期望速度，[0:1]代表左右电机
 * @param  angle_speed_pid      角度传感器PID
 * @param  catch_state          持球状态，即是否持球
 * @param  involution           内卷分量
 */
static void multi_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2],
                                    pid_type_def *angle_speed_pid, catch_move_t *catch_state,                        
                                    int16_t involution)
{
    if(catch_state == NULL)
    {
        return;
    }
    static fp32 angle_catch_out[2] = {0,0};
    static fp32 chassis_catch_out[2] = {0,0};
    dynamic_catch_param_t dynamic_catch_param = catch_state->dynamic_catch_param;
    if(catch_state->is_catched)
    {
        if(catch_state->vx > 0)//前进
        {
            dynamic_catch_param.left_vx_param = dynamic_catch_param.vx_decrease;
            dynamic_catch_param.right_vx_param = dynamic_catch_param.vx_decrease;
        }
        else if(catch_state->vx < 0)//后退
        {
            dynamic_catch_param.left_vx_param = dynamic_catch_param.vx_increase;
            dynamic_catch_param.right_vx_param = dynamic_catch_param.vx_increase;
        }
        if(catch_state->vy > 0)//右平移
        {
            dynamic_catch_param.left_vy_param = dynamic_catch_param.vy_decrease;
            dynamic_catch_param.right_vy_param = dynamic_catch_param.vy_increase;
        }
        else if(catch_state->vy < 0)//左平移
        {
            dynamic_catch_param.left_vy_param = dynamic_catch_param.vy_increase;
            dynamic_catch_param.right_vy_param = dynamic_catch_param.vy_decrease;
        }
        if(catch_state->wz > 0)//右旋转
        {
            dynamic_catch_param.left_wz_param = dynamic_catch_param.wz_decrease;
            dynamic_catch_param.right_wz_param = dynamic_catch_param.wz_increase;
        }
        else if(catch_state->wz < 0)//左旋转
        {
            dynamic_catch_param.left_wz_param = dynamic_catch_param.wz_increase;
            dynamic_catch_param.right_wz_param = dynamic_catch_param.wz_decrease;
        }
        angle_catch_out[0] = PID_calc(angle_speed_pid, catch_angle[0], catch_state->left_aim_angle + dynamic_catch_param.dynamic_param*catch_state->vx);
        angle_catch_out[1] = PID_calc(angle_speed_pid, catch_angle[1], catch_state->right_aim_angle -dynamic_catch_param.dynamic_param*catch_state->vx);

        chassis_catch_out[0] = dynamic_catch_param.left_vx_param*catch_state->vx - dynamic_catch_param.left_vy_param*catch_state->vy - dynamic_catch_param.left_wz_param*catch_state->wz;
        chassis_catch_out[1] = -dynamic_catch_param.right_vx_param*catch_state->vx - dynamic_catch_param.right_vy_param*catch_state->vy - dynamic_catch_param.right_wz_param*catch_state->wz;
    }
    else if (!catch_state->is_catched)
    {
        angle_catch_out[0] = PID_calc(angle_speed_pid, catch_angle[0], catch_state->left_aim_angle);
        angle_catch_out[1] = PID_calc(angle_speed_pid, catch_angle[1], catch_state->right_aim_angle);
        chassis_catch_out[0] = 0;
        chassis_catch_out[1] = 0;
    }

    out[0] = angle_catch_out[0]+chassis_catch_out[0];
    out[1] = angle_catch_out[1]+chassis_catch_out[1];

    //遥控器直接过来的数据范围为[-660,660]，这个速度对持球来说太大
		//如果需要通过遥控器的值手动控制持球轮的转速，请修改下列注释，但是请注意！遥控器此通道同时控制电容的期望电压！
		//fp32 involution_param =0.01*involution;
		fp32 involution_param = 0;
    catch_wheel_speed[0] = out[0] - involution_param;
    catch_wheel_speed[1] = out[1] + involution_param;
}


/**
 * @brief                     更据遥控器手动施加持续性内卷风量
 * @param  catch_wheel_speed  持球电机的的期望速度，[0:1]代表左右电机
 * @param  involution         内卷分量
 */
static void rc_involution_to_catch_wheel_speed(fp32 catch_wheel_speed[2], int16_t involution)
{
    static fp32 involution_param = 0;
    involution_param += 0.0001*involution;
    catch_wheel_speed[0] = -involution_param;
    catch_wheel_speed[1] = involution_param;
}
/**
 * @brief                       慢速持续内卷，特殊模式
 * @param  catch_wheel_speedMy  持球电机的的期望速度，[0:1]代表左右电机
 * @param  catch_state          适当更据车辆速度改变持球电机速度
 */
static void catch_tmp_move(fp32 catch_wheel_speed[2],catch_move_t *catch_state)
{
    catch_wheel_speed[0] = -0.5 + catch_state->vx;
    catch_wheel_speed[1] = 0.5 - catch_state->vx;
	
}

/**
 * @brief   Get the vx object, 返回x轴方向的速度
 * @return fp32
 */
static fp32 get_vx()
{
    return  chassis_move.vx;
}
/**
 * @brief Get the vy object, 返回y轴方向的速度
 * @return fp32
 */
static fp32 get_vy()
{
    return  chassis_move.vy;
}
/**
 * @brief Get the wz object, 返回z轴方向的速度
 * @return fp32
 */
static fp32 get_wz()
{
    return  chassis_move.wz;
}

