/*
 * CommunicatorDriver.cpp
 *
 *  Created on: 29 сент. 2020 г.
 *      Author: dezzm
 */

#include <CommunicatorDriver.h>
#include <vector>
#include "handlers.h"

shared_ptr<CommunicatorDriver> CommunicatorDriver::instance = nullptr;
const uint8_t CommunicatorDriver::TX_QUEUE_SIZE = 50;
UART_HandleTypeDef CommunicatorDriver::huart = { 0 };
const USART_TypeDef & CommunicatorDriver::USART_INST = *USART1;

CommunicatorDriver::CommunicatorDriver() {
	initUart();
	initModule();
	initQueueAndTask();
}

shared_ptr<CommunicatorDriver> CommunicatorDriver::getInstance() {
	if (instance == nullptr) {
		instance = shared_ptr<CommunicatorDriver>(new CommunicatorDriver);
	}
	return instance;
}

void CommunicatorDriver::sendData(const uint8_t* dataArrayPtr,
		uint16_t dataCount) {
	auto dataVector = new vector<uint8_t>(dataArrayPtr,
			dataArrayPtr + dataCount);

	xQueueSend(txQueueHandle, &dataVector, 0);
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

void CommunicatorDriver::initQueueAndTask() {
	txQueueHandle = xQueueCreate(TX_QUEUE_SIZE, sizeof(vector<uint8_t>*));

	/* Create the task, storing the handle. */
	if (xTaskCreate(this->processingTaskFunc, "US-communication", 128 * 7, this,
			24, &processingTaskHandle) != pdPASS) {
		Init_Error_Handler();
	}
}

void CommunicatorDriver::processingTaskFunc(void * pvParameters) {
	CommunicatorDriver *instance =
			static_cast<CommunicatorDriver *>(pvParameters);
	vector<uint8_t> *txDataVectorPtr = nullptr;

	for (;;) {
		if (xQueueReceive(instance->txQueueHandle, &txDataVectorPtr,
				1000) == pdPASS) {
			HAL_UART_Transmit(&instance->huart, &((*txDataVectorPtr)[0]),
					txDataVectorPtr->size(), 500);
			delete txDataVectorPtr;
		}
	}
}
