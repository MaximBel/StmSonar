/*
 * BlinkerLogic.cpp
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#include "BlinkerLogic.h"
#include "FreeRTOS.h"
#include "task.h"

const uint16_t BlinkerLogic::delayMillis = 1000;

BlinkerLogic::BlinkerLogic()
	: lastState(false) {
	driver = BlinkerDriver::createBlinker();
}

BlinkerLogic::~BlinkerLogic() {
}

void BlinkerLogic::process() {
	if (lastState == false) {
		driver->setLightState(BlinkerDriver::LightState_Enable);
		lastState = true;
	} else {
		driver->setLightState(BlinkerDriver::LightState_Disable);
		lastState = false;
	}
	vTaskDelay(delayMillis);
}


