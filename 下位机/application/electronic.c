/*
 * @Author: your name
 * @Date: 2021-05-18 21:40:58
 * @LastEditTime: 2021-07-14 16:17:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \standard_robot\application\electronic.c
 */
#include "electronic.h"
#include "struct_typedef.h"
#include "cmsis_os.h"

#include "bsp_usart.h"

#include "detect_task.h"
#include "remote_control.h"
#include "unpack_task.h"

const RC_ctrl_t *electronic_rc;


electronic_t electronic;
/**
 * @brief                   ��ʼ����ŵ���
 * @param  electronic       My Param doc
 */
void electronic_init(electronic_t* electronic);

/**
 * @brief                   ��ȡ������ѹֵ
 * @param  electronic       My Param doc
 */
void read_exepert_voltage(electronic_t* electronic);

/**
 * @brief                   ����������ѹֵ
 * @param  electronic       My Param doc
 */
void set_exepert_voltage(electronic_t* electronic);

/**
 * @brief ��ȡ��ŵ�����ӵ�״̬
 */
void read_actual_electronic_state(void);

/**
 * @brief                   ���õ�ŵ�����ӵ�״̬
 * @param  electronic       My Param doc
 */
void set_actual_electronic_state(electronic_t* electronic);

void electronic_task(void const * argument)  //�������
{
    //wait a time
    //����һ��ʱ��
    vTaskDelay(500);
    //��ŵ����ʼ��
    electronic_init(&electronic);
    //electronic.Rc_Flag=0;
    while(1)
    {
        read_exepert_voltage(&electronic);
        set_exepert_voltage(&electronic);
        read_actual_electronic_state();
        set_actual_electronic_state(&electronic);
        osDelay(20);
    }
}

/**
 * @brief                   ��ʼ����ŵ���
 * @param  electronic       My Param doc
 */
void electronic_init(electronic_t* electronic)
{
    //HAL_UART_Receive_DMA(&huart6,electronic->received_data,TRANSMIT_DATA_LENGTH);// ����DMA����
    //��ȡң����ָ��
    electronic_rc = get_remote_control_point();
    //��ʼ��Ҫ���͵����ݱ�������
    electronic->transmit_data[HEAD_CHANNEL] = HEAD;
    electronic->transmit_data[END_CHANNEL] = END;
}

/**
 * @brief                   ��ȡ������ѹֵ
 * @param  electronic       My Param doc
 */
void read_exepert_voltage(electronic_t* electronic)
{
//��ȡң������ͨ��ֵ
//�����������ע��ǧ����ȡ��������
//�ҵı����ǵ�ŵ������ֵ�����ֵֻ�����Ҳ���������ʱ�ſ������ã������ҷ�������д���������bug	
//    if(electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL] == RC_CAP_CHANNEL || 
//			 electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL] == RC_SHOOT_CHANNEL)
//    {
        electronic->rc_cap_value = electronic_rc->rc.ch[ELECTRONIC_CAP_CHANNEL];
        electronic->rc_shoot_value = electronic_rc->rc.s[ELECTRONIC_SHOOT_CHANNEL];
//    }
}
/**
 * @brief                   ����������ѹֵ
 * @param  electronic       My Param doc
 */
void set_exepert_voltage(electronic_t* electronic)
{
    //����ң������ֵ�ı������ĵ�ŵ����ѹֵ
    if(handshake_flag == UNHANDSHAKE)
    {
        electronic->cap_change_value = electronic->rc_cap_value>>5;
        electronic->cap_expert_value += electronic->cap_change_value;
    }
    //��λ�����͵�����ֱ�Ӵ��������ĵ��ݵ�ѹֵ
    else if(handshake_flag == HANDSHAKE)
    {
        electronic->cap_expert_value =electronic->rc_cap_value/1.65;//0-400
    }
    //��ŵ����ѹ�޷�
    if(electronic->cap_expert_value<MIN_ELECTRONIC_VALUE)
    {
        electronic->cap_expert_value = MIN_ELECTRONIC_VALUE;
    }
    if(electronic->cap_expert_value>MAX_ELECTRONIC_VALUE)
    {
        electronic->cap_expert_value = MAX_ELECTRONIC_VALUE;
    }
}

/**
 * @brief ��ȡ��ŵ�����ӵ�״̬
 */
void read_actual_electronic_state()
{
    //������ŵ�����ӵ�DMA����
    //HAL_UART_Receive_DMA(&huart6,dianci.buf,7);// ����DMA����
}

/**
 * @brief                   ���õ�ŵ�����ӵ�״̬
 * @param  electronic       My Param doc
 */
void set_actual_electronic_state(electronic_t* electronic)
{
    //if(electronic->Rc_Flag==0) return ;
    //�������״̬
    if(electronic->rc_shoot_value == SHOOT)
    {
        electronic->transmit_data[STATE_CHANNEL] = SHOOT;
    }
    else
    {
        electronic->transmit_data[STATE_CHANNEL] = UNSHOOT;
    }
    //�����������ݵ���
    electronic->transmit_data[CAP_CHANNEL] = electronic->cap_expert_value;
    electronic->transmit_data[CAP_CHANNEL+1] = (electronic->cap_expert_value)>>8;
    //bccУ��
    electronic->transmit_data[BCC_CHANNEL] = check_bcc(electronic->transmit_data,BCC_CHANNEL);
    //���ڷ���
    HAL_UART_Transmit(&huart6,electronic->transmit_data,sizeof(electronic->transmit_data),0xffff);
    //DMA����
    //HAL_UART_Transmit_DMA(&huart6,electronic->transmit_data,sizeof(electronic->transmit_data));
    //electronic->Rc_Flag=0;
}

//void USART6_IRQHandler(void);
