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
    HAL_UART_Receive_DMA(&huart1,angle_read.Buf,30);// 启动DMA接收
}

//uint8_t check_buf(void)
//{
//    /* 检测接收的数据是否正确，成功返回1，失败返回0
//    * 使用了USART1 4Pin的接口  波特率为115200 BPS
//    * 通信协议：包头+数据流+包尾+校验 共计12个字节
//    * 包头：一个字节 0x2d   angle_read.Buf[0]
//    * 数据流：两个字节ADC1 +两个字节ADC2 +两个字节ADC3 +两个字节ADC4传感 1-8
//    * 包尾：一个字节 0x73   angle_read.Buf[9]
//    */
//	/*
//        检测接收的数据是否正确，成功返回1，失败返回0
//        * 使用了USART1 4Pin的接口  波特率为115200 BPS
//        * 通信协议：包头+数据流+包尾+校验 共计12个字节
//        * 包头：一个字节 0x2d   ADC_Read.Buf[0]
//        * 数据流：两个字节ADC1 +两个字节ADC2 +两个字节ADC3 +两个字节ADC4传感 1-8
//        * 校验：整理出四个ADC值相加的值发送到C板端  ADC_Read.Buf[9]   ADC_Read.Buf[10]
//        * 包尾：一个字节 0x73   ADC_Read.Buf[11]
//        */
//    angle_read.flag=0;
//    angle_read.num=0;

//    __HAL_DMA_DISABLE(huart1.hdmarx);

//    //检测包头：
//    if(angle_read.Buf[0]!=0x2d)
//    {
//        huart1.hdmarx->Instance->CR |= DMA_SxCR_CT;
//        __HAL_DMA_ENABLE(huart1.hdmarx);
//        return 0;
//    }
//    //检测包尾
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
 * @brief 读取角度传感器的ADC串口任务
 * @param argument
 */
void angle_usart_task(void const * argument)
{
    osDelay(100);
    ADC_UART_Read_Init();//初始化
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