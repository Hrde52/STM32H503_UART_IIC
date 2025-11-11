/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define DebugPrintfFlag  0  // 1����ģʽ 0 �����,�п��Ź���2 �޿��Ź�
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern uint8_t isToolingTest;
extern uint8_t getIN_IO;
#define RX_BUF_SIZE 30

extern uint8_t rxBuf[RX_BUF_SIZE];
extern uint16_t receivedLength;
extern uint8_t cargoLift_IN;

void ProcessDTSDMAData(uint8_t *data, uint16_t length);
int8_t checkSequenceMatch(const unsigned char received[30]);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_GHP_TX_Pin GPIO_PIN_2
#define RS485_GHP_TX_GPIO_Port GPIOA
#define RS485_GHP_RX_Pin GPIO_PIN_3
#define RS485_GHP_RX_GPIO_Port GPIOA
#define DTS6012_TX_Pin GPIO_PIN_10
#define DTS6012_TX_GPIO_Port GPIOB
#define IO_IN_Pin GPIO_PIN_12
#define IO_IN_GPIO_Port GPIOB
#define IO_OUT_Pin GPIO_PIN_13
#define IO_OUT_GPIO_Port GPIOB
#define RS485_TX_Pin GPIO_PIN_14
#define RS485_TX_GPIO_Port GPIOB
#define RS485_RX_Pin GPIO_PIN_15
#define RS485_RX_GPIO_Port GPIOB
#define ND06_EN_Pin GPIO_PIN_8
#define ND06_EN_GPIO_Port GPIOA
#define TOF_LED_Pin GPIO_PIN_9
#define TOF_LED_GPIO_Port GPIOA
#define DTS6012M_INT_Pin GPIO_PIN_10
#define DTS6012M_INT_GPIO_Port GPIOA
#define ND06_XSHUT_Pin GPIO_PIN_15
#define ND06_XSHUT_GPIO_Port GPIOA
#define RS485_GHP_EN_Pin GPIO_PIN_3
#define RS485_GHP_EN_GPIO_Port GPIOB
#define DTS6012_EN_Pin GPIO_PIN_4
#define DTS6012_EN_GPIO_Port GPIOB
#define RS485_EN_Pin GPIO_PIN_5
#define RS485_EN_GPIO_Port GPIOB
#define DTS6012_RX_Pin GPIO_PIN_8
#define DTS6012_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void ToolingTest();
void normalWork();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
