/*
 * TimerMicrosInterface.h
 *
 *  Created on: 28 ���. 2020 �.
 *      Author: dezzm
 */

#ifndef SONARDRIVERS_TIMERMICROSINTERFACE_H_
#define SONARDRIVERS_TIMERMICROSINTERFACE_H_

#include "stdint.h"

class TimerMicrosInterface {
public:
	TimerMicrosInterface() = default;
	virtual ~TimerMicrosInterface() = default;
	virtual void start() = 0;
	virtual uint64_t getTimeMicros() = 0;
	virtual void delayUsec(uint64_t delay) = 0;
	virtual void delayUntilUsec(uint64_t &startMoment, uint64_t delay) = 0;
};

#endif /* SONARDRIVERS_TIMERMICROSINTERFACE_H_ */
