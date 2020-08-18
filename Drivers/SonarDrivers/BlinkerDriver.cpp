/*
 * BlinkerDriver.cpp
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#include "BlinkerDriver.h"

GPIO_TypeDef *BlinkerDriver::blinkerGPIO = GPIOC;
const uint16_t BlinkerDriver::blinkerPin = GPIO_PIN_13;

std::shared_ptr<BlinkerDriver> BlinkerDriver::instance = nullptr;

std::shared_ptr<BlinkerDriver> BlinkerDriver::createBlinker() {
	if (instance == nullptr) {
		instance = std::shared_ptr<BlinkerDriver>(new BlinkerDriver);
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
	HAL_GPIO_WritePin(blinkerGPIO, blinkerPin, GPIO_PIN_RESET);
}

BlinkerDriver::~BlinkerDriver() {
	// TODO Auto-generated destructor stub
}

