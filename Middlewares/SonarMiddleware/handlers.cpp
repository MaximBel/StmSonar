/*
 * handlers.cpp
 *
 *  Created on: 3 мар. 2021 г.
 *      Author: dezzm
 */

#include "handlers.h"

void Init_Error_Handler() {
	while (1) {
		asm("NOP");
	}

}

