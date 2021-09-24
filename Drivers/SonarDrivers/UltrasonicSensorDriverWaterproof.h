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
		uint16_t responceTime;
		EXTITriggerType_t type;
	} EchoResponceData_t;

#pragma pack(push, 1)
    typedef struct {
        uint16_t timeFromStart;
        uint16_t duration;
    } PulseData_t;
#pragma pack(pop)

	using measurementResultType = shared_ptr<vector<PulseData_t>>;

	UltrasonicSensorDriverWaterproof(shared_ptr<TimerMicrosInterface> timer);
	virtual ~UltrasonicSensorDriverWaterproof();

	/**
	 * @param waitTime time in microseconds
	 */
	void startMeasurement(uint8_t pulseCount);

	measurementResultType stopMeasurement();

	void timReloadCallback();

	void EXTICallback();

private:
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

	shared_ptr<vector<PulseData_t>> resultDataVectorPtr;

	shared_ptr<TimerMicrosInterface> timerMicros;

	EXTITriggerType_t nextExpectedType;
	PulseData_t pulseData;

	void initGpio();

	void trigSensor();

	void resetEchoQueue();
};

#endif /* SONARDRIVERS_ULTRASONICSENSORDRIVER_H_ */
