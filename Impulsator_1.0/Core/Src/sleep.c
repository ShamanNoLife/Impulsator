/*
 * PVD.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */
#include <sleep.h>

void Stop_mode(void){

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    PWR->CR |= PWR_CR_CWUF;
    PWR->CR |= PWR_CR_ULP;
    PWR->CR |= PWR_CR_PDDS;

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

   __WFI();

}
