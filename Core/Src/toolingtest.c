#include "toolingtest.h"
#include "rs485.h"
#include "dts6012m.h"
#include "nd06av1c_app.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "nd06av1c_dev.h"
#include "string.h"

void initTooling()
{
	//TOOLING_USART1_UART_Init();
	TOOLING_USART2_UART_Init();
}

void TOOLING_USART1_UART_Init(void)
{

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 125000;
	huart1.Init.WordLength = UART_WORDLENGTH_9B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_EVEN;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	huart1.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

void TOOLING_USART2_UART_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 125000;
	huart2.Init.WordLength = UART_WORDLENGTH_9B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_EVEN;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	huart2.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

uint8_t testDTS()
{
	DTS6012_DATA dts6012DataTooling = {0, 0, 0, 0};
	
	dts6012_start();
	dts6012_start();
	dts6012_start();

	dts6012DataTooling.firstPeakDistance = dts6012_DMAdata->firstPeakDistance;
	dts6012DataTooling.firstPeakAmp = dts6012_DMAdata->firstPeakAmp;
	dts6012DataTooling.secondPeakDistance = dts6012_DMAdata->secondPeakDistance;
	dts6012DataTooling.secondPeakAmp = dts6012_DMAdata->secondPeakAmp;

	if (dts6012DataTooling.firstPeakDistance == 0)
	{
		return 0;
	}

	resultUp[8] = (dts6012DataTooling.firstPeakDistance >> 0) & 0xFF;
	resultUp[9] = (dts6012DataTooling.firstPeakDistance >> 8) & 0xFF;

	resultUp[10] = (dts6012DataTooling.secondPeakDistance >> 0) & 0xFF;
	resultUp[11] = (dts6012DataTooling.secondPeakDistance >> 8) & 0xFF;
	dts6012m_Stop();
	
	return 1;
}

ND06_DATA ND06DataTooling = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
uint16_t tryTimesToolingN = 10;
uint8_t testND06()
{
	uint32_t ND06ValueSum = 0;
	uint16_t ND06EvenValue = 0;
	uint16_t ND06ValueMax = 0;
	uint16_t ND06ValueMin = 65300;
	int i = 0;
	int j = 0;
	uint8_t ret = 0;
	
	while (tryTimesToolingN > 0)
	{
		tryTimesToolingN++;
		ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)ND06DataTooling.amp, (uint16_t *)ND06DataTooling.dep);
		if (ret == ND06AV1C_GET_DATA_SUCCESS)
		{
			break;
		}
	}

	if (ret == ND06AV1C_GET_DATA_SUCCESS)
	{
		uint8_t sum_cnt = 0;
		for (i = 2; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if ((0 < ND06DataTooling.dep[i * 4 + j]) && (ND06DataTooling.dep[i * 4 + j] < 65000))
				{
					if (ND06DataTooling.dep[i * 4 + j] > ND06ValueMax)
					{
						ND06ValueMax = ND06DataTooling.dep[i * 4 + j];
					}
					if (ND06DataTooling.dep[i * 4 + j] < ND06ValueMin)
					{
						ND06ValueMin = ND06DataTooling.dep[i * 4 + j];
					}

					ND06ValueSum = ND06ValueSum + ND06DataTooling.dep[i * 4 + j];
					sum_cnt++;
				}
			}
		}

		ND06EvenValue = ND06ValueSum / sum_cnt;
	}

	resultUp[17] = (ND06EvenValue >> 0) & 0xFF;
	resultUp[18] = (ND06EvenValue >> 8) & 0xFF;

	resultUp[13] = (ND06EvenValue >> 0) & 0xFF;
	resultUp[14] = (ND06EvenValue >> 8) & 0xFF;

	resultUp[15] = (ND06EvenValue >> 0) & 0xFF;
	resultUp[16] = (ND06EvenValue >> 8) & 0xFF;

	return 1;
}

void testIO()
{

	HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET);

	return;
}

void testInX()
{
	if (getIN_IO == 0)
	{
			if (HAL_GPIO_ReadPin(IO_IN_GPIO_Port, IO_IN_Pin) == GPIO_PIN_SET)
			{
				getIN_IO = 1;
			}
			else
			{
				getIN_IO = 0;
			}
	}
}
