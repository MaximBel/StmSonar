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
#include "UltrasonicSensorDriverWaterproof.h"
#include "CommunicatorDriver.h"
#include "TimerMicrosInterface.h"

using namespace std;

class SonarLogic {
public:
	SonarLogic(shared_ptr<UltrasonicSensorDriverWaterproof> sonar,
			shared_ptr<CommunicatorDriver> communicator,
			shared_ptr<TimerMicrosInterface> timer);
	~SonarLogic() = default;

	void process();

private:
	static string PULSE_START_TIME;
	static string PULSE_DURATION;
	static const uint32_t SEND_RECEIVE_TICKS_TO_WAIT;
	static char indexString[15];
	static uint8_t preambleData[];

	shared_ptr<UltrasonicSensorDriverWaterproof> sonarDriver;
	shared_ptr<CommunicatorDriver> communicatorDriver;
	shared_ptr<TimerMicrosInterface> timerDriver;

	static uint16_t calcCrc16(uint8_t *pcBlock, uint16_t len);

	static uint8_t *packData(UltrasonicSensorDriverWaterproof::measurementResultType result, uint16_t *resultDataSize);

	static string serializeData(
			UltrasonicSensorDriverWaterproof::measurementResultType result);
};

#endif /* SONARLOGIC_H_ */
