/*
 * CommunicatorDriver.h
 *
 *  Created on: 29 ����. 2020 �.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_COMMUNICATORDRIVER_H_
#define SONARDRIVERS_COMMUNICATORDRIVER_H_

#include "stddef.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include <memory>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

using namespace std;

class CommunicatorDriver {
public:
	CommunicatorDriver();
	~CommunicatorDriver() = default;

	static shared_ptr<CommunicatorDriver> getInstance();

	void sendData(const uint8_t* dataArrayPtr, uint16_t dataCount);

private:
	static const uint8_t TX_QUEUE_SIZE;
	static const USART_TypeDef& USART_INST;

	static shared_ptr<CommunicatorDriver> instance;
	static UART_HandleTypeDef huart;

	TaskHandle_t processingTaskHandle;
	QueueHandle_t txQueueHandle;

	void initUart();
	void initModule();
	void initQueueAndTask();

	static void processingTaskFunc(void * pvParameters);
};

#endif /* SONARDRIVERS_COMMUNICATORDRIVER_H_ */
