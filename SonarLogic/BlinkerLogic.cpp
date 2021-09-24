/*
 * BlinkerLogic.cpp
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#include "BlinkerLogic.h"
#include "FreeRTOS.h"
#include "task.h"

const uint32_t BlinkerLogic::delayMicros = 1000000;

BlinkerLogic::BlinkerLogic(shared_ptr<TimerMicrosInterface> timer) :
		lastUpdateTime(0), lastState(false), driver(BlinkerDriver::createBlinker()), timerDriver(timer) {
}

void BlinkerLogic::process() {
	if (!isTimeToUpdate()) {
		return;
	}

	if (lastState == false) {
		driver->setLightState(BlinkerDriver::LightState_Enable);
		lastState = true;
	} else {
		driver->setLightState(BlinkerDriver::LightState_Disable);
		lastState = false;
	}
}

bool BlinkerLogic::isTimeToUpdate() {
	if (timerDriver->getTimeMicros() - lastUpdateTime > delayMicros) {
		lastUpdateTime = timerDriver->getTimeMicros();
		return true;
	}
	return false;
}

