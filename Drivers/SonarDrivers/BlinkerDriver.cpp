/*
 * BlinkerDriver.cpp
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#include "BlinkerDriver.h"

GPIO_TypeDef *BlinkerDriver::blinkerGPIO = GPIOC;
const uint16_t BlinkerDriver::blinkerPin = GPIO_PIN_13;

shared_ptr<BlinkerDriver> BlinkerDriver::instance = nullptr;

shared_ptr<BlinkerDriver> BlinkerDriver::createBlinker() {
	if (instance == nullptr) {
		instance = shared_ptr<BlinkerDriver>(new BlinkerDriver);
	}
	return instance;
}

void BlinkerDriver::setLightState(BlinkerDriver::LightState state) {
	if (state == LightState_Enable) {
		HAL_GPIO_WritePin(blinkerGPIO, blinkerPin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(blinkerGPIO, blinkerPin, GPIO_PIN_RESET);
	}
}

BlinkerDriver::BlinkerDriver() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
