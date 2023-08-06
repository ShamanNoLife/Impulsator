/*
 * PVD.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */
#include  "PVD.h"

//void PVD_init(void){
//	RCC->APB1ENR |=RCC_APB1ENR_PWREN;
//	PWR->CR |= PWR_CR_PLS_LEV7;
//	EXTI->RTSR |=EXTI_RTSR_RT16;
//	NVIC_EnableIRQ(PVD_IRQn);
//	NVIC_SetPriority(PVD_IRQn,8);
//	PWR->CR |= PWR_CR_PVDE;
//}

static void Stop_mode(void){
    HAL_Delay(2000);
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);
}

void PVD_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR_PVDTypeDef sConfigPVD;
	sConfigPVD.PVDLevel = PWR_PVDLEVEL_3;
	sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING;
	HAL_PWR_PVDConfig(&sConfigPVD);
	NVIC_EnableIRQ(PVD_IRQn);
	NVIC_SetPriority(PVD_IRQn, 0);
	HAL_PWR_EnablePVD();
}

void HAL_PWR_PVDCallback(void){
    if (PWR->CSR & PWR_CSR_PVDO) {
		GPIOB->BSRR=(1U<<2);
		uint8_t state_pvd=0;
		state_pvd++;
		save_data(0x0800FFA0, state_pvd, 0, 0, 0);
		//Stop_mode();
    }
}


