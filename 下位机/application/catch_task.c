/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       catch.c/h
  * @brief      catch control task,
  *             �����������
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
  * @brief          ��ʼ��"catch_move"����������pid��ʼ���� ң����ָ���ʼ����3508���̵��ָ���ʼ������̨�����ʼ���������ǽǶ�ָ���ʼ��
  * @param[out]     catch_move_init:"catch_move"����ָ��.
  * @retval         none
  */
static void catch_init(catch_move_t *catch_move_init);

/**
 * @brief                   �����ڲ�ͬ���˶�״̬�£�����ƾ��ǶȻ�pid�ǲ��ܿɿ������
 *                          ��Ҫ�ڳ����Ĳ�ͬ�ٶ��¶�̬��ƥ������ֵ��ٶ�
 * @param  catch_move_init  My Param doc
 */
static void init_dynamtic_catch_param(catch_move_t *catch_move_init);

/**
 * @brief  ��ȡadc��ֵ������ת����������ṹ��
 * @param  catch_move_init  ����ṹ��
 */
static void angle_sampling(catch_move_t *catch_move_init);

/**
 * @brief                   ��ʼ���Ǵ���Ŀ��ֵ�������ܻ����ڲ����ʹ��͵��³�ʼ��ʱ����������һ���������ǽǴ����ߵ��¶�����ֵΪ0��
 *                          ������ڱ����̵�read_angle.c�ļ��£�����C����������ݣ���adc_to_uart���������adcת����ķ������ݣ�
 *                          ˼·����Ĭ�ϳ��ǲ�����ģ���ʼ��ʱ �Ǵ�������ֵ �� ����ʱ�ĽǴ���ֵ ����õ�Ŀ��ֵ
 * @param  catch_move_init  My Param doc
 */
static void init_aim_catch_angle(catch_move_t *catch_move_init);

/**
  * @brief          set catch control mode, mainly call 'catch_behaviour_mode_set' function
  * @param[out]     catch_move_mode: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          ���ó������ģʽ����Ҫ��'catch_behaviour_mode_set'�����иı�
  * @param[out]     catch_move_mode:"catch_move"����ָ��.
  * @retval         none
  */
static void catch_set_mode(catch_move_t *catch_move_mode);

/**
  * @brief          when catch mode change, some param should be changed, suan as catch yaw_set should be now catch yaw
  * @param[out]     catch_move_transit: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          ����ģʽ�ı䣬��Щ������Ҫ�ı䣬������̿���yaw�Ƕ��趨ֵӦ�ñ�ɵ�ǰ����yaw�Ƕ�
  * @param[out]     catch_move_transit:"catch_move"����ָ��.
  * @retval         none
  */
static void catch_mode_change_control_transit(catch_move_t *catch_move_transit);
/**
  * @brief          catch some measure data updata, such as motor speed, euler angle�� robot speed
  * @param[out]     catch_move_update: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          ���̲������ݸ��£���������ٶȣ�ŷ���Ƕȣ��������ٶ�
  * @param[out]     catch_move_update:"catch_move"����ָ��.
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
  * @param[out]     catch_move_update:"catch_move"����ָ��.
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
  * @brief          ����ѭ�������ݿ����趨ֵ������������ֵ�����п���
  * @param[out]     catch_move_control_loop:"catch_move"����ָ��.
  * @retval         none
  */
static void catch_control_loop(catch_move_t *catch_move_control_loop);
/*=======================================================================================*/
/*=======================================================================================*/
/*============================�������ڲ��Թ����еĸ������ģʽ=============================*/
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
 * @brief   ������������������
 * @param   catch_wheel_speed  My Param doc
 */
static void catch_wheel_no_move(fp32 catch_wheel_speed[2]);
/*=======================================================================================*/
/*=======================================================================================*/
/*============================�������ڲ��Թ����еĸ������ģʽ=============================*/
/*=======================================================================================*/
/*=======================================================================================*/
/**
 * @brief  �������ϣ������ͨ�����̵��ٶȽ�������������ٶȣ����̵��ٶȴ�Ų��ᳬ��3��ÿ��
 * @param  vx_set             ����x��������ٶ�
 * @param  vy_set             ����y��������ٶ�
 * @param  wz_set             ����z��������ٶ�
 * @param  catch_wheel_speed  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution         �ھ����
 */
static void chassis_vector_to_catch_wheel_speed(const fp32 vx_used, const fp32 vy_used, const fp32 wz_used,
        fp32 catch_wheel_speed[2], int16_t involution );

/**
 * @brief                           ͨ���Ƕȴ�������ֵ������������ת�٣�����ģʽ
 * @param  catch_angle              ���Ҳ�������Ƕȴ������ĽǶ�ֵ[0:1]��������
 * @param  catch_wheel_speed        �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution               �ھ����
 * @param  angle_speed_pid          �Ƕȴ�����ȷ�ϵ�pid
 */
static void angle_vector_to_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2], int16_t involution,
        pid_type_def *angle_speed_pid );


                                    

/**
 * @brief                     ����ң�����ֶ�ʩ�ӳ������ھ����������ģʽ
 * @param  catch_wheel_speed  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution         �ھ����
 */
static void rc_involution_to_catch_wheel_speed(fp32 catch_wheel_speed[2], int16_t involution);

/**
 * @brief                       ���ٳ����ھ�����ģʽ
 * @param  catch_wheel_speedMy  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  catch_state          �ʵ����ݳ����ٶȸı�������ٶ�
 */
static void catch_tmp_move(fp32 catch_wheel_speed[2],catch_move_t *catch_state);
#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t catch_high_water;
#endif



//�����˶�����
extern chassis_move_t chassis_move;
//����Ƕ�����
extern angle_read_t  angle_read;
//�����˶�����
catch_move_t catch_move;


/**
  * @brief          catch task, osDelay CATCH_CONTROL_TIME_MS (2ms)
  * @param[in]      pvParameters: null
  * @retval         none
  */
/**
  * @brief          �������񣬼�� CATCH_CONTROL_TIME_MS 2ms
  * @param[in]      pvParameters: ��
  * @retval         none
  */
void catch_task(void const *pvParameters)
{
    //wait a time
    //����һ��ʱ��
    vTaskDelay(CATCH_TASK_INIT_TIME);
    //catch init
    //�����ʼ��
    catch_init(&catch_move);
    //make sure all catch motor is online,
    //�жϳ������Ƿ�����
    while (toe_is_error(YAW_GIMBAL_MOTOR_TOE) || toe_is_error(PITCH_GIMBAL_MOTOR_TOE) || toe_is_error(DBUS_TOE))
    {
        vTaskDelay(CATCH_CONTROL_TIME_MS);
    }

    while (1)
    {
        //vTaskSuspendAll();
        //set catch control mode
        //���ó������ģʽ
        catch_set_mode(&catch_move);
        //when mode changes, some data save
        //ģʽ�л����ݱ��棬������ʵ���ڳ�����˵��û��ʲô��������Ҫ����У׼��
        catch_mode_change_control_transit(&catch_move);
        //catch data update
        //�������ݸ���
        catch_feedback_update(&catch_move);
        //set catch control set-point
        //�������������
        catch_set_contorl(&catch_move);
        //catch control pid calculate
        //�������PID����
        catch_control_loop(&catch_move);

        //make sure  one motor is online at least, so that the control CAN message can be received
        //ȷ������һ��������ߣ� ����CAN���ư����Ա����յ�
        if (!(toe_is_error(YAW_GIMBAL_MOTOR_TOE) && toe_is_error(PITCH_GIMBAL_MOTOR_TOE)))
        {
            //when remote control is offline, catch motor should receive zero current.
            //��ң�������ߵ�ʱ�򣬷��͸������������.
            if (toe_is_error(DBUS_TOE))
            {
                CAN_cmd_catch(0, 0);
            }
            else
            {
                //send control current
                //���Ϳ��Ƶ���
                CAN_cmd_catch(catch_move.motor_catch[0].give_current, catch_move.motor_catch[1].give_current);
            }
        }
        //os delay
        //ϵͳ��ʱ
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
  * @brief          ��ʼ��"catch_move"����������pid��ʼ���� ң����ָ���ʼ����3508���̵��ָ���ʼ������̨�����ʼ���������ǽǶ�ָ���ʼ��
  * @param[out]     catch_move_init:"catch_move"����ָ��.
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
    //�����ٶȻ�pidֵ
    const static fp32 motor_speed_pid[3] = {M3505_MOTOR_SPEED_PID_KP, M3505_MOTOR_SPEED_PID_KI, M3505_MOTOR_SPEED_PID_KD};
    //����ǶȻ�pidֵ,�ɽǶȴ�������ֵȷ��������ٶ�
    const static fp32 angle_speed_pid[3] = {ANGLE_TO_MOTOR_SPEED_PID_KP, ANGLE_TO_MOTOR_SPEED_PID_KI, ANGLE_TO_MOTOR_SPEED_PID_KD};

    catch_move_init->vx=get_vx();
    catch_move_init->vy=get_vy();
    catch_move_init->wz=get_wz();


    uint8_t i;
    //get catch motor data point,  initialize motor speed PID
    //��ȡ����������ָ�룬��ʼ��PID
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
 * @brief                   �����ڲ�ͬ���˶�״̬�£�����ƾ��ǶȻ�pid�ǲ��ܿɿ������
 *                          ��Ҫ�ڳ����Ĳ�ͬ�ٶ��¶�̬��ƥ������ֵ��ٶ�
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
 * @brief  ��ȡadc��ֵ������ת����������ṹ��
 * @param  catch_move_init  ����ṹ��
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
    //����������uint16_tת��Ϊuint32_t
    catch_angle[0] = catch_move_init->p_angle_read->adc1;
    catch_angle[1] = catch_move_init->p_angle_read->adc2;
    //��uint32_tת��Ϊfp32������ṹ��
    catch_move_init->catch_angle[0] = 1.0 * catch_angle[0];
    catch_move_init->catch_angle[1] = 1.0 * catch_angle[1];
}

/**
 * @brief                   ��ʼ���Ǵ���Ŀ��ֵ�������ܻ����ڲ����ʹ��͵��³�ʼ��ʱ����������һ���������ǽǴ����ߵ��¶�����ֵΪ0��
 *                          ������ڱ����̵�read_angle.c�ļ��£�����C����������ݣ���adc_to_uart���������adcת����ķ������ݣ�
 *                          ˼·����Ĭ�ϳ��ǲ�����ģ���ʼ��ʱ �Ǵ�������ֵ �� ����ʱ�ĽǴ���ֵ ����õ�Ŀ��ֵ
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
		//����read_angle.c �����⣬��ʱ��ô����
		catch_move_init->left_aim_angle = SET_LEFT_AIM_ANGLE;
    catch_move_init->right_aim_angle = SET_RIGHT_AIM_ANGLE;
}

/**
  * @brief          set catch control mode, mainly call 'catchs_behaviour_mode_set' function
  * @param[out]     catch_move_mode: "catch_move" valiable point
  * @retval         none
  */
/**
  * @brief          ���ó������ģʽ����Ҫ��'catch_behaviour_mode_set'�����иı�
  * @param[out]     catch_move_mode:"catch_move"����ָ��.
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
  * @brief          ����ģʽ�ı䣬��Щ������Ҫ�ı䣬������̿���yaw�Ƕ��趨ֵӦ�ñ�ɵ�ǰ����yaw�Ƕ�
  * @param[out]     catch_move_transit:"catch_move"����ָ��.
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
 * @brief  ����״̬����
 * @param  catch_move_updateMy Param doc
 */
static void catch_feedback_update(catch_move_t *catch_move_update)
{
    if (catch_move_update == NULL)
    {
        return;
    }
    //���µ��̵��˶�״̬
    catch_move_update->vx=get_vx();
    catch_move_update->vy=get_vy();
    catch_move_update->wz=get_wz();

    //���³���ĽǶ�ֵ   
    angle_sampling(catch_move_update);

    //�ж��Ƿ����
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
        //���³������ٶȣ����ٶ����ٶȵ�PID΢��
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
  * @brief          ���ó����������ֵ, ���ݲ�ͬ�ĵ��̿���ģʽ���ò�ͬ������ĵ����ٶȷ���
  * @param[out]     catch_move_update:"catch_move"����ָ��.
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


    //���ݵ��̵��ٶȽ����������ٶ�
    //CATCH_VECTOR_NO_FOLLOW_YAWģʽ�£� ����Ҫ�ĵ����ٶ�ȫ��Ϊ0
    if(catch_move_control_loop->catch_mode == CATCH_VECTOR_NO_FOLLOW_YAW)
    {
        catch_wheel_no_move(catch_wheel_speed);
        //chassis_vector_to_catch_wheel_speed(0, 0, 0, catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //CATCH_VECTOR_FOLLOW_CHASSISģʽ�£�����Ҫ�ĵ��̵��ٶ�����ʵ�ĵ����ٶȣ�
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS)
    {
        chassis_vector_to_catch_wheel_speed(catch_move_control_loop->vx_used, catch_move_control_loop->vy_used,
                                            catch_move_control_loop->wz_used, catch_wheel_speed,
                                            catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //����Ƕȴ�����״̬�����������ٶ�
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_ANGLE)
    {
        angle_vector_to_catch_wheel_speed(catch_move_control_loop->catch_angle,
                                          catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL],
                                          catch_move_control_loop->angle_speed_pid);
    }
    //������̺ͽǶȴ�������״̬�����������ٶ�
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_CHASSIS_AND_ANGLE)
    {
			  //catch_tmp_move(catch_wheel_speed,catch_move_control_loop);
			  //catch_no_move(catch_wheel_speed);
       multi_catch_wheel_speed(catch_move_control_loop->catch_angle, catch_wheel_speed,
                               catch_move_control_loop->angle_speed_pid,catch_move_control_loop,
                               catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //����ң�����ֶ�ʩ�ӳ������ھ����
    else if(catch_move_control_loop->catch_mode == CATCH_VECTOR_FOLLOW_INVOLUTION)
    {
        rc_involution_to_catch_wheel_speed(catch_wheel_speed, catch_move_control_loop->catch_RC->rc.ch[CATCH_INVOLUTION_CHANNEL]);
    }
    //calculate the max speed in four wheels, limit the max speed
    //�������ӿ�������ٶȣ�������������ٶ�
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
    //����pid
    for (i = 0; i < 2; i++)
    {
        PID_calc(&catch_move_control_loop->motor_speed_pid[i], catch_move_control_loop->motor_catch[i].speed, catch_move_control_loop->motor_catch[i].speed_set);
    }


    //���ʿ���
    //chassis_power_control(chassis_move_control_loop);

    //��ֵ����ֵ
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
 * @brief  �������ϣ������ͨ�����̵��ٶȽ�������������ٶȣ����̵��ٶȴ�Ų��ᳬ��3��ÿ��
 * @param  vx_set             ����x��������ٶ�
 * @param  vy_set             ����y��������ٶ�
 * @param  wz_set             ����z��������ٶ�
 * @param  catch_wheel_speed  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution         �ھ����
 */
static void chassis_vector_to_catch_wheel_speed(const fp32 vx_used, const fp32 vy_used, const fp32 wz_used,
        fp32 catch_wheel_speed[2], int16_t involution)
{
    fp32			SQRT2_2  	=	0.7071;
    fp32 			R 				=	0.165;

    //ǰ��ʱ��Ϊ�˱�֤�򲻱��Ƴ�ȥ������Ҫ�����һ��
    //����ʱ����ϣ�������ȹ̵Ĵ�������Ҫ�����һ��
    //����������������ϣ���ٶȱ仯�ı���
    fp32 increase_param = 1.8;
    fp32 decrease_param = 0.5;
    //�����������������ת�ٹ������µ��ٶ���ʧ
    fp32 compensate_param = 1.2;
    //������vparam��������ת�٣�����һ������
    fp32 left_vx_param = 3;
    fp32 left_vy_param = 4.8;
    fp32 left_wz_param = 6;
    fp32 right_vx_param = 3;
    fp32 right_vy_param = 4.8;
    fp32 right_wz_param = 6;


    //ң����ֱ�ӹ��������ݷ�ΧΪ[-660,660]������ٶȶԳ�����˵̫��
    fp32 involution_param =0.01*involution;
    //x���ٶȷ����ж�
    if(vx_used > 0) {
        left_vx_param = left_vx_param * decrease_param;
        right_vx_param = right_vx_param * decrease_param;
    }
    else if(vx_used < 0) {
        left_vx_param = left_vx_param * increase_param;
        right_vx_param = right_vx_param * increase_param;
    }

    //y���ٶȷ����ж�
    if(vy_used > 0) {
        left_vy_param = left_vy_param * increase_param;
        right_vy_param = right_vy_param * decrease_param;
    }
    else if(vy_used < 0) {
        left_vy_param = left_vy_param * decrease_param;
        right_vy_param = right_vy_param * increase_param;
    }

    //w���ٶȷ����ж�
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
 * @brief                           ͨ���Ƕȴ�������ֵ������������ת��
 * @param  catch_angle              ���Ҳ�������Ƕȴ������ĽǶ�ֵ[0:1]��������
 * @param  catch_wheel_speed        �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution               �ھ����
 * @param  angle_speed_pid          �Ƕȴ�����ȷ�ϵ�pid
 */
fp32 out[2] = {0,0};
static void angle_vector_to_catch_wheel_speed(fp32 catch_angle[2], fp32 catch_wheel_speed[2], int16_t involution,
        pid_type_def *angle_speed_pid )
{

    out[0] = PID_calc(angle_speed_pid, catch_angle[0], SET_LEFT_AIM_ANGLE);
    out[1] = PID_calc(angle_speed_pid, catch_angle[1], SET_RIGHT_AIM_ANGLE);

    //ң����ֱ�ӹ��������ݷ�ΧΪ[-660,660]������ٶȶԳ�����˵̫��
    fp32 involution_param =0.01*involution;
    catch_wheel_speed[0] = out[0] - involution_param;
    catch_wheel_speed[1] = out[1] + involution_param;
}





/**
 * @brief                   
 * @param  catch_angle          �Ƕȴ���������ֵ��[0:1]�������ҽǴ�
 * @param  catch_wheel_speed    �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  angle_speed_pid      �Ƕȴ�����PID
 * @param  catch_state          ����״̬�����Ƿ����
 * @param  involution           �ھ����
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
        if(catch_state->vx > 0)//ǰ��
        {
            dynamic_catch_param.left_vx_param = dynamic_catch_param.vx_decrease;
            dynamic_catch_param.right_vx_param = dynamic_catch_param.vx_decrease;
        }
        else if(catch_state->vx < 0)//����
        {
            dynamic_catch_param.left_vx_param = dynamic_catch_param.vx_increase;
            dynamic_catch_param.right_vx_param = dynamic_catch_param.vx_increase;
        }
        if(catch_state->vy > 0)//��ƽ��
        {
            dynamic_catch_param.left_vy_param = dynamic_catch_param.vy_decrease;
            dynamic_catch_param.right_vy_param = dynamic_catch_param.vy_increase;
        }
        else if(catch_state->vy < 0)//��ƽ��
        {
            dynamic_catch_param.left_vy_param = dynamic_catch_param.vy_increase;
            dynamic_catch_param.right_vy_param = dynamic_catch_param.vy_decrease;
        }
        if(catch_state->wz > 0)//����ת
        {
            dynamic_catch_param.left_wz_param = dynamic_catch_param.wz_decrease;
            dynamic_catch_param.right_wz_param = dynamic_catch_param.wz_increase;
        }
        else if(catch_state->wz < 0)//����ת
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

    //ң����ֱ�ӹ��������ݷ�ΧΪ[-660,660]������ٶȶԳ�����˵̫��
		//�����Ҫͨ��ң������ֵ�ֶ����Ƴ����ֵ�ת�٣����޸�����ע�ͣ�������ע�⣡ң������ͨ��ͬʱ���Ƶ��ݵ�������ѹ��
		//fp32 involution_param =0.01*involution;
		fp32 involution_param = 0;
    catch_wheel_speed[0] = out[0] - involution_param;
    catch_wheel_speed[1] = out[1] + involution_param;
}


/**
 * @brief                     ����ң�����ֶ�ʩ�ӳ������ھ����
 * @param  catch_wheel_speed  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  involution         �ھ����
 */
static void rc_involution_to_catch_wheel_speed(fp32 catch_wheel_speed[2], int16_t involution)
{
    static fp32 involution_param = 0;
    involution_param += 0.0001*involution;
    catch_wheel_speed[0] = -involution_param;
    catch_wheel_speed[1] = involution_param;
}
/**
 * @brief                       ���ٳ����ھ�����ģʽ
 * @param  catch_wheel_speedMy  �������ĵ������ٶȣ�[0:1]�������ҵ��
 * @param  catch_state          �ʵ����ݳ����ٶȸı�������ٶ�
 */
static void catch_tmp_move(fp32 catch_wheel_speed[2],catch_move_t *catch_state)
{
    catch_wheel_speed[0] = -0.5 + catch_state->vx;
    catch_wheel_speed[1] = 0.5 - catch_state->vx;
	
}

/**
 * @brief   Get the vx object, ����x�᷽����ٶ�
 * @return fp32
 */
static fp32 get_vx()
{
    return  chassis_move.vx;
}
/**
 * @brief Get the vy object, ����y�᷽����ٶ�
 * @return fp32
 */
static fp32 get_vy()
{
    return  chassis_move.vy;
}
/**
 * @brief Get the wz object, ����z�᷽����ٶ�
 * @return fp32
 */
static fp32 get_wz()
{
    return  chassis_move.wz;
}

