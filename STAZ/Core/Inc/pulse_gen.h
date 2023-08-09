/*
 * pulse_gen.h
 *
 *  Created on: Jul 27, 2023
 *      Author: Dell
 */

#ifndef INC_PULSE_GEN_H_
#define INC_PULSE_GEN_H_

#include "main.h"

uint8_t send_pulse_v1(uint8_t freg, uint8_t duty_cycle);
uint32_t send_pulse_v2(uint32_t num, uint8_t freg, uint8_t duty_cycle);

#endif /* INC_PULSE_GEN_H_ */
