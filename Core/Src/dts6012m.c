#include "dts6012m.h"
#include "rs485.h"
#include "sensorAppLogic.h"

/*dts6012m*/
uint8_t dts6012m_start_cmd[] = {0xA5, 0x03, 0x20, 0x01, 0x00, 0x00, 0x00, 0x02, 0x6E};
void dts6012_start(void)
{
	HAL_UART_Transmit(&huart3, dts6012m_start_cmd, sizeof(dts6012m_start_cmd), 10);
}

/*dts6012m*/
uint8_t dts6012m_stop_cmd[] = {0xA5, 0x03, 0x20, 0x02, 0x00, 0x00, 0x00, 0x46, 0x6E};
void dts6012m_Stop(void)
{
	HAL_UART_Transmit(&huart3, dts6012m_stop_cmd, sizeof(dts6012m_stop_cmd), 10);
}

uint8_t dTS6012M_Rdata[23] = {0};
uint16_t dTS6012M_errCnt = 0;
uint16_t dTS6012M_errFlag = 0;

HAL_StatusTypeDef dts6012_getDepthAndAmp(DTS6012_DATA *dts6012_data)
{
	HAL_StatusTypeDef status = HAL_OK;
	if ((dts6012_DMAdata->firstPeakDistance == 0) || (dts6012_DMAdata->secondPeakDistance == 0))
	{
		status = HAL_ERROR;
		return status;
	}
	dts6012_data->firstPeakDistance = dts6012_DMAdata->firstPeakDistance;
	dts6012_data->firstPeakAmp = dts6012_DMAdata->firstPeakAmp;
	dts6012_data->secondPeakDistance = dts6012_DMAdata->secondPeakDistance;
	dts6012_data->secondPeakAmp = dts6012_DMAdata->secondPeakAmp;
	//	if(HAL_UART_Receive(&huart3,dTS6012M_Rdata,23,3) == HAL_OK)
	//	{
	//		if((dTS6012M_Rdata[0]==0xA5)&&(dTS6012M_Rdata[1]==0x03)&&(dTS6012M_Rdata[2]==0x20))
	//		{
	//			dts6012_data->firstPeakDistance = (dTS6012M_Rdata[14] << 8) | (dTS6012M_Rdata[13]);
	//			dts6012_data->firstPeakAmp = (dTS6012M_Rdata[18] << 8) | (dTS6012M_Rdata[17]);
	//			dts6012_data->secondPeakDistance = (dTS6012M_Rdata[8] << 8) | (dTS6012M_Rdata[7]);
	//			dts6012_data->secondPeakAmp = (dTS6012M_Rdata[12] << 8) | (dTS6012M_Rdata[11]);
	//			dTS6012M_errCnt = 0;
	// #if (DebugPrintfFlag == 1)
	//			printf("dts6012_data.firstPeakDistance = %d\r\n", dts6012_data->firstPeakDistance);
	//			printf("dts6012_data.firstPeakAmp = %d\r\n", dts6012_data->firstPeakAmp);
	//			printf("dts6012_data.secondPeakDistance = %d\r\n", dts6012_data->secondPeakDistance);
	//			printf("dts6012_data.secondPeakAmp = %d\r\n", dts6012_data->secondPeakAmp);
	// #endif
	//		}
	//		else
	//		{
	//			if (++dTS6012M_errCnt >= 10)
	//			{
	//				dTS6012M_errFlag = 1;
	//				status = HAL_ERROR;
	//				E002 = 1;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if (++dTS6012M_errCnt >= 10)
	//		{
	//			dTS6012M_errFlag = 1;
	//			status = HAL_ERROR;
	//		}
	//	}

	return status;
}
