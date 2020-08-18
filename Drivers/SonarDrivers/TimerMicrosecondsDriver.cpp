/*
 * TimerMicrosecondsDriver.cpp
 *
 *  Created on: 26 рту. 2020 у.
 *      Author: dezzm
 */

#include <TimerMicrosecondsDriver.h>
#include "handlers.h"

// TODO: remove this dependency
#include "main.h"

shared_ptr<TimerMicrosecondsDriver> TimerMicrosecondsDriver::instance = nullptr;
TIM_TypeDef *TimerMicrosecondsDriver::TIMER_INSTANCE = TIM3;

TimerMicrosecondsDriver::TimerMicrosecondsDriver() :
		reloadCounter(0) {
	htim = {0};

	mutex = xSemaphoreCreateMutex();

	initTim();
}

void TimerMicrosecondsDriver::initTim() {

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	htim.Instance = TIMER_INSTANCE;
	htim.Init.Prescaler = 72;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 0xFFFF;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim) != HAL_OK) {
		Init_Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK) {
		Init_Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig)
			!= HAL_OK) {
		Init_Error_Handler();
	}
	HAL_TIM_MspPostInit(&htim);
}

shared_ptr<TimerMicrosecondsDriver> TimerMicrosecondsDriver::getInstance() {
	if (instance == nullptr) {
		instance = shared_ptr<TimerMicrosecondsDriver>(
				new TimerMicrosecondsDriver);
	}
	return instance;
}

uint64_t TimerMicrosecondsDriver::getTimeMicros() {
	uint64_t currentTime = 0;
	//xSemaphoreTake(mutex, portMAX_DELAY);
	// TODO: think about atomic
	currentTime = reloadCounter * 0xFFFF + htim.Instance->CNT;
	//xSemaphoreGive(mutex);
	return currentTime;
}

void TimerMicrosecondsDriver::start() {
	HAL_TIM_Base_Start_IT(&htim);
}

void TimerMicrosecondsDriver::timReloadCallback() {
	//static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	//xSemaphoreTakeFromISR(mutex, &xHigherPriorityTaskWoken);
	reloadCounter++;
	//xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);

	if (__HAL_TIM_GET_FLAG(&htim, TIM_FLAG_UPDATE) != RESET) {
		if (__HAL_TIM_GET_IT_SOURCE(&htim, TIM_IT_UPDATE) != RESET) {
			__HAL_TIM_CLEAR_IT(&htim, TIM_IT_UPDATE);
		}
	}

	//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
