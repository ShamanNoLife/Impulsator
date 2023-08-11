/*
 * PVD.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */
#include  "PVD.h"

void Stop_mode(void){
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_Delay(2000);
    /* Select Standby mode */
     SET_BIT(PWR->CR, PWR_CR_PDDS);

     /* Set SLEEPDEEP bit of Cortex System Control Register */
     SET_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

     /* This option is used to ensure that store operations are completed */
   #if defined ( __CC_ARM)
     __force_stores();
   #endif
}
