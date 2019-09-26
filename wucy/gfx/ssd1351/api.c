/*
 * hal.c
 *
 *  Created on: 2019-09-17
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

#include "api.h"
#include "ssd1351.h"

/* USER PRIVATE INCLUDES START */

	/* USER PRIVATE INCLUDES END */




inline void HAL_PinOutputInit(uint32_t pin) {

	USER_HAL_COM_PinOutputInit(pin);
}



inline void HAL_PinWrite(ssd1351_t * disp, uint32_t pin, uint8_t state){

	switch(disp->Status.Interface){

	case SSD1351_IF_SPI:

		if(pin == disp->Pin.SPI->EN && disp->Status.EN_Polarity == EN_ACTIVE_LOW) {
			state = !state;
		}

		break;

	case SSD1351_IF_8_BIT:

		if(pin == disp->Pin.Prll->EN && disp->Status.EN_Polarity == EN_ACTIVE_LOW) {
			state = !state;
		}

		break;

	}
	USER_HAL_COM_PinWrite(pin, state);
}



inline void HAL_DelayMs(uint32_t ms) {

	USER_HAL_COM_DelayMs(ms);
}

inline pixel_vram_t * HAL_Malloc(malloc_type_e malloc_type, uint32_t size){

		return (pixel_vram_t *)USER_HAL_Malloc(malloc_type, size);

}

inline void HAL_Init(ssd1351_t * disp) {

	switch(disp->Status.Interface){

	case SSD1351_IF_SPI:

		HAL_PinOutputInit(disp->Pin.SPI->RES);
		HAL_PinOutputInit(disp->Pin.SPI->CS);
		HAL_PinOutputInit(disp->Pin.SPI->DC);
		HAL_PinOutputInit(disp->Pin.SPI->EN);

		USER_HAL_SPI_Init();

		break;

	case SSD1351_IF_8_BIT:

		HAL_PinOutputInit(disp->Pin.Prll->RES);
		HAL_PinOutputInit(disp->Pin.Prll->CS);
		HAL_PinOutputInit(disp->Pin.Prll->DC);
		HAL_PinOutputInit(disp->Pin.Prll->EN);
		HAL_PinOutputInit(disp->Pin.Prll->WR);
		HAL_PinOutputInit(disp->Pin.Prll->RD);

		USER_HAL_Parallel_Init();

		break;
	}


	disp->FrameBuff.Ping = HAL_Malloc(MALLOC_SPECIALIZED_DMA, VRAM_SIZE);
	disp->FrameBuff.Pong = HAL_Malloc(MALLOC_SPECIALIZED_DMA, VRAM_SIZE);
	disp->FrameBuff.geo.x = 0;
	disp->FrameBuff.geo.y = 0;
	disp->FrameBuff.geo.w = SSD1351_WIDTH;
	disp->FrameBuff.geo.h = SSD1351_HEIGHT;

    TAILQ_INIT(&WindowListhead);

	//memset(disp->WindowList, NULL, WINDOW_MAX_AMOUNT);

}


inline void HAL_DeInit(ssd1351_t * disp) {

	switch(disp->Status.Interface){

	case SSD1351_IF_SPI:

		USER_HAL_SPI_DeInit();

		break;

	case SSD1351_IF_8_BIT:

		USER_HAL_Parallel_DeInit();

		break;

	}
}

inline void HAL_Transmit(ssd1351_t * disp, dc_e dc, uint8_t * data, uint32_t size) {

	switch(disp->Status.Interface){

	case SSD1351_IF_SPI:

		USER_HAL_SPI_Transmit(disp, data, size, dc);

		break;

	case SSD1351_IF_8_BIT:

		USER_HAL_Parallel_Transmit(disp, data, size, dc);

		break;

	}
}
