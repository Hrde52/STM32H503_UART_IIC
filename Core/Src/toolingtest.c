#include "toolingtest.h"
#include "rs485.h"
#include "dts6012m.h"
#include "nd06av1c_app.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "nd06av1c_dev.h"

void initTooling()
{
	TOOLING_USART2_UART_Init();
}

void TOOLING_USART2_UART_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 38400;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
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

void testUART1()
{
	uint8_t test1[5] = {1, 0, 0, 0, 1};
	HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	test1[0] = 1;
	test1[4] = xor_checkSum(test1, 4);
	HAL_StatusTypeDef res = HAL_UART_Transmit(&huart1, test1, 5, 100);

	return;
}

void testUART2()
{
	uint8_t test1[5] = {1, 0, 0, 0, 1};
	HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	test1[0] = 1;
	test1[4] = xor_checkSum(test1, 4);
	HAL_StatusTypeDef res = HAL_UART_Transmit(&huart2, test1, 5, 100);

	return;
}

DTS6012_DATA dts6012DataTooling = {0, 0, 0, 0};
uint8_t dTS6012_Rdata[23] = {0};
uint8_t testDTS()
{

	dts6012_start();
	dts6012_start();
	dts6012_start();

	// HAL_StatusTypeDef status = dts6012_getDepthAndAmp(&dts6012DataTooling);
	HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, dTS6012_Rdata, 23, 20);
	if (status == HAL_OK)
	{
		if ((dTS6012_Rdata[0] == 0xA5) && (dTS6012_Rdata[1] == 0x03) && (dTS6012_Rdata[2] == 0x20))
		{
			dts6012DataTooling.firstPeakDistance = (dTS6012_Rdata[14] << 8) | (dTS6012_Rdata[13]);
			dts6012DataTooling.firstPeakAmp = (dTS6012_Rdata[18] << 8) | (dTS6012_Rdata[17]);
			dts6012DataTooling.secondPeakDistance = (dTS6012_Rdata[8] << 8) | (dTS6012_Rdata[7]);
			dts6012DataTooling.secondPeakAmp = (dTS6012_Rdata[12] << 8) | (dTS6012_Rdata[11]);
		}
		else if ((dTS6012_Rdata[1] == 0xA5) && (dTS6012_Rdata[2] == 0x03) && (dTS6012_Rdata[3] == 0x20))
		{
			dts6012DataTooling.firstPeakDistance = (dTS6012_Rdata[15] << 8) | (dTS6012_Rdata[14]);
			dts6012DataTooling.firstPeakAmp = (dTS6012_Rdata[19] << 8) | (dTS6012_Rdata[18]);
			dts6012DataTooling.secondPeakDistance = (dTS6012_Rdata[9] << 8) | (dTS6012_Rdata[8]);
			dts6012DataTooling.secondPeakAmp = (dTS6012_Rdata[13] << 8) | (dTS6012_Rdata[12]);
		}
	}
	// dts6012DataTooling.firstPeakDistance;
	// dts6012DataTooling.secondPeakDistance;
	if (status != HAL_OK || dts6012DataTooling.firstPeakDistance == 0)
	{
		return 0;
	}
	uint8_t test1[10] = {0};
	// dts6012DataTooling.firstPeakDistance;
	test1[1] = (dts6012DataTooling.firstPeakDistance >> 0) & 0xFF;
	test1[2] = (dts6012DataTooling.firstPeakDistance >> 8) & 0xFF;
	// dts6012DataTooling.secondPeakDistance
	test1[3] = (dts6012DataTooling.secondPeakDistance >> 0) & 0xFF;
	test1[4] = (dts6012DataTooling.secondPeakDistance >> 8) & 0xFF;

	HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);

	HAL_UART_Transmit(&huart1, test1, 10, 100);
	dts6012m_Stop();

	return 1;
}

ND06_DATA ND06DataTooling = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
uint16_t tryTimesToolingN = 10;
uint8_t testND06()
{
	// uint8_t tryTimesToolingN = 10;
	uint8_t ret = 0;
	while (tryTimesToolingN > 0)
	{
		// 657 658
		tryTimesToolingN++;
		ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)ND06DataTooling.amp, (uint16_t *)ND06DataTooling.dep);
		if (ret == ND06AV1C_GET_DATA_SUCCESS)
		{
			break;
		}
	}

	uint16_t ND06ValueSum, ND06EvenValue;
	uint16_t ND06ValueMax = 0;
	uint16_t ND06ValueMin = 65300;
	int i = 0;
	int j = 0;
	if (ret == ND06AV1C_GET_DATA_SUCCESS)
	{
		uint8_t sum_cnt = 0;
		for (i = 2; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (0 < ND06DataTooling.dep[i * 4 + j] && ND06DataTooling.dep[i * 4 + j] < 65300)
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

	uint8_t test1[10] = {0};

	test1[1] = (ND06ValueSum >> 0) & 0xFF;
	test1[2] = (ND06ValueSum >> 8) & 0xFF;

	test1[3] = (ND06ValueMin >> 0) & 0xFF;
	test1[4] = (ND06ValueMin >> 8) & 0xFF;

	test1[5] = (ND06ValueMax >> 0) & 0xFF;
	test1[6] = (ND06ValueMax >> 8) & 0xFF;

	HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);

	HAL_UART_Transmit(&huart1, test1, 10, 100);

	return 1;
}

void testIO()
{
	uint8_t timesIO = 3;
	while (timesIO > 0)
	{
		timesIO--;
		HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET);
	}
	return;
}

void transmitResult(UART_HandleTypeDef *huart, uint8_t *data, uint16_t len)
{
	if (huart == &huart1)
		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	// uint8_t test1[5] = {1,0,0,0,1};

	HAL_UART_Transmit(huart, data, len, 100);
}
