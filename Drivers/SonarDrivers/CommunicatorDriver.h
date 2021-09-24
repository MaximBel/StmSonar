/*
 * CommunicatorDriver.h
 *
 *  Created on: 29 сент. 2020 г.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_COMMUNICATORDRIVER_H_
#define SONARDRIVERS_COMMUNICATORDRIVER_H_

#include "stddef.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include <memory>

using namespace std;

class CommunicatorDriver {
public:
	CommunicatorDriver();
	~CommunicatorDriver() = default;

	void sendData(uint8_t* dataArrayPtr, uint16_t dataCount);

private:
	static const uint8_t TX_QUEUE_SIZE;
	static const USART_TypeDef& USART_INST;

	static UART_HandleTypeDef huart;


	void initUart();
	void initModule();
};

#endif /* SONARDRIVERS_COMMUNICATORDRIVER_H_ */
