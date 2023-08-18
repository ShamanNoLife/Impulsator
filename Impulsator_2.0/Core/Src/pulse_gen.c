
/*
 * pulse_gen.c
 *
 *  Created on: Jul 27, 2023
 *      Author: Dell
 */
#include <pulse_gen.h>

uint8_t send_pulse_v1(uint8_t freg, uint8_t duty_cycle){
	uint8_t total_pulses=0;
	float Ton,Toff,Period;
	Period=(float)(1000/freg);
	Ton=(float)((Period*duty_cycle)/100);
	Toff=Period-Ton;
	while(1){
		GPIOC->BSRR = ~(1U<<4);
		while((GPIOC->ODR & GPIO_ODR_OD4)){}
		HAL_Delay((uint32_t)Ton);
		GPIOC->BSRR = (1U<<4);
		while(!(GPIOC->ODR & GPIO_ODR_OD4)){}
		total_pulses++;
		HAL_Delay((uint32_t)Toff);
		}
	return total_pulses;
}

uint32_t send_pulse_v2(uint32_t num, uint8_t freg, uint8_t duty_cycle){
	uint32_t total_pulses=0;
	float Ton,Toff,Period;
	Period=(float)(1000/freg);
	Ton=(float)((Period*duty_cycle)/100);
	Toff=Period-Ton;
	while(num>0){
		GPIOC->BSRR = ~(1U<<4);
		while((GPIOC->ODR & GPIO_ODR_OD4)){}
		HAL_Delay((uint32_t)Ton);
		GPIOC->BSRR = (1U<<4);
		while(!(GPIOC->ODR & GPIO_ODR_OD4)){}
		total_pulses++;
		HAL_Delay((uint32_t)Toff);
		num--;
		}
	return total_pulses;
}


