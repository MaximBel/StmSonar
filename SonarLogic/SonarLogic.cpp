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
uint8_t SonarLogic::preambleData[] = { 0x45, 0x43, 0x48, 0xF4, 0x50, 0x52, 0x45 };

uint16_t SonarLogic::calcCrc16(uint8_t *pcBlock, uint16_t len) {
	uint16_t crc = 0xFFFF;

    while (len--) {
        crc ^= *pcBlock++ << 8;

        for (uint8_t i = 0; i < 8; i++) {
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
    }
    return crc;
}

SonarLogic::SonarLogic(shared_ptr<UltrasonicSensorDriverWaterproof> sonar,
		shared_ptr<CommunicatorDriver> communicator, shared_ptr<TimerMicrosInterface> timer) :
		sonarDriver(sonar), communicatorDriver(communicator), timerDriver(timer) {
}

uint8_t *SonarLogic::packData(UltrasonicSensorDriverWaterproof::measurementResultType result, uint16_t *resultDataSize) {
	uint16_t dataCount = result->size();
	uint16_t sizeOfArray = dataCount * sizeof(UltrasonicSensorDriverWaterproof::PulseData_t) + sizeof(preambleData) + sizeof(dataCount) + 2;
	auto returnArray = new uint8_t[sizeOfArray];

	dataCount = dataCount | 0xFF00;

	memcpy(returnArray, preambleData, sizeof(preambleData));
	memcpy(returnArray + sizeof(preambleData), &dataCount, sizeof(dataCount));

	uint16_t dataIndex = sizeof(preambleData) + sizeof(dataCount);
	for (UltrasonicSensorDriverWaterproof::PulseData_t element : *result) {
		memcpy(&returnArray[dataIndex], &element, sizeof(element));
		dataIndex += sizeof(element);
	}

	auto crc = calcCrc16(returnArray, sizeOfArray - 2);

	memcpy(&returnArray[sizeOfArray - 2], &crc, sizeof(crc));

	*resultDataSize = sizeOfArray;

	return returnArray;
}

string SonarLogic::serializeData(
		UltrasonicSensorDriverWaterproof::measurementResultType result) {
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

#include "vector"

void SonarLogic::process() {
	uint16_t returnDataSize = 0;

	sonarDriver->startMeasurement(8);
	timerDriver->delayUsec(50000);
	uint8_t *outputData = packData(sonarDriver->stopMeasurement(), &returnDataSize);
//	sonarDriver->stopMeasurement();
//
//	auto resultDataVectorPtr = std::make_shared<std::vector<UltrasonicSensorDriverWaterproof::PulseData_t>>();
//
//	UltrasonicSensorDriverWaterproof::PulseData_t data;
//
//	data.duration = 0xFAFA;
//	data.timeFromStart = 0xBDBD;
//
//	resultDataVectorPtr->push_back(data);
//	resultDataVectorPtr->push_back(data);
//
//	uint8_t * outputData = packData(resultDataVectorPtr, &returnDataSize);

//	uint64_t time = timerDriver->getTimeMicros();

	communicatorDriver->sendData(outputData, returnDataSize);

	//uint64_t delta = timerDriver->getTimeMicros() - time;

//	if (delta > 100000) {
//		asm("NOP");
//	}

	delete[] outputData;
}
