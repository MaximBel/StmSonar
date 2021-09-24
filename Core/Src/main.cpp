#include "main.h"
#include "handlers.h"

#include <memory.h>
#include "BlinkerLogic.h"
#include "TimerMicrosecondsDriver.h"
#include "UltrasonicSensorDriverWaterproof.h"
#include "CommunicatorDriver.h"
#include "SonarLogic.h"

using namespace std;

static shared_ptr<TimerMicrosInterface> timerMicros = nullptr;
static shared_ptr<UltrasonicSensorDriverWaterproof> usDriver = nullptr;
static SonarLogic *sonarLogic = nullptr;
static BlinkerLogic *blinkerLogic = nullptr;

void SystemClock_Config(void);
void StartDefaultTask(void *argument);

//void tim2IrqCallback() {
//	if (usDriver != nullptr) {
//		usDriver->timReloadCallback();
//	}
//}

void tim3IrqCallback() {
	if (timerMicros != nullptr) {
		//TODO: think about how to make something better, than pointer conversion.
		(static_pointer_cast<TimerMicrosecondsDriver>(timerMicros))->timReloadCallback();
	}
}

void EXTI1IrqCallback() {
	if (usDriver != nullptr) {
		usDriver->EXTICallback();
	}
}

int main(void) {
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	timerMicros = std::make_shared<TimerMicrosecondsDriver>();
	timerMicros->start();

	usDriver = std::make_shared<UltrasonicSensorDriverWaterproof>(timerMicros);

	sonarLogic = new SonarLogic(usDriver, std::make_shared<CommunicatorDriver>(), timerMicros);

	blinkerLogic = new BlinkerLogic(timerMicros);

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */

	auto timestamp = timerMicros->getTimeMicros();
	while (1) {
		sonarLogic->process();
		blinkerLogic->process();
		timerMicros->delayUntilUsec(timestamp, 50000);
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Init_Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Init_Error_Handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
}
