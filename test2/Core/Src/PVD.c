/*
 * PVD.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */
#include  "PVD.h"

void Stop_mode(void){
    HAL_Delay(2000);
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);
}

void PVD_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR_PVDTypeDef sConfigPVD;
	sConfigPVD.PVDLevel = PWR_PVDLEVEL_6;
	sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING;
	HAL_PWR_PVDConfig(&sConfigPVD);
	HAL_PWR_EnablePVD();
}




