#include "nd06av1c_app.h"
#include "nd06av1c_comm.h"
#include "nd06av1c_dev.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "platform.h"


/* ND06AV1C模组设备结构体 */
ND06AV1C_Dev_t g_nd06av1c_device = {
    .i2c_dev_addr = ND06AV1C_DEFAULT_SLAVE_ADDR,
    .SetXShutPinLevelFunc = set_xshut_pin_level
};

/* 函数指针结构体 */
ND06AV1C_Func_Ptr_t dev_op = {NULL, NULL, NULL, NULL};




void ND06AV1C_Init(void)
{
	dev_op.Delay10usFunc = delay_10us;
	dev_op.Delay1msFunc = delay_1ms;
	dev_op.GetNowTimeUsFunc = get_now_time_us;     /* Note: 如果不进行频率标定则可以不实现get_now_time_us函数，此函数仅在对ND06AV1C模组频率进行标定时才使用 */
	dev_op.I2C_ReadNBytesFunc = i2c_read_nbytes;
	dev_op.I2C_WriteNBytesFunc = i2c_write_nbytes;
	ND06AV1C_RegisteredPlatformApi(dev_op);

	/* 循环等待设备启动, 若模组或者IIC读写函数有问题则会报错 */
	if(ND06AV1C_ERROR_NONE != ND06AV1C_WaitDeviceBootUp(&g_nd06av1c_device,ND06AV1C_BOOTFLAG_RUN))
	{
		;
		#if(DebugPrintfFlag == 1)
	    printf("ND06AV1C_WaitDeviceBootUp error\r\n");
		#endif
	    //return -1;
	}
	/* 初始化模组设备 */
	if(ND06AV1C_ERROR_NONE != ND06AV1C_InitDevice(&g_nd06av1c_device))
	{
		;
		#if(DebugPrintfFlag == 1)
	    printf("ND06AV1C_InitDevice error!!\r\n");
		#endif
	    //return -1;
	}
}



///**
// * @brief 单次数据获取
// *
// * @param pNxDevice
// */


//int32_t ND06AV1C_Ranging(struct ND06_DATA nd06_data)
//{
//    int32_t	 ret = ND06AV1C_ERROR_NONE;
//	uint8_t i,j;
//	uint16_t objDetectCnt = 0;
//	ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device,(uint16_t *)nd06_data.amp,(uint16_t *)nd06_data.dep);

//	if(ret == ND06AV1C_GET_DATA_SUCCESS)
//    {
//		objDetectCnt = 0;
//		for( i = 0;i < 4;i ++)
//        {
//            for( j = 0;j < 3;j ++)
//            {
//                //printf("%5d\t",dep[i*4+j]);
//				if(nd06_data.dep[i*4+j]<=2000)
//					objDetectCnt++;
//            }
//        }
//		if(objDetectCnt>=1)
//		{
//			nd06AV1C_objDetectFlag = 1;
//		}
//		else
//			nd06AV1C_objDetectFlag = 0;
//	    printf("amp:\r\n");
//        for( i = 0;i < 4;i ++)
//        {
//            for( j = 0;j < 4;j ++)
//            {
//                printf("%4d\t",nd06_data.amp[i*4+j]);
//            }

//            printf("\r\n");
//        }

//        printf("\r\ndep:\r\n");
//        for( i = 0;i < 4;i ++)
//        {
//            for( j = 0;j < 4;j ++)
//            {
//                printf("%5d\t",nd06_data.dep[i*4+j]);
//            }

//            printf("\r\n");
//        }

//    }
//    else if(ret == ND06AV1C_ERROR_NONE)
//    {
//		;
//    }
//    else
//    {
//		printf("nd06av1c get dataInfo fail!\r\n");
//    }
//	return ret;
//}










































