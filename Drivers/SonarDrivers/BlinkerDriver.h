/*
 * BlinkerDriver.h
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_BLINKERDRIVER_H_
#define SONARDRIVERS_BLINKERDRIVER_H_

#include <memory>
#include "stm32f1xx_hal.h"

class BlinkerDriver {
public:
	enum LightState {
		LightState_Enable,
		LightState_Disable
	};

	virtual ~BlinkerDriver();

	static std::shared_ptr<BlinkerDriver> createBlinker();
	void setLightState(LightState state);

private:
	static GPIO_TypeDef *blinkerGPIO;
	static const uint16_t blinkerPin;

	static std::shared_ptr<BlinkerDriver> instance;

	BlinkerDriver();

};

#endif /* SONARDRIVERS_BLINKERDRIVER_H_ */
