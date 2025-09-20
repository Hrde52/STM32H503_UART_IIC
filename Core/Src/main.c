/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "icache.h"
#include "iwdg.h"
#include "memorymap.h"
#include "rtc.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "PDA_Communication.h"
#include "dts6012m.h"
#include "elevatorSignalMonitor.h"
#include "nd06av1c_app.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "nd06av1c_dev.h"
#include "platform.h"
#include "rs485.h"
#include "sensorAppLogic.h"
#include "sensorParaTable.h"
#include "stdbool.h"
#include "string.h"
#include "usart.h"
#include <stdio.h>
#include "toolingtest.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t isToolingTest = 0;
uint8_t RxTestTooling[5];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
    MX_ICACHE_Init();
    MX_RTC_Init();
    HAL_Delay(1000);
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
#if (DebugPrintfFlag != 2)
    MX_IWDG_Init();
#endif

    /* USER CODE BEGIN 2 */

    HAL_UART_DeInit(&huart3); //
    MX_GPIO_DTS6012_Init();
    HAL_GPIO_WritePin(DTS6012_EN_GPIO_Port, DTS6012_EN_Pin, 0); //
    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, 0);       //
    HAL_Delay(500);
    HAL_GPIO_WritePin(DTS6012_EN_GPIO_Port, DTS6012_EN_Pin, 1); //
    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, 1);
    HAL_Delay(500);
#if (DebugPrintfFlag != 2)
    HAL_IWDG_Refresh(&hiwdg);
#endif

    HAL_Delay(1000);
#if (DebugPrintfFlag != 2)
    HAL_IWDG_Refresh(&hiwdg);
#endif

    paraTable_Init();
    MX_USART3_UART_Init();

    ND06AV1C_Init();
    dts6012_start();

    if (PARA_TABLE_USE.data.programVerison != 0x0100)
    {
        ToolingTest();
    }

    else
    {
        isToolingTest = 0;
        normalWork();
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

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_CSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.CSIState = RCC_CSI_ON;
    RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 125;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure the programming delay
     */
    __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/* USER CODE BEGIN 4 */
void ToolingTest()
{
    LEDON;
    isToolingTest = 1;
    initTooling();
    HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);
    // HAL_Delay(100);
    HAL_UART_Receive_IT(&huart1, &rxDBuffTooling[rxIndexTooling], 1);

    //    HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_RESET);
    //		HAL_Delay(100);
    //    HAL_UART_Receive_IT(&huart2, &rxDBuffTooling[rxIndexTooling], 1);
    while (1)
    {
        if (isToolingTest == 0)
        {

            return;
        }
    }
    LEDOFF;
    // isToolingTest = 0;
}

void normalWork()
{

    RS485_PDA_RX_ENABLE();
    HAL_UART_Receive_IT(&RS485_PDA_USART, rxDBuffPDA, 1);

    RS485_Elevator_Init();

    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        switch (PARA_TABLE_USE.data.functionChioce)
        {
        case 0x11: //
        {
            if (TIME_1MS_FLAG == 1)
            {
                TIME_1MS_FLAG = 0;
            }
            if (TIME_5MS_FLAG == 1)
            {
                TIME_5MS_FLAG = 0;
                sensor_action_control();
            }
            if (TIME_10MS_FLAG == 1)
            {
                TIME_10MS_FLAG = 0;

                UpdateDoorStateMachine();
                ElevatorSignaMonitoring();
                sensor_status_control();
            }
            if (TIME_100MS_FLAG == 1)
            {
                TIME_100MS_FLAG = 0;
            }
            if (TIME_1S_FLAG == 1)
            {
                TIME_1S_FLAG = 0;
                dts6012_start();
#if (DebugPrintfFlag != 2)
                HAL_IWDG_Refresh(&hiwdg);
#endif
            }
            break;
        }
        case 0x22: //
        {
            if (TIME_1MS_FLAG == 1)
            {
                TIME_1MS_FLAG = 0;
            }
            if (TIME_5MS_FLAG == 1)
            {
                TIME_5MS_FLAG = 0;
                cargoLift_sensor_action_control();
            }
            if (TIME_10MS_FLAG == 1)
            {
                TIME_10MS_FLAG = 0;
                cargoLift_sensor_status_control();
            }
            if (TIME_100MS_FLAG == 1)
            {
                TIME_100MS_FLAG = 0;
            }
            if (TIME_1S_FLAG == 1)
            {
                TIME_1S_FLAG = 0;
                dts6012_start();
#if (DebugPrintfFlag != 2)
                HAL_IWDG_Refresh(&hiwdg);
#endif
            }
            break;
        }

        default:
            break;
        }
    }
}
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

#ifdef USE_FULL_ASSERT
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
