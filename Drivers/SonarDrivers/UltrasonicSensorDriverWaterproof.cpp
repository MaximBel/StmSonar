/*
 * UltrasonicSensorDriver.cpp
 *
 *  Created on: 25 рту. 2020 у.
 *      Author: dezzm
 */

#include <UltrasonicSensorDriverWaterproof.h>
#include "portmacro.h"
#include "vector"
// TODO: remove this dependency
#include "main.h"
#include "string.h"
#include "handlers.h"

shared_ptr<UltrasonicSensorDriverWaterproof> UltrasonicSensorDriverWaterproof::instance = nullptr;

uint16_t UltrasonicSensorDriverWaterproof::ECHO_PIN = GPIO_PIN_1;
uint16_t UltrasonicSensorDriverWaterproof::ECHO_LOCK_PIN = GPIO_PIN_2;
GPIO_TypeDef *UltrasonicSensorDriverWaterproof::ECHO_PORT = GPIOA;
GPIO_TypeDef *UltrasonicSensorDriverWaterproof::ECHO_LOCK_PORT = GPIOA;
uint16_t UltrasonicSensorDriverWaterproof::TRIG_PIN = GPIO_PIN_0;
GPIO_TypeDef *UltrasonicSensorDriverWaterproof::TRIG_PORT = GPIOA;
IRQn_Type UltrasonicSensorDriverWaterproof::ECHO_EXTI = EXTI1_IRQn;
const uint16_t UltrasonicSensorDriverWaterproof::ECHO_QUEUE_COUNT = 120;

void UltrasonicSensorDriverWaterproof::initGpio() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin : ECHO_Pin */
	GPIO_InitStruct.Pin = ECHO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ECHO_LOCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ECHO_LOCK_PORT, &GPIO_InitStruct);

	/*Configure GPIO pin : TRIG_Pin */
	GPIO_InitStruct.Pin = TRIG_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(ECHO_EXTI, 6, 0);
	HAL_NVIC_EnableIRQ(ECHO_EXTI);
}

void UltrasonicSensorDriverWaterproof::trigSensor() {
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

void UltrasonicSensorDriverWaterproof::resetEchoQueue() {
	EchoResponceData_t dataElement;
	while (xQueueReceive(echoQueueHandle, &(dataElement), (TickType_t) 0)
			== pdPASS) {
	}
}

UltrasonicSensorDriverWaterproof::UltrasonicSensorDriverWaterproof() :
		measurementStartTime(0) {
	initGpio();

	echoQueueHandle = xQueueCreate(ECHO_QUEUE_COUNT, sizeof(EchoResponceData_t));
}

UltrasonicSensorDriverWaterproof::~UltrasonicSensorDriverWaterproof() {
	// TODO Auto-generated destructor stub
}

shared_ptr<UltrasonicSensorDriverWaterproof> UltrasonicSensorDriverWaterproof::getInstance() {
	if (instance == nullptr) {
		instance = shared_ptr<UltrasonicSensorDriverWaterproof>(
				new UltrasonicSensorDriverWaterproof);
	}
	return instance;
}

void UltrasonicSensorDriverWaterproof::setTimerMicros(
		shared_ptr<TimerMicrosInterface> timer) {
	timerMicros = timer;
}

void UltrasonicSensorDriverWaterproof::startMeasurement(uint8_t pulseCount) {
	(void)pulseCount;

	HAL_NVIC_DisableIRQ(ECHO_EXTI);

	resetEchoQueue();

	trigSensor();

	// Not sure I should put it here
	HAL_NVIC_EnableIRQ(ECHO_EXTI);

	measurementStartTime = timerMicros->getTimeMicros();
}

UltrasonicSensorDriverWaterproof::measurementResultType UltrasonicSensorDriverWaterproof::stopMeasurement() {
	auto resultVector = new vector<PulseData_t>;
	resultVector->reserve(20);

	HAL_NVIC_DisableIRQ(ECHO_EXTI);

	EchoResponceData_t data[120];
	int index = 0;
	//PulseData_t pdata[30];
	//int pindex = 0;

	uint8_t availableDataInQueue = uxQueueMessagesWaiting(echoQueueHandle) - 2;

	// only numbers multiple of 2 are available
	if (availableDataInQueue % 2 == 0) {
		EXTITriggerType_t nextExpectedType = EXTITriggerTypeRise;
		PulseData_t pulseData;
		uint64_t pulseStartTime = 0;

		while (availableDataInQueue > 0) {
			EchoResponceData_t dataElement = { 0 };
			if (xQueueReceive(echoQueueHandle, &(dataElement),
					(TickType_t) 0) == pdPASS) {

				data[index] = dataElement;
				index++;

				if (dataElement.type == nextExpectedType) {
					if (dataElement.type == EXTITriggerTypeFall) {
						pulseData.duration = dataElement.responceTime
								- pulseStartTime;
						//resultVector->push_back(pulseData);

						//pdata[pindex] = pulseData;
						//pindex++;

						nextExpectedType = EXTITriggerTypeRise;
					}
					if (dataElement.type == EXTITriggerTypeRise) {
						pulseData.timeFromStart = dataElement.responceTime
								- measurementStartTime;
						pulseStartTime = dataElement.responceTime;


						nextExpectedType = EXTITriggerTypeFall;
					}

					availableDataInQueue--;
				} else {
	//				resultVector->clear();
	//				break;
				}
			} else {
				resultVector->clear();
				break;
			}
		}
	} else {
		resultVector->clear();
	}

	return resultVector;
}

void UltrasonicSensorDriverWaterproof::EXTICallback() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(ECHO_PIN) != 0x00u) {
		EchoResponceData_t data;

		data.type =
				HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET ?
						EXTITriggerTypeRise : EXTITriggerTypeFall;
		data.responceTime = timerMicros->getTimeMicros();

		xQueueSendFromISR(echoQueueHandle, &data, &xHigherPriorityTaskWoken);

		__HAL_GPIO_EXTI_CLEAR_IT(ECHO_PIN);
		HAL_GPIO_EXTI_Callback(ECHO_PIN);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

