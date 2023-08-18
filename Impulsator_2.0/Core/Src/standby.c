/*
 * PVD.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */
#include <standby.h>

void STANDBY(void){
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	PWR->CR |= PWR_CR_ULP;
	PWR->CR |= (1U<<1);

	SCB->SCR|= SCB_SCR_SLEEPDEEP_Msk;
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

	#if defined ( __CC_ARM)
		__force_stores();
	#endif
	for(int i=0;i<50;i++){
		GPIOA->ODR=(1U<<5);
		HAL_Delay(500);
		GPIOA->ODR=~(1U<<5);
	}
		while(1){
		 __WFI();
	}
}
