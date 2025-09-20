#ifndef _RS485_DRV_H_
#define _RS485_DRV_H_

#include "usart.h"
#include "platform.h"
#include "main.h"
#include <stdbool.h>
/* User Config */
#define RS485_Elevator_USART	huart1
#define RS485_PDA_USART		huart2
//#define RS485_RE_GPIO_PORT  GPIOG
//#define RS485_RE_GPIO_PIN   GPIO_PIN_8

/* RS485 TX/RX Control */


#define RS485_Elevator_TX_ENABLE()   	delay_10us(2);\
									HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);\
									delay_10us(2);

#define RS485_Elevator_RX_ENABLE()   	delay_10us(2);\
									HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);\
									delay_10us(2);
                                    
//#define RS485_Elevator_TX_ENABLE()   	delay_10us(2);\
//									HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_SET);\
//									delay_10us(2);

//#define RS485_Elevator_RX_ENABLE()   	delay_10us(2);\
//									HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_RESET);\
//									delay_10us(2);                 
									
									
#define RS485_PDA_TX_ENABLE()   		delay_10us(2);\
									HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_SET);\
									delay_10us(2);

#define RS485_PDA_RX_ENABLE()   		delay_10us(2);\
									HAL_GPIO_WritePin(RS485_GHP_EN_GPIO_Port, RS485_GHP_EN_Pin, GPIO_PIN_RESET);\
									delay_10us(2);							
									
								

void RS485_Elevator_Init(void);
int RS485_Elevator_Transmit(uint8_t *data, uint16_t len);
void RS485_PDA_Init(void);
int RS485_PDA_Transmit(uint8_t *data, uint16_t len);




extern uint8_t rs485ElevatorRxCpltFlag;
extern uint8_t rx_data_Elevator[6];
extern uint8_t rs485PDARxCpltFlag;
extern uint8_t rxDataBuffPDA[4];
extern uint16_t RxElevatorCnt;

/*
uint32_t closeDoorCmd;      关门指令
uint32_t openDoorCmd;       开门指令
uint32_t OLS;               开门到位
uint32_t CLS;               关门到位
uint32_t LevelingSignal;    平层
*/
typedef struct{
	uint32_t closeDoorCmd;    
	uint32_t openDoorCmd;
	uint32_t OLS;
	uint32_t CLS;
	uint32_t LevelingSignal;
}ControlSystemPara;

extern ControlSystemPara CSpara;

//   PDA   
#pragma pack(push, 1)
typedef struct {
    uint16_t head;   // 2 
    uint16_t sj;     // 2 
    uint8_t version; // 1 
    uint16_t length; // 2 
    uint16_t crc;    // 2 
    uint16_t cmd;    // 2 
    uint8_t l1;      // 1 
    uint8_t data1[]; //   ,  
} SensorProtocol;
#pragma pack(pop)

typedef union {
    uint8_t raw; // 整个字节
    struct {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
    } bits;
} ByteBits;

#define MAX_RX_LEN 128 
#define HEADER 0x55AA
extern uint8_t rxDBuffPDA[MAX_RX_LEN];
extern uint16_t rxSize;
extern uint8_t RxDataElevator[6];
extern uint16_t rxSize;
extern bool isSixBytes;
extern bool isFiveBytes;

extern uint16_t headerIsFoundFlag;

extern uint8_t masterElevator_LevelingSignal;

extern uint8_t dTS6012M_rxDone;

extern uint16_t rxIndexTooling;
extern uint8_t rxDBuffTooling[10];

// 新增数据处理函数
void ProcessElevatorData(void);

uint8_t CalcCRC8(uint8_t *inBuffer, int inSize);
uint8_t xor_checkSum(uint8_t *data, uint8_t length);
uint8_t isFrontDoorID(uint8_t id);
uint8_t isBackDoorID(uint8_t id);

void ProcessTooling(const uint8_t *data);

#endif /* _RS485_DRV_H_ */