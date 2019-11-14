/*
 * hal.h
 *
 *  Created on: 2019-09-30
 *      Author: lukas.neverauskis
 */
/*
	Software for project WUCY - wearable open source personal computer.
	based on ESP32 running FreeRTOS on custom Therram kernel.
	<https://github.com/therram/wucy>

	License: GPL 3.0

	Copyright (C) 2019 Lukas Neverauskis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef WUCY_HAL_INCLUDE_HAL_H_
#define WUCY_HAL_INCLUDE_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{ MALLOC_SPECIALIZED_DMA} w_malloc_type_e;

typedef enum {W_PIN_DIR_DISABLED = 0, W_PIN_DIR_INPUT, W_PIN_DIR_OUTPUT} w_pin_dir_e;

typedef enum {W_PIN_PULL_UP = 0, W_PIN_PULL_DOWN, W_PIN_PULL_UP_DOWN, W_PIN_PULL_FLOATING} w_pin_pull_e;

/* HAL (lower abstraction) layer  */

/* Common User HAL */
void wucy_hal_PinInit(uint32_t pin, w_pin_dir_e direction, w_pin_pull_e pull_mode);
void wucy_hal_PinWrite(uint32_t pin, uint8_t state);
uint8_t wucy_hal_PinRead(uint32_t pin);
void wucy_hal_DelayMs(uint32_t ms);
void * wucy_hal_SpecMalloc(w_malloc_type_e malloc_type, uint32_t size);

/* User SPI HAL */
void wucy_hal_SPI_Init(void);
void wucy_hal_SPI_DeInit(void);
void wucy_hal_SPI_Transmit(uint8_t * data, uint32_t size, uint8_t dc);

/* User parallel interface HAL */
void wucy_hal_Parallel_Init(void);
void wucy_hal_Parallel_DeInit(void);
void wucy_hal_Parallel_Transmit(uint8_t * data, uint32_t size, uint8_t dc);


#ifdef __cplusplus
}
#endif

#endif /* WUCY_HAL_INCLUDE_HAL_H_ */
