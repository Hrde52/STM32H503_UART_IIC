#include "elevatorSignalMonitor.h"
#include "main.h"
#include "rs485.h"
#include "sensorParaTable.h"

uint32_t IdleFlg = 0;
uint32_t ds6012OutputEN = 0;
uint32_t nd06OutputEN = 0;
uint32_t closingDoorTimeCnt = 0;
uint32_t closingDoorTimeCountFlg = 0;


DoorState currentDoorState = STATE_UNKNOWN;
void DetectInitialState(void)
{
    if (CSpara.CLS == 1)
    {
        currentDoorState = STATE_CLOSED; // 关门到位
    }
    else if (CSpara.OLS == 1)
    {
        currentDoorState = STATE_OPENED; // 开门到位
    }
    else if (CSpara.openDoorCmd == 1)
    {
        currentDoorState = STATE_OPENING; // 正在开门
    }
    else if (CSpara.closeDoorCmd == 1)
    {
        currentDoorState = STATE_CLOSING; // 正在关门
    }
    else
    {
        // 无法确定状态，默认关门（或报错）
        currentDoorState = STATE_CLOSED;
    }
}

uint32_t closingPhaseTimer = 0;
DoorState pastDoorState = STATE_UNKNOWN;
// 状态机更新函数
void UpdateDoorStateMachine(void)
{
    // 确保平层信号有效
    if (CSpara.LevelingSignal == 0 && masterElevator_LevelingSignal == 0)
    {
        IdleFlg = 1;
        return;
    }

    IdleFlg = 0;

    switch (currentDoorState)
    {
        // 如果状态未知，先检测初始状态
    case STATE_UNKNOWN:
        DetectInitialState();
        break;

    case STATE_CLOSED: // 关门到位状态
			pastDoorState = currentDoorState;
        if (CSpara.openDoorCmd /*&& CSpara.CLS*/)
        {           
            currentDoorState = STATE_OPENING;
        }
				
        break;

    case STATE_OPENING: // 开门过程中
			pastDoorState = currentDoorState;
        if (CSpara.OLS)
        {
            currentDoorState = STATE_OPENED;
        }
			  else if(CSpara.closeDoorCmd)
				{
					currentDoorState = STATE_CLOSING;
				}
				
        break;

    case STATE_OPENED: // 开门到位状态
			pastDoorState = currentDoorState;
        if (CSpara.closeDoorCmd)
        {
            currentDoorState = STATE_CLOSING;
            closingPhaseTimer = 0; // 重置关门过程计时器
        }
        break;

    case STATE_CLOSING: // 关门过程中
			pastDoorState = currentDoorState;
        if (CSpara.CLS)
        {
            currentDoorState = STATE_CLOSED;
        }
        else if (CSpara.openDoorCmd)
        {        
            currentDoorState = STATE_OPENING;
        }
        
        break;

    default:
        break;
    }
}

void ElevatorSignaMonitoring(void)
{
    switch (currentDoorState)
    {
    case STATE_CLOSED: 
        ds6012OutputEN = 1;
        nd06OutputEN = 1;   
        break;

    case STATE_OPENING: 
        ds6012OutputEN = 0;
        nd06OutputEN = 0;
        break;

    case STATE_OPENED: 
        ds6012OutputEN = 1;
        nd06OutputEN = 1;
        break;

    case STATE_CLOSING:                                           
        if (closingPhaseTimer++ < PARA_TABLE_USE.data.closingDoorTime / 10)    // 5ms周期检查 
        {
            ds6012OutputEN = 1;
            nd06OutputEN = 1;
        }
        else
        {
            ds6012OutputEN = 1;
            nd06OutputEN = 0;
        }
        
        break;

    default:
        break;
    }
}
