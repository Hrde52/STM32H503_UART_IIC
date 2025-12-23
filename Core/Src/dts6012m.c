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
		E002 = 1;
		return status;
	}
	E002 = 0;
	dts6012_data->firstPeakDistance = dts6012_DMAdata->firstPeakDistance;
	dts6012_data->firstPeakAmp = dts6012_DMAdata->firstPeakAmp;
	dts6012_data->secondPeakDistance = dts6012_DMAdata->secondPeakDistance;
	dts6012_data->secondPeakAmp = dts6012_DMAdata->secondPeakAmp;

	return status;
}
