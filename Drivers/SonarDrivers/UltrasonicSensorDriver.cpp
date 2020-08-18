/*
 * UltrasonicSensorDriver.cpp
 *
 *  Created on: 25 рту. 2020 у.
 *      Author: dezzm
 */

#include <UltrasonicSensorDriver.h>
#include "portmacro.h"
#include "vector"
// TODO: remove this dependency
#include "main.h"
#include "string.h"
#include "handlers.h"

shared_ptr<UltrasonicSensorDriver> UltrasonicSensorDriver::instance = nullptr;
TIM_TypeDef *UltrasonicSensorDriver::TIMER_INSTANCE = TIM2;
uint32_t UltrasonicSensorDriver::TIMER_CHANNEL = TIM_CHANNEL_1;
uint32_t UltrasonicSensorDriver::TIMER_CHANNEL_IT = TIM_IT_CC1;

uint16_t UltrasonicSensorDriver::ECHO_PIN = GPIO_PIN_1;
uint16_t UltrasonicSensorDriver::ECHO_LOCK_PIN = GPIO_PIN_2;
GPIO_TypeDef *UltrasonicSensorDriver::ECHO_PORT = GPIOA;
GPIO_TypeDef *UltrasonicSensorDriver::ECHO_LOCK_PORT = GPIOA;
IRQn_Type UltrasonicSensorDriver::ECHO_EXTI = EXTI1_IRQn;
const uint16_t UltrasonicSensorDriver::ECHO_QUEUE_COUNT = 30;

void UltrasonicSensorDriver::initGpio() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

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

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(ECHO_EXTI, 6, 0);
	HAL_NVIC_EnableIRQ(ECHO_EXTI);
}

void UltrasonicSensorDriver::initTim() {
	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	htim.Instance = TIMER_INSTANCE;
	htim.Init.Prescaler = 36;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 48;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim) != HAL_OK) {
		Init_Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK) {
		Init_Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim) != HAL_OK) {
		Init_Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig)
			!= HAL_OK) {
		Init_Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 23;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, TIMER_CHANNEL) != HAL_OK) {
		Init_Error_Handler();
	}
	HAL_TIM_MspPostInit(&htim);
}

void UltrasonicSensorDriver::setEchoLockState(EchoState_t state) {
	GPIO_PinState newState =
			state == EchoStateUnlock ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(ECHO_LOCK_PORT, ECHO_LOCK_PIN, newState);
}

void UltrasonicSensorDriver::resetEchoQueue() {
	EchoResponceData_t dataElement;
	while (xQueueReceive(echoQueueHandle, &(dataElement), (TickType_t) 0)
			== pdPASS) {
	}
}

UltrasonicSensorDriver::UltrasonicSensorDriver() :
		pulseCountTillStop(0), measurementStartTime(0) {
	htim = {0};

	initGpio();
	initTim();

	echoQueueHandle = xQueueCreate(ECHO_QUEUE_COUNT, sizeof(EchoResponceData_t));
}

UltrasonicSensorDriver::~UltrasonicSensorDriver() {
	// TODO Auto-generated destructor stub
}

shared_ptr<UltrasonicSensorDriver> UltrasonicSensorDriver::getInstance() {
	if (instance == nullptr) {
		instance = shared_ptr<UltrasonicSensorDriver>(
				new UltrasonicSensorDriver);
	}
	return instance;
}

void UltrasonicSensorDriver::setTimerMicros(
		shared_ptr<TimerMicrosInterface> timer) {
	timerMicros = timer;
}

void UltrasonicSensorDriver::startMeasurement(uint8_t pulseCount) {

	resetEchoQueue();

	pulseCountTillStop = pulseCount;
	HAL_TIM_PWM_Start_IT(&htim, TIMER_CHANNEL);
	measurementStartTime = timerMicros->getTimeMicros();
}

UltrasonicSensorDriver::measurementResultType UltrasonicSensorDriver::stopMeasurement() {
	auto resultVector = new vector<PulseData_t>;
	resultVector->reserve(15);

	setEchoLockState(EchoStateLock);

	uint8_t availableDataInQueue = uxQueueMessagesWaiting(echoQueueHandle);

	// only numbers multiple of 2 are available
	if (availableDataInQueue % 2 == 0) {
		EXTITriggerType_t nextExpectedType = EXTITriggerTypeFall;
		PulseData_t pulseData;
		uint64_t pulseStartTime = 0;

		while (availableDataInQueue > 0) {
			EchoResponceData_t dataElement = { 0 };
			if (xQueueReceive(echoQueueHandle, &(dataElement),
					(TickType_t) 0) == pdPASS) {

				if (dataElement.type == nextExpectedType) {
					if (dataElement.type == EXTITriggerTypeFall) {
						pulseData.timeFromStart = dataElement.responceTime
								- measurementStartTime;
						pulseStartTime = dataElement.responceTime;

						nextExpectedType = EXTITriggerTypeRise;
					}
					if (dataElement.type == EXTITriggerTypeRise) {
						pulseData.duration = dataElement.responceTime
								- pulseStartTime;
						resultVector->push_back(pulseData);

						nextExpectedType = EXTITriggerTypeFall;
					}

					availableDataInQueue--;
				} else {
					resultVector->clear();
					break;
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

void UltrasonicSensorDriver::timReloadCallback() {
	/* Capture compare 1 event */
	if (__HAL_TIM_GET_FLAG(&htim, TIMER_CHANNEL_IT) != RESET) {

		pulseCountTillStop--;
		if (pulseCountTillStop == 0) {
			HAL_TIM_PWM_Stop(&htim, TIMER_CHANNEL);
			// unlock receive
			setEchoLockState(EchoStateUnlock);
		}

		if (__HAL_TIM_GET_IT_SOURCE(&htim, TIMER_CHANNEL_IT) != RESET) {
			__HAL_TIM_CLEAR_IT(&htim, TIMER_CHANNEL_IT);
		}
	}
}

void UltrasonicSensorDriver::EXTICallback() {
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

