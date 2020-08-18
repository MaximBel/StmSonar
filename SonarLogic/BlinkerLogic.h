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

using namespace std;

class BlinkerLogic {
public:
	BlinkerLogic();
	virtual ~BlinkerLogic() = default;

	void process();
private:
	static const uint16_t delayMillis;

	bool lastState;
	shared_ptr<BlinkerDriver> driver;

};

#endif /* SONARMIDDLEWARE_BLINKERLOGIC_H_ */
