/*
 * PVD.h
 *
 *  Created on: Jul 24, 2023
 *      Author: Karol
 */

#ifndef INC_PVD_H_
#define INC_PVD_H_

#include "main.h"
#include "flash.h"
void PVD_init(void);
void HAL_PWR_PVDCallback(void);

#endif /* INC_PVD_H_ */
