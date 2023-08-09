/*
 * flash.h
 *
 *  Created on: Jul 26, 2023
 *      Author: Dell
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

void save_data(uint32_t Adrr, uint32_t data);
uint32_t read_data(uint32_t Address);
void erase_data(uint32_t Address);
#endif /* INC_FLASH_H_ */
