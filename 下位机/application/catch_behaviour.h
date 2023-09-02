/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       catch_behaviour.c/h
  * @brief      according to remote control, change the catch behaviour.
  *             根据遥控器的值，决定持球行为。
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
    如果要添加一个新的行为模式
    1.首先，在catch_behaviour.h文件中， 添加一个新行为名字在 catch_behaviour_e
    erum
    {  
        ...
        ...
        CATCH_XXX_XXX, // 新添加的
    }catch_behaviour_e,

    2. 实现一个新的函数 catch_xxx_xxx_control(fp32 *vx, fp32 *vy, fp32 *wz, catch_move_t * catch )
        "vx,vy,wz" 参数是底盘运动控制输入量
        第一个参数: 'vx' 通常控制纵向移动,正值 前进， 负值 后退
        第二个参数: 'vy' 通常控制横向移动,正值 左移, 负值 右移
        第三个参数: 'wz' 可能是角度控制或者旋转速度控制
        在这个新的函数, 你能给 "vx","vy",and "wz" 赋值想要的速度参数
    3.  在"catch_behaviour_mode_set"这个函数中，添加新的逻辑判断，给catch_behaviour_mode赋值成CATCH_XXX_XXX
        在函数最后，添加"else if(catch_behaviour_mode == CATCH_XXX_XXX)" ,然后选择一种底盘控制模式
        4种:
        CATCH_VECTOR_FOLLOW_GIMBAL_YAW : 'vx' and 'vy'是速度控制， 'wz'是角度控制 云台和底盘的相对角度
        你可以命名成"xxx_angle_set"而不是'wz'
        CATCH_VECTOR_FOLLOW_CATCH_YAW : 'vx' and 'vy'是速度控制， 'wz'是角度控制 底盘的陀螺仪计算出的绝对角度
        你可以命名成"xxx_angle_set"
        CATCH_VECTOR_NO_FOLLOW_YAW : 'vx' and 'vy'是速度控制， 'wz'是旋转速度控制
        CATCH_VECTOR_RAW : 使用'vx' 'vy' and 'wz'直接线性计算出车轮的电流值，电流值将直接发送到can 总线上
    4.  在"catch_behaviour_control_set" 函数的最后，添加
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
  CATCH_NO_MOVE,                         //持球保持不动，可更据遥控器手动设置瞬时内卷风量
  CATCH_FOLLOW_CHASSIS,                  //持球跟随底盘模式
  CATCH_FOLLOW_ANGLE,                    //持球跟随角度传感器角度环
  CATCH_FOLLOW_CHASSIS_AND_ANGLE,        //持球同时又底盘和角度传感器角度环决定
  CATCH_FOLLOW_INVOLUTION                //内卷模式，根据遥控器手动设置施加持续内卷分量
} catch_behaviour_e;


/**
  * @brief          logical judgement to assign "catch_behaviour_mode" variable to which mode
  * @param[in]      catch_move_mode: catch data
  * @retval         none
  */
/**
  * @brief          通过逻辑判断，赋值"catch_behaviour_mode"成哪种模式
  * @param[in]      catch_move_mode: 底盘数据
  * @retval         none
  */
extern void catch_behaviour_mode_set(catch_move_t *catch_move_mode);

#endif
