/*
 * api.h
 *
 *  Created on: 2019-09-18
 *      Author: lukas.neverauskis
 */

/*
	ssd1351 - software driver for ssd1351 display driver.
	Portable, although designed for project WUCY. <https://github.com/therram/wucy>
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


#ifndef COMPONENTS_SSD1351_INCLUDE_API_H_
#define COMPONENTS_SSD1351_INCLUDE_API_H_

#include "ssd1351.h"

/* API layer */

extern void HAL_PinWrite(ssd1351_t * disp, uint32_t pin, uint8_t state);
extern void HAL_DelayMs(uint32_t ms);
extern pixel_vram_t * HAL_Malloc(malloc_type_e malloc_type, uint32_t size);
extern void HAL_Init(ssd1351_t * disp);
extern void HAL_DeInit(ssd1351_t * disp);
extern void HAL_Transmit(ssd1351_t * disp, dc_e dc, uint8_t * data, uint32_t size);




/* HAL (lower abstraction) layer  */

/* Common User HAL */
extern void USER_HAL_COM_PinOutputInit(uint32_t pin);
extern void USER_HAL_COM_PinWrite(uint32_t pin, uint8_t state);
extern void USER_HAL_COM_DelayMs(uint32_t ms);
extern void * USER_HAL_Malloc(malloc_type_e malloc_type, uint32_t size);

/* User SPI HAL */
extern void USER_HAL_SPI_Init(void);
extern void USER_HAL_SPI_DeInit(void);
extern void USER_HAL_SPI_Transmit(ssd1351_t * disp, uint8_t * data, uint32_t size, dc_e dc);

/* User parallel interface HAL */
extern void USER_HAL_Parallel_Init(void);
extern void USER_HAL_Parallel_DeInit(void);
extern void USER_HAL_Parallel_Transmit(ssd1351_t * disp, uint8_t * data, uint32_t size, dc_e dc);


#endif /* COMPONENTS_SSD1351_INCLUDE_API_H_ */
