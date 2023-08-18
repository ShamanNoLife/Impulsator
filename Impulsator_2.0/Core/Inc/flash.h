/*
 * flash.h
 *
 *  Created on: Jul 26, 2023
 *      Author: Dell
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

struct data_for_flash{
	uint32_t data1;
	uint32_t data2;
	uint32_t data3;
	uint32_t data4;
	uint32_t data5;
	uint32_t data6;
};

void save_data_1(uint32_t Adrr, uint32_t data);
void save_data_2(uint32_t Adrr, uint32_t data1,uint32_t data2,uint32_t data3,uint32_t data4,uint32_t data5,uint32_t data6);
uint32_t read_data(uint32_t Address);
void erase_data(uint32_t Address);
#endif /* INC_FLASH_H_ */
