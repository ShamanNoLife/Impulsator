/*
 * flash.h
 *
 *  Created on: Jul 26, 2023
 *      Author: Dell
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

void save_data_to_flash(uint32_t Address, uint32_t* data, uint8_t size_of_config);
uint32_t read_data_from_flash(uint32_t Address);
void erase_data_from_flash(uint32_t Address);
#endif /* INC_FLASH_H_ */
