/*
 * BlinkerLogic.h
 *
 *  Created on: 16 рту. 2020 у.
 *      Author: dezzm
 */

#ifndef SONARMIDDLEWARE_BLINKERLOGIC_H_
#define SONARMIDDLEWARE_BLINKERLOGIC_H_

#include "BlinkerDriver.h"

class BlinkerLogic {
public:
	BlinkerLogic();
	virtual ~BlinkerLogic();

	void process();
private:
	static const uint16_t delayMillis;

	std::shared_ptr<BlinkerDriver> driver;
	bool lastState;
};

#endif /* SONARMIDDLEWARE_BLINKERLOGIC_H_ */
