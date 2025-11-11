#ifndef __DTS6012M_H__
#define __DTS6012M_H__

#include "usart.h"


typedef struct 
{
	uint16_t firstPeakDistance;
	uint16_t firstPeakAmp;
	uint16_t secondPeakDistance;
	uint16_t secondPeakAmp;
	uint16_t objDetectFlag;
}DTS6012_DATA;



void dts6012_start(void);
void dts6012m_Stop(void);
HAL_StatusTypeDef dts6012_getDepthAndAmp(DTS6012_DATA *dts6012_data);

extern DTS6012_DATA dts6012_DMAdata[];

#endif