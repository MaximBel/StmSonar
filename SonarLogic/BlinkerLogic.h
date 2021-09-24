/*
 * BlinkerLogic.h
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#ifndef SONARMIDDLEWARE_BLINKERLOGIC_H_
#define SONARMIDDLEWARE_BLINKERLOGIC_H_

#include "stdint.h"
#include "memory.h"
#include "BlinkerDriver.h"
#include "TimerMicrosInterface.h"

using namespace std;

class BlinkerLogic {
public:
	BlinkerLogic(shared_ptr<TimerMicrosInterface> timer);
	virtual ~BlinkerLogic() = default;

	void process();
private:
	static const uint32_t delayMicros;

	uint64_t lastUpdateTime;
	bool lastState;
	shared_ptr<BlinkerDriver> driver;
	shared_ptr<TimerMicrosInterface> timerDriver;

	bool isTimeToUpdate();

};

#endif /* SONARMIDDLEWARE_BLINKERLOGIC_H_ */
