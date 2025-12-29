#include "sensorAppLogic.h"

#include "PDA_Communication.h"
#include "elevatorSignalMonitor.h"
#include "gpio.h"
#include "main.h"
#include "productToolingInspection.h"
#include "rs485.h"
#include "sensorParaTable.h"
#include "string.h"

#include "nd06av1c_comm.h"
#include "nd06av1c_data.h"
#include "nd06av1c_def.h"
#include "nd06av1c_dev.h"
#include "platform.h"

ERR_DATA ErrData = {0};
ND06_DATA nd06_data = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
ND06_DATA nd06_DLdata = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
DTS6012_DATA dts6012_data = {0, 0, 0, 0};

uint32_t DistanceThresholdLearningSuccessFlg = 0;
uint32_t dts6012DistancdLearnValue = 0;
uint32_t dts6012DistanceThreshold = 500;

uint32_t nd06DistancdLearnValue = 0;
uint32_t nd06DistanceThreshold = 500;
uint32_t cargoLift_nd06LearnValue[16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
uint32_t dts6012DistanceThresholdLearningSuccessFlg = 0;
uint32_t nd06DistanceThresholdLearningSuccessFlg = 0;

uint32_t objectDCnt_P = 0;
uint32_t objectDCnt_H = 0;

uint8_t IO_dts6012 = 0;
uint8_t IO_ND06 = 0;
uint8_t fingerDetected_flag = 0;
uint8_t regionalObjectDetected_flag_ND06 = 0;
uint8_t masterElevator_OF = 1;
uint8_t now_DistanceThresholdLearning_Flg = 0;
uint8_t DistanceThresholdLearningFailed_Flg = 0;
uint8_t times_DistanceThresholdLearn = 0;

uint8_t DistanceThresholdLearning(void)
{
    dts6012DistancdLearnValue = 0;

    uint8_t dtsLearnTimes = 0;
    uint8_t ndLearnTimes = 0;

    HAL_StatusTypeDef status = HAL_OK;

    while (dtsLearnTimes < 5) // (dts6012DistanceThresholdLearningSuccessFlg == 0) &&
    {
        dtsLearnTimes++;

        status = dts6012_getDepthAndAmp(&dts6012_data);
        if (status == HAL_OK)
        {
            if ((dts6012_data.firstPeakDistance > PARA_TABLE_USE.data.dts6012MinChkDistance))
            {
                // nd06DistancdLearnValue  dts6012DistancdLearnValue
                dts6012DistancdLearnValue += dts6012_data.firstPeakDistance;
                //                dts6012DistanceThresholdLearningSuccessFlg = 1;
                break;
            }
        }
    }
    if (dts6012DistancdLearnValue != 0)
    {
        dts6012DistanceThresholdLearningSuccessFlg = 1;
        dts6012DistancdLearnValue = dts6012DistancdLearnValue / dtsLearnTimes;
    }

    int32_t ret = ND06AV1C_ERROR_NONE;

    int sum_cnt = 0;

    while ((nd06DistanceThresholdLearningSuccessFlg == 0) && (ndLearnTimes < 255))
    {
        uint32_t nd06PixelValueSum = 0;
        ndLearnTimes++;

        ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_DLdata.amp, (uint16_t *)nd06_DLdata.dep);
        int i = 0;
        int j = 0;
        if (ret == ND06AV1C_GET_DATA_SUCCESS)
        {

            for (i = 2; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    if (nd06_DLdata.dep[i * 4 + j] < 65300)
                    {
                        cargoLift_nd06LearnValue[i * 4 + j] = nd06_DLdata.dep[i * 4 + j];
                        nd06PixelValueSum = nd06PixelValueSum + nd06_DLdata.dep[i * 4 + j];
                        sum_cnt++;
                    }
                }
            }

            if (sum_cnt > 0 && (nd06PixelValueSum > PARA_TABLE_USE.data.nd06MinChkDistance)) // 防止除零错误
            {
                nd06DistancdLearnValue = nd06PixelValueSum / sum_cnt;

                nd06DistanceThresholdLearningSuccessFlg = 1;
                DistanceThresholdLearningFailed_Flg = 0;
                break;
            }
        }
    }

    if ((dts6012DistanceThresholdLearningSuccessFlg == 1) && (nd06DistanceThresholdLearningSuccessFlg == 1))
    {
        DistanceThresholdLearningSuccessFlg = 1;
        now_DistanceThresholdLearning_Flg = 0;
        times_DistanceThresholdLearn = 0;
        dts6012DistanceThresholdLearningSuccessFlg = 0;
        nd06DistanceThresholdLearningSuccessFlg = 0;

        if (PARA_TABLE_USE.data.dts6012StudyDistance != dts6012DistancdLearnValue)
        {
            PARA_TABLE_USE.data.dts6012StudyDistance = dts6012DistancdLearnValue;
        }

        if (PARA_TABLE_USE.data.nd06StudyDistance != nd06DistancdLearnValue)
        {
            PARA_TABLE_USE.data.nd06StudyDistance = nd06DistancdLearnValue;
        }
        paraTable_Write();
        //        sensor_status = 6;
        return 1;
    }
    /*
        if ((dts6012DistanceThresholdLearningSuccessFlg == 1) && (nd06DistanceThresholdLearningSuccessFlg == 1))
        {
            DistanceThresholdLearningSuccessFlg = 1;
            now_DistanceThresholdLearning_Flg = 0;
            times_DistanceThresholdLearn = 0;
            dts6012DistanceThresholdLearningSuccessFlg = 0;
            nd06DistanceThresholdLearningSuccessFlg = 0;

            if (PARA_TABLE_USE.data.dts6012StudyDistance != dts6012DistancdLearnValue)
            {
                // PARA_TABLE_USE.data.dts6012StudyDistance = dts6012DistancdLearnValue;
            }

            if (PARA_TABLE_USE.data.nd06StudyDistance != nd06DistancdLearnValue)
            {
                // PARA_TABLE_USE.data.nd06StudyDistance = nd06DistancdLearnValue;
            }
            sensor_status = 6;
            return 1;
        }
    */
    return 0;
}

uint32_t nd06PixelAddCnt[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t nd06StudyDep[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t nd06AddCnt = 0;
uint32_t dts6012AddCnt = 0;
uint32_t nd06StudyDepEven = 0;
void cargolift_DistanceThresholdLearning(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint32_t nd06PixelValueSum = 0;
    int i = 0;
    int j = 0;
    int sum_cnt = 0;
    if (dts6012DistanceThresholdLearningSuccessFlg == 0)
    {
        if (dts6012AddCnt < 10)
        {
            status = dts6012_getDepthAndAmp(&dts6012_data);
            if (status == HAL_OK)
            {
                if ((dts6012_data.firstPeakDistance >= PARA_TABLE_USE.data.cargoLift_dts6012MinChkDistance) &&
                    (dts6012_data.firstPeakDistance < PARA_TABLE_USE.data.cargoLift_dts6012MaxChkDistance))
                {
                    dts6012DistancdLearnValue += dts6012_data.firstPeakDistance;
                    dts6012AddCnt++;
                }
            }
        }
        else
        {
            dts6012DistancdLearnValue = dts6012DistancdLearnValue / dts6012AddCnt;
            PARA_TABLE_USE.data.cargoLift_dts6012StudyDistance = dts6012DistancdLearnValue;
            paraTable_Write();
            dts6012DistanceThresholdLearningSuccessFlg = 1;
            dts6012DistancdLearnValue = 0;
            dts6012AddCnt = 0;
        }
    }

    /*TOF*/
    if (nd06DistanceThresholdLearningSuccessFlg == 0)
    {
        if (nd06AddCnt < 10)
        {
            ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_data.amp, (uint16_t *)nd06_data.dep);

            if (ret == ND06AV1C_GET_DATA_SUCCESS)
            {
                for (i = 0; i < 4; i++)
                {
                    for (j = 0; j < 4; j++)
                    {
                        if ((nd06_data.dep[i * 4 + j] >= PARA_TABLE_USE.data.cargoLift_nd06MinChkDistance) &&
                            (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.cargoLift_dts6012MaxChkDistance))
                        {
                            nd06PixelAddCnt[i * 4 + j]++;
                            nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j] + nd06_data.dep[i * 4 + j];
                        }
                    }
                }
                nd06AddCnt++;
            }
            else if (ret == ND06AV1C_ERROR_NONE)
            {
                // printf("do nothing!\r\n");
            }
            else
            {
                // printf("nd06av1c get dataInfo fail!\r\n");
            }
        }
        else //
        {
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    if (nd06PixelAddCnt[i * 4 + j] != 0)
                        nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j] / nd06PixelAddCnt[i * 4 + j];
                }
            }

            j = 1;
            for (i = 0; i < 4; i++)
            {
                if (nd06PixelAddCnt[i * 4 + j] == 0)
                {
                    nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j + 1];
                }
            }

            j = 2;
            for (i = 0; i < 4; i++)
            {
                if (nd06PixelAddCnt[i * 4 + j] == 0)
                {
                    nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j - 1];
                }
            }
            j = 0;
            for (i = 0; i < 4; i++)
            {
                if (nd06PixelAddCnt[i * 4 + j] == 0)
                {
                    nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j + 1];
                }
            }
            j = 3;
            for (i = 0; i < 4; i++)
            {
                if (nd06PixelAddCnt[i * 4 + j] == 0)
                {
                    nd06StudyDep[i * 4 + j] = nd06StudyDep[i * 4 + j - 1];
                }
            }

            for (uint8_t indexI = 0; indexI < 16; indexI++)
            {
                nd06StudyDepEven += nd06StudyDep[indexI];
            }
            nd06StudyDepEven /= 16;

            memcpy(PARA_TABLE_USE.data.cargoLift_nd06StudyPixelDistance, nd06StudyDep, sizeof(nd06StudyDep));
            paraTable_Write();
            nd06DistanceThresholdLearningSuccessFlg = 1;

            memset(nd06StudyDep, 0, sizeof(nd06StudyDep));
            memset(nd06PixelAddCnt, 0, sizeof(nd06PixelAddCnt));
            nd06AddCnt = 0;
        }
    }
    if ((dts6012DistanceThresholdLearningSuccessFlg == 1) && (nd06DistanceThresholdLearningSuccessFlg == 1))
    {
        DistanceThresholdLearningSuccessFlg = 1;
    }
}

uint32_t ClosingTimeLearningSuccessFlg = 0;
uint32_t closeTimeCnt = 0;
uint32_t closeTimeCountFlg = 0;
uint32_t pixelOccludedFlag = 0;
uint8_t now_ClosingTimeLearning_Flg = 0;
uint8_t times_ClosingTimeLearning = 0;
uint8_t ClosingTimeLearningFailed_Flg = 0;
uint16_t closeTimeCycle = 0;
// uint32_t closeTimeCnt = 0;
void ClosingTimeLearning()
{
    static uint32_t startTick = 0;
    static uint8_t learningStarted = 0;
    int32_t ret = ND06AV1C_ERROR_NONE;
    int i = 0, j = 0;

    now_ClosingTimeLearning_Flg = 1;

    // 1. 等待门打开状态消失，记录起始时间
    if (!learningStarted && currentDoorState == STATE_CLOSING && pastDoorState == STATE_OPENED)
    {
        learningStarted = 1;
        pixelOccludedFlag = 0;
    }

    // 2. 只有在学习已开始时才继续
    if (learningStarted)
    {
        closeTimeCycle++;
        ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_data.amp, (uint16_t *)nd06_data.dep);
        if (ret == ND06AV1C_GET_DATA_SUCCESS)
        {
            for (i = 2; i < 4; i++) // 1
            {
                for (j = 0; j < 4; j++) // 1
                {
                    // if (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold)
                    if ((nd06_data.dep[i * 4 + j] >= PARA_TABLE_USE.data.nd06MinChkDistance) &&
                        (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold))
                    {
                        pixelOccludedFlag = 1;
                        learningStarted = 0;
                        break;
                    }
                }
                //                if (pixelOccludedFlag)
                //                    break;
            }
        }
    }
    // 3. 检测到像素遮挡，记录终止时间
    if (pixelOccludedFlag == 1)
    {
        // closeTimeCnt = HAL_GetTick() - startTick;
        closeTimeCnt = (closeTimeCycle - 100) * 5;
        ClosingTimeLearningSuccessFlg = 1;
        now_ClosingTimeLearning_Flg = 0;
        times_ClosingTimeLearning = 0;
        pixelOccludedFlag = 0;
        learningStarted = 0;

        // 保存学习结果
        if (PARA_TABLE_USE.data.closingDoorTime != closeTimeCnt)
        {
            PARA_TABLE_USE.data.closingDoorTime = closeTimeCnt;
            paraTable_Write();
        }
    }
}

uint8_t ObjectIsDetectedFlag = 0;
int8_t nd06AV1C_objDetectFlag = 0;
uint8_t prevFlag = 0;
uint8_t timesND06Failed = 0;
void ObjectDetection(void)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t num_of_pixel_occluded = 0;
    uint16_t current_exp = 0;

    dts6012_getDepthAndAmp(&dts6012_data);
    if ((dts6012_data.firstPeakDistance < (PARA_TABLE_USE.data.dts6012StudyDistance - PARA_TABLE_USE.data.dts6012DistanceChkThreshold)) && (dts6012_data.firstPeakDistance > PARA_TABLE_USE.data.dts6012MinChkDistance) && (ds6012OutputEN == 1))
    {
        dts6012_data.objDetectFlag = 1;
    }
    else if ((dts6012_data.secondPeakDistance < (PARA_TABLE_USE.data.dts6012StudyDistance - PARA_TABLE_USE.data.dts6012DistanceChkThreshold)) && (dts6012_data.secondPeakDistance > PARA_TABLE_USE.data.dts6012MinChkDistance) && (ds6012OutputEN == 1))
    {
        dts6012_data.objDetectFlag = 1;
    }
    else
    {
        dts6012_data.objDetectFlag = 0;
    }

    ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_data.amp, (uint16_t *)nd06_data.dep);

    // ret = ND06AV1C_ERROR_NONE;
    if (ret == ND06AV1C_GET_DATA_SUCCESS)
    {
        E001 = 0;
        timesND06Failed = 0;
        num_of_pixel_occluded = 0;
        for (i = 2; i < 4; i++) // i = 2;i < 4;i ++
        {
            for (j = 0; j < 4; j++) // j = 0;j < 4;j ++
            {
                //  printf("%5d\t",dep[i*4+j]);
                if ((nd06_data.dep[i * 4 + j] >= PARA_TABLE_USE.data.nd06MinChkDistance) &&
                    (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.nd06StudyDistance - PARA_TABLE_USE.data.nd06DistanceChkThreshold))
                    num_of_pixel_occluded++;
            }
        }
        if ((num_of_pixel_occluded >= 1) && (nd06OutputEN == 1))
        {
            nd06AV1C_objDetectFlag = 1;
        }
        else
        {
            nd06AV1C_objDetectFlag = 0;
        }

#if (DebugPrintfFlag == 1)
        {
            printf("amp:\r\n");
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    printf("%4d\t", nd06_data.amp[i * 4 + j]);
                }

                printf("\r\n");
            }

            printf("\r\ndep:\r\n");
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    printf("%5d\t", nd06_data.dep[i * 4 + j]);
                }

                printf("\r\n");
            }
        }
#endif
    }
    else if (ret == ND06AV1C_ERROR_NONE)
    {

        // printf("do nothing!\r\n");
    }
    else
    {
        if (timesND06Failed++ > 3)
        {
            E001 = 1;
            timesND06Failed = 0;
            ND06_Reset();
            ND06AV1C_Init();

#if (DebugPrintfFlag == 1)
            printf("nd06av1c get dataInfo fail!\r\n");
#endif
        }
    }

    if ((dts6012_data.objDetectFlag == 1) || (nd06AV1C_objDetectFlag == 1))
    {
        ObjectIsDetectedFlag = 1;
        if (prevFlag == 0)
        {
            if (objectDCnt_P < UINT32_MAX)
            {
                objectDCnt_P++;
            }
            else
            {
                objectDCnt_P = 1;
            }

            prevFlag = 1;
        }
    }
    else
    {
        ObjectIsDetectedFlag = 0;
        prevFlag = 0;
    }

    if (dts6012_data.objDetectFlag == 1)
    {
        IO_dts6012 = 1;
        if (nd06OutputEN == 0)
        {
            fingerDetected_flag = 1;
        }
        else
        {
            fingerDetected_flag = 0;
        }
    }
    else
    {
        IO_dts6012 = 0;
    }

    if (nd06AV1C_objDetectFlag == 1)
    {
        IO_ND06 = 1;
        regionalObjectDetected_flag_ND06 = 1;
    }
    else
    {
        IO_ND06 = 0;
        regionalObjectDetected_flag_ND06 = 0;
    }
}

void cargoLift_ObjectDetection(void)
{
    int32_t ret = ND06AV1C_ERROR_NONE;
    //  uint16_t amp[16];
    //  uint16_t dep[16];
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t num_of_pixel_occluded = 0;
    uint16_t current_exp = 0;
    static uint16_t IOoutDelayCnt = 0;
    static uint16_t IOoutDelayFlag = 0;
    static uint16_t nd06AV1C_objDetectFlagOLD = 0;
    static uint16_t dts6012ObjDetectFlagOLD = 0;
    nd06AV1C_objDetectFlagOLD = nd06AV1C_objDetectFlag;
    dts6012ObjDetectFlagOLD = dts6012_data.objDetectFlag;

    /*TOF*/
    dts6012_getDepthAndAmp(&dts6012_data);
    if ((dts6012_data.firstPeakDistance < (PARA_TABLE_USE.data.cargoLift_dts6012StudyDistance - PARA_TABLE_USE.data.cargoLift_dts6012DistanceChkThreshold)) &&
        (dts6012_data.firstPeakDistance > PARA_TABLE_USE.data.cargoLift_dts6012MinChkDistance))
    {
        dts6012_data.objDetectFlag = 1;
    }
    else if ((dts6012_data.secondPeakDistance < (PARA_TABLE_USE.data.cargoLift_dts6012StudyDistance - PARA_TABLE_USE.data.cargoLift_dts6012DistanceChkThreshold)) &&
             (dts6012_data.secondPeakDistance > PARA_TABLE_USE.data.cargoLift_dts6012MinChkDistance))
    {
        dts6012_data.objDetectFlag = 1;
    }
    else
    {
        dts6012_data.objDetectFlag = 0;
    }

    /*����TOF*/

    ret = ND06AV1C_GetDepthAndAmpData(&g_nd06av1c_device, (uint16_t *)nd06_data.amp, (uint16_t *)nd06_data.dep);
    // ret = ND06AV1C_ERROR_NONE;
    if (ret == ND06AV1C_GET_DATA_SUCCESS)
    {
        timesND06Failed = 0;
        //			if(timesND06Failed++ > 10){
        //						ND06_Reset();    // ND06
        //            ND06AV1C_Init(); // ND06
        //				}
        E001 = 0;
        num_of_pixel_occluded = 0;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++) // 像素左右与实际门的左右相反
            {
                if ((nd06_data.dep[i * 4 + j] >= PARA_TABLE_USE.data.cargoLift_nd06MinChkDistance) &&
                    (nd06_data.dep[i * 4 + j] <= PARA_TABLE_USE.data.cargoLift_nd06StudyPixelDistance[i * 4 + j] - PARA_TABLE_USE.data.cargoLift_nd06DistanceChkThreshold))
                {
                    num_of_pixel_occluded++;
                    break;
                }
            }
        }
        if (num_of_pixel_occluded >= 1)
        {
            nd06AV1C_objDetectFlag = 1;
        }
        else
            nd06AV1C_objDetectFlag = 0;

#if (DebugPrintfFlag == 1)
        {
            printf("amp:\r\n");
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    printf("%4d\t", nd06_data.amp[i * 4 + j]);
                }

                printf("\r\n");
            }

            printf("\r\ndep:\r\n");
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    printf("%5d\t", nd06_data.dep[i * 4 + j]);
                }

                printf("\r\n");
            }
        }
#endif
    }
    else if (ret == ND06AV1C_ERROR_NONE)
    {
        // printf("do nothing!\r\n");
    }
    else
    {
        if (timesND06Failed++ > 3) // ����3��ʧND06
        {
            E001 = 1;
            timesND06Failed = 0;
            ND06_Reset();    // ND06
            ND06AV1C_Init(); // ND06
                             // HAL_NVIC_SystemReset();
        }

#if (DebugPrintfFlag == 1)
        printf("nd06av1c get dataInfo fail!\r\n");
#endif
    }

    if (((nd06AV1C_objDetectFlag == 0) && (nd06AV1C_objDetectFlagOLD == 1)) || ((dts6012_data.objDetectFlag == 0) && (dts6012ObjDetectFlagOLD == 1)))
    {
        IOoutDelayFlag = 1;
    }

    if ((dts6012_data.objDetectFlag == 1) || (nd06AV1C_objDetectFlag == 1))
    {
        ObjectIsDetectedFlag = 1;
        if (prevFlag == 0)
        {
            if (objectDCnt_H < UINT32_MAX)
            {
                objectDCnt_H++;
            }
            else
            {
                objectDCnt_H = 0;
            }
            prevFlag = 1;
        }
    }
    else if (IOoutDelayFlag == 1)
    {

        if (IOoutDelayCnt++ >= 200)
        {
            ObjectIsDetectedFlag = 0;
            IOoutDelayCnt = 0;
            IOoutDelayFlag = 0;
        }
        else
            ObjectIsDetectedFlag = 1;
    }
    else
    {
        ObjectIsDetectedFlag = 0;
        prevFlag = 0;
    }
}

uint32_t sensor_status = NormalWorking_STATUS;
uint32_t sensor_old_status = Unknown_STATUS;
void sensor_status_control(void)
{
    sensor_old_status = sensor_status;
    if (ToolingTestFlag != 1)
    {
        sensor_status = NotDetected_STATUS;
    }
    // 2025/11/10
    //    if (ERR_D != 0)
    //    {
    //        //        sensor_status = Fault_STATUS;

    //        //        if (sensor_self_check())
    //        //        {
    //        //            ERR_D = 0;
    //        //            sensor_status = NormalWorking_STATUS;
    //        //        }
    //        //        return; // ERR_D = 0;
    //    }
    //    else
    //    {
    //        sensor_status = NormalWorking_STATUS;
    //    }

    if ((sensor_status == NormalWorking_STATUS) && (IdleFlg == 1))
    {
        sensor_status = Idle_STATUS;
    }
    else if ((sensor_status == Idle_STATUS) && (IdleFlg == 0))
    {
        sensor_status = NormalWorking_STATUS;
    }
//		else if((sensor_status == NormalWorking_STATUS) && (ERR_D != 0))
//		{
//			sensor_status = Fault_STATUS;
//		}
    else
    {
        // sensor_status = NormalWorking_STATUS;
        sensor_status = sensor_status;
    }
}

int sensor_self_check(void)
{
    // HAL_NVIC_SystemReset();
    if (E001 == 1)
    {
        HAL_NVIC_SystemReset();
    }
    else if (E007 == 1)
    {
        if (currentDoorState == STATE_CLOSED || currentDoorState == STATE_OPENED)
        {
            if (dts6012_getDepthAndAmp(&dts6012_data) == HAL_OK)
            {
                // nd06DistancdLearnValue  dts6012DistancdLearnValue
                uint8_t diff_distance = dts6012DistancdLearnValue > dts6012_data.firstPeakDistance ? 
																				(dts6012DistancdLearnValue - dts6012_data.firstPeakDistance) : 
																				(dts6012_data.firstPeakDistance - dts6012DistancdLearnValue);
                if (diff_distance < 50)
                {
                    E007 = 0;
                }
            }
        }
    }
    else if (E008 == 1)
    {
    }

    if (ERR_D == 0)
    {
        return 1;
    }
    return 0;
}

void cargoLift_sensor_status_control(void)
{
    sensor_old_status = sensor_status;
    if (ToolingTestFlag != 1)
    {
        sensor_status = NotDetected_STATUS;
    }
		/*
    else if (ERR_D != 0)
    {
        sensor_status = Fault_STATUS;
        HAL_NVIC_SystemReset();
        //        ERR_D = 0;
        //        sensor_status = NormalWorking_STATUS;
    }
		*/
    else if (((sensor_status == NormalWorking_STATUS) ||
              (sensor_status == Idle_STATUS)) &&
             (DistanceThresholdLearningReqFlg == 1))
    {
        sensor_status = DistanceThresholdLearning_STATUS;
        // DistanceThresholdLearningReqFlg = 0;
    }
    else if ((sensor_status == DistanceThresholdLearning_STATUS) &&
             (DistanceThresholdLearningSuccessFlg == 1))
    {
        sensor_status = NormalWorking_STATUS;
        DistanceThresholdLearningSuccessFlg = 0;
        dts6012DistanceThresholdLearningSuccessFlg = 0;
        nd06DistanceThresholdLearningSuccessFlg = 0;
    }
    else if ((sensor_status == NormalWorking_STATUS) && (IdleFlg == 1))
    {
        sensor_status = Idle_STATUS;
    }
    else if ((sensor_status == Idle_STATUS) && (IdleFlg == 0))
    {
        sensor_status = NormalWorking_STATUS;
    }
    else
    {
        // sensor_status = NormalWorking_STATUS;
        sensor_status = sensor_status;
    }

    if ((sensor_old_status != sensor_status) &&
        ((sensor_status == NormalWorking_STATUS) ||
         (sensor_status == DistanceThresholdLearning_STATUS)))
    {
        dts6012_start();
        // nd06av1c_Start();
    }
    else if ((sensor_old_status != sensor_status) &&
             ((sensor_status == NotDetected_STATUS) ||
              (sensor_status == Fault_STATUS)))
    {
        dts6012m_Stop();
        //  nd06av1c_Stop();
    }
    else
        ;
}

/*
#define Unknown_STATUS                                         -1
#define NotDetected_STATUS                                      0
#define NormalWorking_STATUS                                    1
#define DistanceThresholdLearning_STATUS                        2
#define ClosingTimeLearning_STATUS                              3
#define Idle_STATUS                                             4
#define Fault_STATUS                                            5
*/
uint8_t objectDetectionErrorCnt = 0;
uint8_t stopIOOutputFlag = 0;
/*
according to sensor_status to do special works
*/
uint8_t closingNoObjectCnt = 0;
void sensor_action_control(void)
{
    if (sensor_status == NotDetected_STATUS)
    {
        LEDON;
    }
    else if (sensor_status == Fault_STATUS)
    {
        LEDON;
    }
    else if (sensor_status == DistanceThresholdLearning_STATUS /*&&(DistanceThresholdLearningSuccessFlg == 0)*/
             && DistanceThresholdLearningReqFlg == 1)
    {
        // currentDoorState == STATE_CLOSED

        // LEDTOGGLE;                   // LED
        /* if (currentDoorState != STATE_CLOSED)
         {
             HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET);

         }
         else */
        if ((currentDoorState == STATE_CLOSED) || (currentDoorState == STATE_OPENED))
        {
            DistanceThresholdLearning(); //
        }
        DistanceThresholdLearningReqFlg = 0;
    }
    else if (sensor_status == DistanceThresholdLearning_STATUS && ClosingTimeLearningReqFlg == 1)
    {
        LEDTOGGLE; // LED
        ClosingTimeLearningSuccessFlg = 0;
        ClosingTimeLearning(); //
                               // ClosingTimeLearningReqFlg = 0;
    }
    else if (sensor_status == NormalWorking_STATUS) //
    {
        ClosingTimeLearningReqFlg = 0;
        ObjectDetection();
        // ObjectIsDetectedFlag = 1;   //E003

        detectError();

        // IO
        if (stopIOOutputFlag == 1)
        {
            LEDOFF;
            HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET); // IO
        }
        else
        {

            if (ObjectIsDetectedFlag == 1)
            {
                LEDON;
                HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET); // IO
            }
            else
            {
                LEDOFF;
                HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET); // IO
            }
        }
    }
    else if (sensor_status == Idle_STATUS)
    {
        ClosingTimeLearningReqFlg = 0;
        LEDOFF;
    }
    else
    {


        ;
    }
}

uint16_t ledCnt = 0;
void cargoLift_sensor_action_control(void)
{
    if (sensor_status == NotDetected_STATUS)
    {
        // LEDON;
    }
    else if (sensor_status == Fault_STATUS)
    {
        // LEDON;
    }
    else if (sensor_status == DistanceThresholdLearning_STATUS)
    {

        if (ledCnt++ > 100)
        {
            ledCnt = 0;
            LEDTOGGLE;
        }
        cargolift_DistanceThresholdLearning();
        DistanceThresholdLearningReqFlg = 0;
    }
    else if (sensor_status == NormalWorking_STATUS && cargoLift_IN)
    {
        cargoLift_ObjectDetection();
        if (ObjectIsDetectedFlag == 1)
        {
            LEDON;
            HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET);
        }
        else
        {
            LEDOFF;
            HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);
        }
    }
    else
    {
        LEDOFF;
        HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);
    }

    //    else if(sensor_status == Idle_STATUS)
    //    {
    //		ClosingTimeLearningReqFlg = 0;
    //		LEDOFF;
    //    }
}

void ND06_Reset(void)
{
    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(ND06_EN_GPIO_Port, ND06_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
}

uint8_t detectError()
{
    if (ObjectIsDetectedFlag == 1)
    {

        // if (currentDoorState == STATE_CLOSING)
        closingNoObjectCnt = 0;
        stopIOOutputFlag = 0;
        E008 = 0;

        if (currentDoorState == STATE_OPENING && pastDoorState == STATE_CLOSING)
        {
            objectDetectionErrorCnt = 0;
            stopIOOutputFlag = 0;
            E007 = 0;

            pastDoorState = STATE_CLOSING;
        }
        else if (currentDoorState == STATE_OPENED && pastDoorState == STATE_CLOSING)
        {
            objectDetectionErrorCnt = 0;
            stopIOOutputFlag = 0;
            E007 = 0;

            pastDoorState = STATE_CLOSING;
        }
        else if (currentDoorState == STATE_CLOSED && pastDoorState == STATE_CLOSING)
        {
            pastDoorState = STATE_CLOSED;

            objectDetectionErrorCnt++;

            if (objectDetectionErrorCnt >= 10)
            {
                stopIOOutputFlag = 1;
                E007= 1;
            }
            else
            {
                stopIOOutputFlag = 0;
                E007 = 0;
            }
        }
    }
    else
    {
        objectDetectionErrorCnt = 0;
        stopIOOutputFlag = 0;

        if (currentDoorState == STATE_CLOSED && pastDoorState == STATE_CLOSING)
        {
            if (closingNoObjectCnt < 20)
            {
                closingNoObjectCnt++;
                pastDoorState = STATE_CLOSED;
            }

            if (closingNoObjectCnt >= 10)
            {
                E008 = 1;
                stopIOOutputFlag = 1;
                closingNoObjectCnt = 0;
            }
            else
            {
                stopIOOutputFlag = 0;
                E008 = 0;
            }
        }
    }

    return stopIOOutputFlag;
}
