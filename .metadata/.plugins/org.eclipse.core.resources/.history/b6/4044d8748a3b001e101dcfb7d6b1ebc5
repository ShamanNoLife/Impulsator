/*
 * tim6.c
 *
 *  Created on: Aug 9, 2023
 *      Author: Dell
 */
#include "tim6.h"

void TIM6_init(void){
	RCC->APB1ENR=RCC_APB1ENR_TIM6EN;

	TIM6->PSC=400-1;
	TIM6->ARR=5000-1;
	TIM6->CNT=0;
	TIM6->CR1= TIM_CR1_CEN;
	TIM6->DIER|=TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_IRQn);
}

