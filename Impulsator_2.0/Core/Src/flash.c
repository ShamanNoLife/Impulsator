/*
 * flash.c
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */

#include "flash.h"

void save_data_1(uint32_t Adrr,uint32_t data){

    HAL_FLASH_Unlock();

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Adrr,data);

	HAL_FLASH_Lock();
}

void save_data_2(uint32_t Adrr, uint32_t data1,uint32_t data2,uint32_t data3,uint32_t data4,uint32_t data5,uint32_t data6){

	struct data_for_flash data_struct;

	data_struct.data1=data1;
	data_struct.data2=data2;
	data_struct.data3=data3;
	data_struct.data4=data4;
	data_struct.data5=data5;
	data_struct.data6=data6;

	uint32_t* ptr = (uint32_t*)&data_struct;

    HAL_FLASH_Unlock();

	for(int i =0;i<6;i++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Adrr+i*16,*(ptr+i));
	}

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
