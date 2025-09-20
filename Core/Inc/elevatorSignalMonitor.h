#ifndef __ELEVATOR_SIGNAL_MONITOR__H__
#define __ELEVATOR_SIGNAL_MONITOR__H__

#include "main.h"

// 状态枚举
typedef enum {
    STATE_UNKNOWN,      // 初始未知状态（用于初始化）
    STATE_CLOSED,          // 关门到位状态
    STATE_OPENING,         // 开门过程中
    STATE_OPENED,          // 开门到位状态
    STATE_CLOSING          // 关门过程中
} DoorState;

extern uint32_t ds6012OutputEN;
extern uint32_t nd06OutputEN;
extern uint32_t IdleFlg;
extern DoorState currentDoorState ;
extern DoorState pastDoorState ;

void ElevatorSignaMonitoring(void);
// currentDoorState: ds6012OutputEN, nd06outputEN 
void ElevatorSignaMonitoring(void);
// CSpara-->currentDoorState
void DetectInitialState(void);
// CSpara-->currentDoorState,pastDoorState
void UpdateDoorStateMachine(void);


#endif