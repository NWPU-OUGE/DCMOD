#include "read_angle.h"
#include "struct_typedef.h"
#include "cmsis_os.h"

#include "bsp_usart.h"
#include "detect_task.h"

angle_read_t angle_read;
extern UART_HandleTypeDef huart1;



void ADC_UART_Read_Init(void)
{
    uint8_t i;
    angle_read.flag=0;
    angle_read.adc1=0;
    angle_read.adc2=0;
    angle_read.adc3=0;
    angle_read.adc4=0;
    for(i=0; i<30; i++)
    {
        angle_read.Buf[i]=0x00;
    }
    angle_read.num=0;
    HAL_UART_Receive_DMA(&huart1,angle_read.Buf,30);// ����DMA����
}

//uint8_t check_buf(void)
//{
//    /* �����յ������Ƿ���ȷ���ɹ�����1��ʧ�ܷ���0
//    * ʹ����USART1 4Pin�Ľӿ�  ������Ϊ115200 BPS
//    * ͨ��Э�飺��ͷ+������+��β+У�� ����12���ֽ�
//    * ��ͷ��һ���ֽ� 0x2d   angle_read.Buf[0]
//    * �������������ֽ�ADC1 +�����ֽ�ADC2 +�����ֽ�ADC3 +�����ֽ�ADC4���� 1-8
//    * ��β��һ���ֽ� 0x73   angle_read.Buf[9]
//    */
//	/*
//        �����յ������Ƿ���ȷ���ɹ�����1��ʧ�ܷ���0
//        * ʹ����USART1 4Pin�Ľӿ�  ������Ϊ115200 BPS
//        * ͨ��Э�飺��ͷ+������+��β+У�� ����12���ֽ�
//        * ��ͷ��һ���ֽ� 0x2d   ADC_Read.Buf[0]
//        * �������������ֽ�ADC1 +�����ֽ�ADC2 +�����ֽ�ADC3 +�����ֽ�ADC4���� 1-8
//        * У�飺������ĸ�ADCֵ��ӵ�ֵ���͵�C���  ADC_Read.Buf[9]   ADC_Read.Buf[10]
//        * ��β��һ���ֽ� 0x73   ADC_Read.Buf[11]
//        */
//    angle_read.flag=0;
//    angle_read.num=0;

//    __HAL_DMA_DISABLE(huart1.hdmarx);

//    //����ͷ��
//    if(angle_read.Buf[0]!=0x2d)
//    {
//        huart1.hdmarx->Instance->CR |= DMA_SxCR_CT;
//        __HAL_DMA_ENABLE(huart1.hdmarx);
//        return 0;
//    }
//    //����β
//    if(angle_read.Buf[9]!=0x73)
//    {
//        huart1.hdmarx->Instance->CR |= DMA_SxCR_CT;
//        __HAL_DMA_ENABLE(huart1.hdmarx);
//        return 0;
//    }

//    huart1.hdmarx->Instance->CR |= DMA_SxCR_CT;
//    __HAL_DMA_ENABLE(huart1.hdmarx);
//    return 1;
//}
/**
 * @brief ��ȡ�Ƕȴ�������ADC��������
 * @param argument
 */
void angle_usart_task(void const * argument)
{
    osDelay(100);
    ADC_UART_Read_Init();//��ʼ��
    while(1)
    {
//			__HAL_DMA_DISABLE(huart1.hdmarx);
//        angle_read.adc1=(uint16_t)angle_read.Buf[1+2*0];
//        angle_read.adc1<<=8;
//        angle_read.adc1|=(uint16_t)angle_read.Buf[2+2*0];
//			
//			
//			
//			
//        angle_read.adc2=(uint16_t)angle_read.Buf[1+2*1];
//        angle_read.adc2<<=8;
//        angle_read.adc2|=(uint16_t)angle_read.Buf[2+2*1];
//			
//			
//			
//			
//        angle_read.adc3=(uint16_t)angle_read.Buf[1+2*2];
//        angle_read.adc3<<=8;
//        angle_read.adc3|=(uint16_t)angle_read.Buf[2+2*2];
//			
//			
//			
//        angle_read.adc4=(uint16_t)angle_read.Buf[1+2*3];
//        angle_read.adc4<<=8;
//        angle_read.adc4|=(uint16_t)angle_read.Buf[2+2*3];
//        osDelay(5);
			for(int i=0;i<30;i++)
			{
				if(angle_read.Buf[i]==0x2d&&i<21)
				{
					if(angle_read.Buf[i+9]==0x73)
					{

						  angle_read.adc1=(uint16_t)angle_read.Buf[i+1+2*0];
              angle_read.adc1<<=8;
              angle_read.adc1|=(uint16_t)angle_read.Buf[i+2+2*0];
						
						

							angle_read.adc2=(uint16_t)angle_read.Buf[i+1+2*1];
							angle_read.adc2<<=8;
							angle_read.adc2|=(uint16_t)angle_read.Buf[i+2+2*1];
						
						
						
						
							angle_read.adc3=(uint16_t)angle_read.Buf[i+1+2*2];
							angle_read.adc3<<=8;
							angle_read.adc3|=(uint16_t)angle_read.Buf[i+2+2*2];
						
						
						
							angle_read.adc4=(uint16_t)angle_read.Buf[i+1+2*3];
							angle_read.adc4<<=8;
							angle_read.adc4|=(uint16_t)angle_read.Buf[i+2+2*3];
							
					    break;
					
					}
					
				}	
			}
				__HAL_DMA_ENABLE(huart1.hdmarx);
				osDelay(10);
    }
}



void HAL_UART_RxCpltCallBack(UART_HandleTypeDef *huart)
{
    if(huart==&huart1)
    {
        //check_buf();
    }
}

//void HAL_UART_TxCpltCallBack(UART_HandleTypeDef *huart)
//{

//}