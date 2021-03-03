/*
 * redefinitions.cpp
 *
 *  Created on: 17 ���. 2020 �.
 *      Author: dezzm
 */

#include "FreeRTOS.h"
#include "portable.h"

void * operator new(size_t size) {
	return pvPortMalloc(size);
}

void * operator new[](size_t size) {
	return pvPortMalloc(size);
}

void operator delete(void * ptr) {
	vPortFree(ptr);
}

void operator delete[](void * ptr) {
	vPortFree(ptr);
}
