#include "main.h"

void initTooling();

void TOOLING_USART2_UART_Init(void);

void testUART1();

void testUART2();

uint8_t testDTS();

uint8_t testND06();

void testIO();

void transmitResult(UART_HandleTypeDef *huart, uint8_t *data, uint16_t len);
