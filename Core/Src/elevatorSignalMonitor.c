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
        currentDoorState = STATE_CLOSED; // 
    }
    else if (CSpara.OLS == 1)
    {
        currentDoorState = STATE_OPENED; // 
    }
    else if (CSpara.openDoorCmd == 1)
    {
        currentDoorState = STATE_OPENING; // 
    }
    else if (CSpara.closeDoorCmd == 1)
    {
        currentDoorState = STATE_CLOSING; //
    }
    else
    {
        currentDoorState = STATE_CLOSED;
    }
}

uint32_t closingPhaseTimer = 0;
DoorState pastDoorState = STATE_UNKNOWN;
// ״̬
void UpdateDoorStateMachine(void)
{
    if (CSpara.LevelingSignal == 0 && masterElevator_LevelingSignal == 0)
    {
      //  IdleFlg = 1;
			IdleFlg = 0;
        return;
    }

    IdleFlg = 0;

    switch (currentDoorState)
    {
        
    case STATE_UNKNOWN:
        DetectInitialState();
        break;

    case STATE_CLOSED: 
        pastDoorState = currentDoorState;
        if (CSpara.openDoorCmd /*&& CSpara.CLS*/)
        {
            currentDoorState = STATE_OPENING;
        }

        break;

    case STATE_OPENING: 
        pastDoorState = currentDoorState;
        if (CSpara.OLS)
        {
            currentDoorState = STATE_OPENED;
        }
        else if (CSpara.closeDoorCmd)
        {
            currentDoorState = STATE_CLOSING;
        }

        break;

    case STATE_OPENED: 
        pastDoorState = currentDoorState;
        if (CSpara.closeDoorCmd)
        {
            currentDoorState = STATE_CLOSING;
            closingPhaseTimer = 0; 
        }
        break;

    case STATE_CLOSING: 
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
        if (closingPhaseTimer++ < PARA_TABLE_USE.data.closingDoorTime / 10) // 5ms
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
