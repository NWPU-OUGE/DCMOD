/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "olediic.h"
#include "electronic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint16_t ADC_VALUE=0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef uint8_t   u8;
unsigned char temp_data = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef struct __Buffer_Type
{
    u8 buffer[64];
    u8 vbuffer[64];
    unsigned int write_count;

} Buffer_Type;
Buffer_Type HOST_Buffer;
int Flag=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t received_data[6];
uint8_t transmit_data[6] = {HEAD,0,0,0,0,END};
uint8_t ADC_VALUE_HIGH;
uint8_t ADC_VALUE_LOW;
uint8_t control_source;

int16_t expert_cap_value=0;
int16_t actual_cap_value=0;
uint8_t expert_shoot_state = 0;
uint8_t actual_shoot_state = 0;
/* 数据包：74 00 00  74 b8 */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void get_actual_value_tx(void);

/**
 * @brief 进行bcc校验，返回校验结果
 *
 * @param data 要校验的数组地址
 * @param sum 校验的数据位数（单位：字节）
 * @return unsigned int
 */
unsigned int check_bcc(unsigned char * data,unsigned int sum)
{
    unsigned char bcc_num = 0;
    unsigned int i = 0;
    while(i < sum)
    {
        bcc_num ^= *data;
        data++;
        i++;
    }
    return bcc_num;
}

void get_data_from_mcu(void);
void get_expert_value_rx(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_ADC_Init();
    MX_USART1_UART_Init();
    MX_TIM3_Init();
    /* USER CODE BEGIN 2 */
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0,0,"Vol:      V",16);
    OLED_ShowString(0,2,"Charging:",16);
    OLED_ShowString(0,4,"Discharg:",16);
    OLED_ShowString(0,6,"Shooting:",16);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_Delay(100);
    HOST_Buffer.write_count=0;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        OLED_ShowNum(36,0,actual_cap_value,5,16);
        //strcat(transmit_data,HYM);
        //HAL_UART_Transmit_DMA(&huart1,LYH,16);

        HAL_UART_Receive_DMA(&huart1,&temp_data,1);
        get_data_from_mcu();//从上位机获得数据
        get_expert_value_rx();//解析获得的数据
        get_actual_value_tx();//获取实际信息并向上位机发送状态
        electronic_dispatch();//控制权分配，
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI14CalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void get_data_from_mcu()
{
    if(HOST_Buffer.write_count<6)
    {
        Flag=0;
    }
    if(HOST_Buffer.write_count>=6) Flag=1;

    if(HOST_Buffer.buffer[HOST_Buffer.write_count-1]==END&&Flag==1)
    {
        if(HOST_Buffer.buffer[HOST_Buffer.write_count-6]==HEAD)
        {
            received_data[RX_HEAD_CHANNEL] = HOST_Buffer.buffer[HOST_Buffer.write_count-6];
            received_data[RX_STATE_CHANNEL] = HOST_Buffer.buffer[HOST_Buffer.write_count-5];
            received_data[RX_VOLTAGE_CHANNEL] = HOST_Buffer.buffer[HOST_Buffer.write_count-4];
            received_data[RX_VOLTAGE_CHANNEL+1] = HOST_Buffer.buffer[HOST_Buffer.write_count-3];
            received_data[RX_BCC_CHANNEL] = HOST_Buffer.buffer[HOST_Buffer.write_count-2];
            received_data[RX_END_CHANNEL] = HOST_Buffer.buffer[HOST_Buffer.write_count-1];
            control_source=CONTROLED_BY_MCU;
        }
    }
}
void get_expert_value_rx(void)
{
    expert_shoot_state = received_data[RX_STATE_CHANNEL];
    expert_cap_value = received_data[RX_VOLTAGE_CHANNEL];
    expert_cap_value|= (received_data[RX_VOLTAGE_CHANNEL+1])<<8;
}

void get_actual_value_tx()
{
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, 10);    //等待转换完成，第二个参数表示超时时间，单位ms
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC))
    {
        ADC_VALUE=HAL_ADC_GetValue(&hadc);
    }
    actual_cap_value = ADC_VALUE*6/35;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
}

uint8_t check_control_src()
{
    if(received_data[RX_HEAD_CHANNEL] == HEAD &&
            received_data[RX_END_CHANNEL] == END &&
            received_data[RX_BCC_CHANNEL] == check_bcc(received_data,RX_BCC_CHANNEL))
    {
        return CONTROLED_BY_MCU;
    }
    else
    {
        return CONTROLED_BY_EXTI;
    }

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
        control_source = check_control_src();
        // get_expert_value_rx();
    }
    if(HOST_Buffer.write_count < 64)
    {
        HOST_Buffer.buffer[HOST_Buffer.write_count] = temp_data;
        HOST_Buffer.write_count++;
    }
    else
    {
        HOST_Buffer.write_count = 0;
        HOST_Buffer.buffer[HOST_Buffer.write_count] = temp_data;
        HOST_Buffer.vbuffer[HOST_Buffer.write_count] = temp_data;
        HOST_Buffer.write_count++;
    }

}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	HAL_UART_Transmit_DMA(&huart1,LYY,12);
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
