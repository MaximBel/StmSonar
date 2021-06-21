/*
 * UltrasonicSensorDriver.h
 *
 *  Created on: 25 рту. 2020 у.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_ULTRASONICSENSORDRIVER_H_
#define SONARDRIVERS_ULTRASONICSENSORDRIVER_H_

#include "stddef.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include <memory>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "TimerMicrosInterface.h"
#include "vector"

using namespace std;

class UltrasonicSensorDriverWaterproof {
public:
	typedef enum {
		EXTITriggerTypeRise, EXTITriggerTypeFall
	} EXTITriggerType_t;

	typedef struct {
		uint64_t responceTime;
		EXTITriggerType_t type;
	} EchoResponceData_t;

	typedef struct {
		uint64_t timeFromStart;
		uint64_t duration;
	} PulseData_t;

	using measurementResultType = vector<PulseData_t>*;

	virtual ~UltrasonicSensorDriverWaterproof();

	static shared_ptr<UltrasonicSensorDriverWaterproof> getInstance();

	void setTimerMicros(shared_ptr<TimerMicrosInterface> timer);

	/**
	 * @param waitTime time in microseconds
	 */
	void startMeasurement(uint8_t pulseCount);

	measurementResultType stopMeasurement();

	void timReloadCallback();

	void EXTICallback();

private:
	static shared_ptr<UltrasonicSensorDriverWaterproof> instance;
	static uint16_t ECHO_PIN;
	static uint16_t ECHO_LOCK_PIN;
	static GPIO_TypeDef *ECHO_PORT;
	static GPIO_TypeDef *ECHO_LOCK_PORT;
	static uint16_t TRIG_PIN;
	static GPIO_TypeDef *TRIG_PORT;
	static IRQn_Type ECHO_EXTI;
	static const uint16_t ECHO_QUEUE_COUNT;

	typedef enum {
		EchoStateLock, EchoStateUnlock
	} EchoState_t;

	uint64_t measurementStartTime;

	QueueHandle_t echoQueueHandle;

	shared_ptr<TimerMicrosInterface> timerMicros;

	UltrasonicSensorDriverWaterproof();
	void initGpio();

	void trigSensor();

	void resetEchoQueue();
};

#endif /* SONARDRIVERS_ULTRASONICSENSORDRIVER_H_ */
