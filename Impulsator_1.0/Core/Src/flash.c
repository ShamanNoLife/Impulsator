/*
 * flash.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */

#include "flash.h"

void save_data(uint32_t Address,uint32_t data){
    HAL_FLASH_Unlock();
	for(int i=0;i<50;i++){}
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Address,(uint32_t)data);
	for(int i=0;i<50;i++){}
	HAL_FLASH_Lock();
}

uint32_t read_data(uint32_t Address){
	__IO uint32_t read_data = *(__IO uint32_t *)Address;
	return (uint32_t)read_data;
}

void erase_data(uint32_t Address){
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = Address;
	EraseInitStruct.NbPages = 1;
	uint32_t PageError;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
			return HAL_ERROR;
	HAL_FLASH_Lock();
}
