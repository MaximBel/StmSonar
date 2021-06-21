/*
 * SonarLogic.h
 *
 *  Created on: 29 но€б. 2020 г.
 *      Author: dezzm
 */

#ifndef SONARLOGIC_H_
#define SONARLOGIC_H_

#include "stddef.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include <memory>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "UltrasonicSensorDriverWaterproof.h"
#include "CommunicatorDriver.h"

using namespace std;

class SonarLogic {
public:
	SonarLogic(shared_ptr<UltrasonicSensorDriverWaterproof> sonar,
			shared_ptr<CommunicatorDriver> communicator);
	~SonarLogic() = default;

private:
	static string PULSE_START_TIME;
	static string PULSE_DURATION;
	static const uint32_t SEND_RECEIVE_TICKS_TO_WAIT;
	static char indexString[15];

	TaskHandle_t processingTaskHandle;
	TaskHandle_t measurementTaskHandle;

	QueueHandle_t measurementQueueHandle;

	shared_ptr<UltrasonicSensorDriverWaterproof> sonarDriver;
	shared_ptr<CommunicatorDriver> communicatorDriver;

	static void processingTaskFunc(void * pvParameters);
	static void measurementTaskFunc(void * pvParameters);

	static string serializeData(
			UltrasonicSensorDriverWaterproof::measurementResultType result);
};

#endif /* SONARLOGIC_H_ */
