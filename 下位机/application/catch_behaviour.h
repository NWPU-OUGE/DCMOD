/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       catch_behaviour.c/h
  * @brief      according to remote control, change the catch behaviour.
  *             ����ң������ֵ������������Ϊ��
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.1.0     Nov-11-2019     RM              1. add some annotation
  *
  @verbatim
  ==============================================================================
    add a catch behaviour mode
    1. in catch_behaviour.h , add a new behaviour name in catch_behaviour
    erum
    {  
        ...
        ...
        CATCH_XXX_XXX, // new add
    }catch_behaviour_e,
    2. implement new function. catch_xxx_xxx_control(fp32 *vx, fp32 *vy, fp32 *wz, catch_move_t * catch )
        "vx, vy, wz" param is catch movement contorl input. 
        first param: 'vx' usually means  vertical speed,
            positive value means forward speed, negative value means backward speed.
        second param: 'vy' usually means horizotal speed,
            positive value means letf speed, negative value means right speed
        third param: 'wz' can be rotation speed set or angle set, 

        in this new function, you can assign speed to "vx","vy",and "wz",as your wish
    3.  in "catch_behaviour_mode_set" function, add new logical judgement to assign CATCH_XXX_XXX to  "catch_behaviour_mode" variable,
        and in the last of the function, add "else if(catch_behaviour_mode == CATCH_XXX_XXX)" 
        choose a catch control mode.
        four mode:
        CATCH_VECTOR_FOLLOW_GIMBAL_YAW : 'vx' and 'vy' are speed control, 'wz' is angle set to control relative angle
            between catch and gimbal. you can name third param to 'xxx_angle_set' other than 'wz'
        CATCH_VECTOR_FOLLOW_CATCH_YAW : 'vx' and 'vy' are speed control, 'wz' is angle set to control absolute angle calculated by gyro
            you can name third param to 'xxx_angle_set.
        CATCH_VECTOR_NO_FOLLOW_YAW : 'vx' and 'vy' are speed control, 'wz' is rotation speed control.
        CATCH_VECTOR_RAW : will use 'vx' 'vy' and 'wz'  to linearly calculate four wheel current set, 
            current set will be derectly sent to can bus.
    4. in the last of "catch_behaviour_control_set" function, add
        else if(catch_behaviour_mode == CATCH_XXX_XXX)
        {
            catch_xxx_xxx_control(vx_set, vy_set, angle_set, catch_move_rc_to_vector);
        }
    ���Ҫ���һ���µ���Ϊģʽ
    1.���ȣ���catch_behaviour.h�ļ��У� ���һ������Ϊ������ catch_behaviour_e
    erum
    {  
        ...
        ...
        CATCH_XXX_XXX, // ����ӵ�
    }catch_behaviour_e,

    2. ʵ��һ���µĺ��� catch_xxx_xxx_control(fp32 *vx, fp32 *vy, fp32 *wz, catch_move_t * catch )
        "vx,vy,wz" �����ǵ����˶�����������
        ��һ������: 'vx' ͨ�����������ƶ�,��ֵ ǰ���� ��ֵ ����
        �ڶ�������: 'vy' ͨ�����ƺ����ƶ�,��ֵ ����, ��ֵ ����
        ����������: 'wz' �����ǽǶȿ��ƻ�����ת�ٶȿ���
        ������µĺ���, ���ܸ� "vx","vy",and "wz" ��ֵ��Ҫ���ٶȲ���
    3.  ��"catch_behaviour_mode_set"��������У�����µ��߼��жϣ���catch_behaviour_mode��ֵ��CATCH_XXX_XXX
        �ں���������"else if(catch_behaviour_mode == CATCH_XXX_XXX)" ,Ȼ��ѡ��һ�ֵ��̿���ģʽ
        4��:
        CATCH_VECTOR_FOLLOW_GIMBAL_YAW : 'vx' and 'vy'���ٶȿ��ƣ� 'wz'�ǽǶȿ��� ��̨�͵��̵���ԽǶ�
        �����������"xxx_angle_set"������'wz'
        CATCH_VECTOR_FOLLOW_CATCH_YAW : 'vx' and 'vy'���ٶȿ��ƣ� 'wz'�ǽǶȿ��� ���̵������Ǽ�����ľ��ԽǶ�
        �����������"xxx_angle_set"
        CATCH_VECTOR_NO_FOLLOW_YAW : 'vx' and 'vy'���ٶȿ��ƣ� 'wz'����ת�ٶȿ���
        CATCH_VECTOR_RAW : ʹ��'vx' 'vy' and 'wz'ֱ�����Լ�������ֵĵ���ֵ������ֵ��ֱ�ӷ��͵�can ������
    4.  ��"catch_behaviour_control_set" ������������
        else if(catch_behaviour_mode == CATCH_XXX_XXX)
        {
            catch_xxx_xxx_control(vx_set, vy_set, angle_set, catch_move_rc_to_vector);
        }
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#ifndef CATCH_BEHAVIOUR_H
#define CATCH_BEHAVIOUR_H
#include "struct_typedef.h"
#include "catch_task.h"

typedef enum
{
  CATCH_NO_MOVE,                         //���򱣳ֲ������ɸ���ң�����ֶ�����˲ʱ�ھ����
  CATCH_FOLLOW_CHASSIS,                  //����������ģʽ
  CATCH_FOLLOW_ANGLE,                    //�������Ƕȴ������ǶȻ�
  CATCH_FOLLOW_CHASSIS_AND_ANGLE,        //����ͬʱ�ֵ��̺ͽǶȴ������ǶȻ�����
  CATCH_FOLLOW_INVOLUTION                //�ھ�ģʽ������ң�����ֶ�����ʩ�ӳ����ھ����
} catch_behaviour_e;


/**
  * @brief          logical judgement to assign "catch_behaviour_mode" variable to which mode
  * @param[in]      catch_move_mode: catch data
  * @retval         none
  */
/**
  * @brief          ͨ���߼��жϣ���ֵ"catch_behaviour_mode"������ģʽ
  * @param[in]      catch_move_mode: ��������
  * @retval         none
  */
extern void catch_behaviour_mode_set(catch_move_t *catch_move_mode);

#endif
