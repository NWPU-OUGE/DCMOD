/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       servo_task.c/h
  * @brief      
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Oct-21-2019     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "servo_task.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_servo_pwm.h"
#include "remote_control.h"
#include "car_id.h"

//ң�������ƶ����ͨ��
#define SERVO_ANGLE_CHANNEL 3
//����Ƕȶ��ͨ����ֵ
#define SHOOT_ANGLE_SERVO_CHANNEL 0


#ifdef CAR_ONE
#define SERVO_MIN_PWM   650
#define SERVO_MAX_PWM   1100
#endif

#ifdef CAR_TWO
#define SERVO_MIN_PWM   1070
#define SERVO_MAX_PWM   1400
#endif

#define PWM_DETAL_VALUE 10

#define SERVO1_ADD_PWM_KEY  KEY_PRESSED_OFFSET_Z
#define SERVO2_ADD_PWM_KEY  KEY_PRESSED_OFFSET_X
#define SERVO3_ADD_PWM_KEY  KEY_PRESSED_OFFSET_C
#define SERVO4_ADD_PWM_KEY  KEY_PRESSED_OFFSET_V

#define SERVO_MINUS_PWM_KEY KEY_PRESSED_OFFSET_SHIFT

const RC_ctrl_t *servo_rc;
//const static uint16_t servo_key[4] = {SERVO1_ADD_PWM_KEY, SERVO2_ADD_PWM_KEY, SERVO3_ADD_PWM_KEY, SERVO4_ADD_PWM_KEY};
//uint16_t servo_pwm[4] = {SERVO_MIN_PWM, SERVO_MIN_PWM, SERVO_MIN_PWM, SERVO_MIN_PWM};

typedef struct
{
    int16_t RC_value;           //ң����ͨ��3��ֵ����Χ��-660��660
    int16_t pwm_value;          //���������ֵ����Χ��500��2500
    int16_t pwm_change_value;   //���pwm��ʱ�ı����
}servo_t;

servo_t shoot_angle_servo;

/**
 * @brief                       ��ʼ����������ֵ
 */
void servo_init()
{
    shoot_angle_servo.pwm_value = (SERVO_MIN_PWM +SERVO_MAX_PWM)/2;
    shoot_angle_servo.RC_value = 0;
}

/**
 * @brief                       ����ң������ֵ�����������Ƕȶ����ֵ
 * @param  servo                //����Ƕȶ��
 * @param  servo_rc             //ң�����ṹ�����
 */

void rc_to_servo_caluate(servo_t* servo ,const RC_ctrl_t *servo_rc)
{
    servo->RC_value = servo_rc->rc.ch[SERVO_ANGLE_CHANNEL];
    servo->pwm_change_value = servo->RC_value >> 4; //ԭ��ң������ͨ���ķ�Χ��-660��660���Ĵ�ʱ���ķ�ΧΪ-20 ��20
    servo->pwm_value -= servo->pwm_change_value;
    if(servo->pwm_value < SERVO_MIN_PWM)
    {
        servo->pwm_value = SERVO_MIN_PWM;
    }
    else if(servo->pwm_value > SERVO_MAX_PWM)
    {
        servo->pwm_value = SERVO_MAX_PWM;
    }

}

/**
 * @brief                   ����Ƕȶ������
 * @param  argument         My Param doc
 */
void servo_task(void const * argument)
{
    servo_rc = get_remote_control_point();
		servo_init();
    while(1)
    {
        rc_to_servo_caluate(&shoot_angle_servo ,servo_rc);
        servo_pwm_set(shoot_angle_servo.pwm_value , SHOOT_ANGLE_SERVO_CHANNEL);
        osDelay(10);
    }
}

/**
  * @brief          servo_task
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
/**
  * @brief          �������
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
// void servo_task(void const * argument)
// {
//     servo_rc = get_remote_control_point();

//     while(1)
//     {
//         for(uint8_t i = 0; i < 4; i++)
//         {

//             if( (servo_rc->key.v & SERVO_MINUS_PWM_KEY) && (servo_rc->key.v & servo_key[i]))
//             {
//                 servo_pwm[i] -= PWM_DETAL_VALUE;
//             }
//             else if(servo_rc->key.v & servo_key[i])
//             {
//                 servo_pwm[i] += PWM_DETAL_VALUE;
//             }

//             //limit the pwm
//            //����pwm
//             if(servo_pwm[i] < SERVO_MIN_PWM)
//             {
//                 servo_pwm[i] = SERVO_MIN_PWM;
//             }
//             else if(servo_pwm[i] > SERVO_MAX_PWM)
//             {
//                 servo_pwm[i] = SERVO_MAX_PWM;
//             }

//             servo_pwm_set(servo_pwm[i], i);
//         }
//         osDelay(10);
//     }
// }


