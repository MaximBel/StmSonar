#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void tim2IrqCallback();
void tim3IrqCallback();
void EXTI1IrqCallback();

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
