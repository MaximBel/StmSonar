/*
 * CommunicatorDriver.cpp
 *
 *  Created on: 29 сент. 2020 г.
 *      Author: dezzm
 */

#include <CommunicatorDriver.h>
#include <vector>
#include "handlers.h"

const uint8_t CommunicatorDriver::TX_QUEUE_SIZE = 50;
UART_HandleTypeDef CommunicatorDriver::huart = { 0 };
const USART_TypeDef & CommunicatorDriver::USART_INST = *USART1;

CommunicatorDriver::CommunicatorDriver() {
	initUart();
	initModule();
}

void CommunicatorDriver::sendData(uint8_t* dataArrayPtr,
		uint16_t dataCount) {
	HAL_UART_Transmit(&huart, dataArrayPtr, dataCount, 500);
}

void CommunicatorDriver::initUart() {
	huart.Instance = (USART_TypeDef*) &USART_INST;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart) != HAL_OK) {
		Init_Error_Handler();
	}
}

void CommunicatorDriver::initModule() {
	// TODO: push up CH_PD pin
}
