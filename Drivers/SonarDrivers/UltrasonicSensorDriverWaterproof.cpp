/*
 * UltrasonicSensorDriver.cpp
 *
 *  Created on: 25 рту. 2020 у.
 *      Author: dezzm
 */

#include <UltrasonicSensorDriverWaterproof.h>
#include "vector"
// TODO: remove this dependency
#include "main.h"
#include "string.h"
#include "handlers.h"

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
	GPIO_InitStruct.Pull = GPIO_NOPULL;
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
	timerMicros->delayUsec(10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

void UltrasonicSensorDriverWaterproof::resetEchoQueue() {
	resultDataVectorPtr = make_shared<vector<PulseData_t>>();
	resultDataVectorPtr->reserve(700);
}

UltrasonicSensorDriverWaterproof::UltrasonicSensorDriverWaterproof(shared_ptr<TimerMicrosInterface> timer) :
		measurementStartTime(0), resultDataVectorPtr(nullptr), timerMicros(timer), nextExpectedType(EXTITriggerTypeRise) {
	initGpio();

	memset(&pulseData, 0, sizeof(pulseData));
}

UltrasonicSensorDriverWaterproof::~UltrasonicSensorDriverWaterproof() {
	// TODO Auto-generated destructor stub
}

void UltrasonicSensorDriverWaterproof::startMeasurement(uint8_t pulseCount) {
	(void)pulseCount;

	HAL_NVIC_DisableIRQ(ECHO_EXTI);

	resetEchoQueue();

	nextExpectedType = EXTITriggerTypeRise;
	resultDataVectorPtr->clear();

	trigSensor();

	measurementStartTime = timerMicros->getTimeMicros();

	// Not sure I should put it here
	HAL_NVIC_EnableIRQ(ECHO_EXTI);
}

UltrasonicSensorDriverWaterproof::measurementResultType UltrasonicSensorDriverWaterproof::stopMeasurement() {
	HAL_NVIC_DisableIRQ(ECHO_EXTI);

	return resultDataVectorPtr;
}

void UltrasonicSensorDriverWaterproof::EXTICallback() {
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(ECHO_PIN) != 0x00u) {
		EchoResponceData_t data;

		EXTITriggerType_t type =
				HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET ?
						EXTITriggerTypeRise : EXTITriggerTypeFall;
		uint32_t responceTime = timerMicros->getTimeMicros() - measurementStartTime;

		if (type == nextExpectedType) {
			if (type == EXTITriggerTypeFall) {
				pulseData.duration = responceTime - pulseData.timeFromStart;
				resultDataVectorPtr->push_back(pulseData);

				nextExpectedType = EXTITriggerTypeRise;
			}
			if (type == EXTITriggerTypeRise) {
				pulseData.timeFromStart = responceTime;

				nextExpectedType = EXTITriggerTypeFall;
			}
		}

		__HAL_GPIO_EXTI_CLEAR_IT(ECHO_PIN);
	}
}

