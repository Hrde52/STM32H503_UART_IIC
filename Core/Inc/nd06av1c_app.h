#ifndef __ND06AV1C_APP_H__
#define __ND06AV1C_APP_H__
#include "main.h"
#include "nd06av1c_def.h"

typedef struct 
{
	uint16_t amp[16];
	uint16_t dep[16];
}ND06_DATA;

//struct ND06_DATA nd06_data_default;



/* ND06AV1C模组设备结构体 */
extern ND06AV1C_Dev_t g_nd06av1c_device;





/* 初始化 */
void ND06AV1C_Init(void);

/* 单次数据获取 */
int32_t ND06AV1C_Ranging(ND06_DATA nd06_data);

































#endif
