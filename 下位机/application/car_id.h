/*
 * @Author: your name
 * @Date: 2021-05-31 19:22:09
 * @LastEditTime: 2021-05-31 19:23:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\car_id.h
 */

#ifndef CAR_ID_H
#define CAR_ID_H

//注意，不同的车辆由于装配的差异，舵机的值和角度传感器的期望值有差异
//如果你有新增的车辆或者需要给不同车辆编号的车烧录程序
//你可以在这里新增或者修改#define CAR_NUMBER,注意一次只能有一辆车被定义
//然后在sevo_task.c里修改舵机pwm的范围，在catch_task.h里修改角传的期望值
//#define CAR_ONE
#define CAR_TWO

#endif

