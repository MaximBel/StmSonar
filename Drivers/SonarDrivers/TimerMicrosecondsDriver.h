/*
 * TimerMicrosecondsDriver.h
 *
 *  Created on: 26 ���. 2020 �.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_TIMERMICROSECONDSDRIVER_H_
#define SONARDRIVERS_TIMERMICROSECONDSDRIVER_H_

#include "stddef.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include <memory>
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "TimerMicrosInterface.h"

using namespace std;

class TimerMicrosecondsDriver: public TimerMicrosInterface {
public:
	TimerMicrosecondsDriver();
	virtual ~TimerMicrosecondsDriver() = default;

	static std::shared_ptr<TimerMicrosecondsDriver> getInstance();

	uint64_t getTimeMicros() override;

	void start() override;

	void delayUsec(uint64_t delay) override;
	void delayUntilUsec(uint64_t &startMoment, uint64_t delay) override;

	void timReloadCallback();

private:
	static TIM_TypeDef *TIMER_INSTANCE;

	uint64_t reloadCounter;

	TIM_HandleTypeDef htim;

	SemaphoreHandle_t mutex;

	void initTim();
};

#endif /* SONARDRIVERS_TIMERMICROSECONDSDRIVER_H_ */
