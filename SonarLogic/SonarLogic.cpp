/*
 * SonarLogic.cpp
 *
 *  Created on: 29 но€б. 2020 г.
 *      Author: dezzm
 */

#include "SonarLogic.h"
#include "DataSerializer.h"
#include "string.h"
#include "handlers.h"

string SonarLogic::PULSE_START_TIME = "pstart";
string SonarLogic::PULSE_DURATION = "pdur";
char SonarLogic::indexString[15];
const uint32_t SonarLogic::SEND_RECEIVE_TICKS_TO_WAIT = 1000;

SonarLogic::SonarLogic(shared_ptr<UltrasonicSensorDriver> sonar,
		shared_ptr<CommunicatorDriver> communicator) :
		sonarDriver(sonar), communicatorDriver(communicator) {

	measurementQueueHandle = xQueueCreate(15,
			sizeof(UltrasonicSensorDriver::measurementResultType));

	/* Create the task, storing the handle. */
	if (xTaskCreate(processingTaskFunc, "US-logic-process", 128 * 3, this, 24,
			&processingTaskHandle) != pdPASS) {
		Init_Error_Handler();
	}

	/* Create the task, storing the handle. */
	if (xTaskCreate(measurementTaskFunc, "US-logic-measure", 128 * 3, this, 24,
			&measurementTaskHandle) != pdPASS) {
		Init_Error_Handler();
	}

}

string SonarLogic::serializeData(
		UltrasonicSensorDriver::measurementResultType result) {
	auto serializer = make_shared<DataSerializer>();

	uint16_t keyIndex = 0;

	for (auto element : *result) {
		sprintf(indexString, "%u", (unsigned) keyIndex);
		keyIndex++;

		(void) serializer->addDataMember(
				string(PULSE_START_TIME + string(indexString)),
				element.timeFromStart);
		(void) serializer->addDataMember(
				string(PULSE_DURATION + string(indexString)), element.duration);
	}
	return serializer->serializeData();
}

void SonarLogic::processingTaskFunc(void * pvParameters) {
	auto instance = static_cast<SonarLogic*>(pvParameters);

	for (;;) {
		UltrasonicSensorDriver::measurementResultType result = nullptr;
		if (xQueueReceive(instance->measurementQueueHandle, &result,
				SEND_RECEIVE_TICKS_TO_WAIT) == pdPASS) {
			string serializedResult = serializeData(result);
			delete result;

			instance->communicatorDriver->sendData(
					reinterpret_cast<const uint8_t *>(serializedResult.c_str()),
					strlen(serializedResult.c_str()));
		}
	}
}

void SonarLogic::measurementTaskFunc(void * pvParameters) {
	auto instance = static_cast<SonarLogic*>(pvParameters);

	for (;;) {
		instance->sonarDriver->startMeasurement(8);
		vTaskDelay(50);
		auto result = instance->sonarDriver->stopMeasurement();

		xQueueSend(instance->measurementQueueHandle, &result,
				SEND_RECEIVE_TICKS_TO_WAIT);
	}
}
