/*
 * wucyOS.hpp
 *
 *  Created on: 2019-10-01
 *      Author: lukas.neverauskis
 */
/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
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


#ifndef WUCY_KERNEL_INCLUDE_WUCYOS_H_
#define WUCY_KERNEL_INCLUDE_WUCYOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "wucy_config.h"

#define WUCY_OS
#define WUCY_OPTIMIZE

#define DISP_WIDTH 128
#define DISP_HEIGHT 128

#define DISP_REF_H 0
#define DISP_REF_W 0

#define DISP_RANGE_H (DISP_WIDTH - 1)
#define DISP_RANGE_W (DISP_HEIGHT - 1)

#define DISP_TOTAL_PIXELS (DISP_WIDTH * DISP_HEIGHT)
#define DISP_PIXEL_SIZE 2 /* in bytes */

#define DISP_FRAMEBUFF_SIZE (DISP_TOTAL_PIXELS * DISP_PIXEL_SIZE)

#include "wucy_hal.h"


/* wucy integrated modules */

typedef enum{

    WUCY_KERNEL_MAX_PRIOR = 10,

	WUCY_PRIOR_HIGHEST = WUCY_KERNEL_MAX_PRIOR - 3,
	WUCY_PRIOR_HIGH = WUCY_PRIOR_HIGHEST- 1,
	WUCY_PRIOR_NORMAL = WUCY_PRIOR_HIGH - 1,
	WUCY_PRIOR_LOW = WUCY_PRIOR_NORMAL - 1,
	WUCY_PRIOR_LOWEST = WUCY_PRIOR_LOW - 1,

	WUCY_WNDS_RENDER_TASK_PRIOR = WUCY_KERNEL_MAX_PRIOR - 1,
	WUCY_WNDS_FRAME_TASK_PRIOR = WUCY_KERNEL_MAX_PRIOR - 1,
	WUCY_WNDS_LAYER_TASK_PRIOR = WUCY_KERNEL_MAX_PRIOR - 1,


}wucy_rtos_taskPrior_e;

// ============================================================================
//								display driver module
//  ===========================================================================

void wucy_disp_RenderNewFrame(uint8_t * data);
int8_t wucy_disp_Init(void);
void wucy_disp_DeInit(void);
void wucy_disp_PowerUp(void);
void wucy_disp_PowerDown(void);
void wucy_disp_PixelsAllOn(void);
void wucy_disp_PixelsAllOff(void);
void wucy_disp_PixelsFromRAM(void);
void wucy_disp_PixelsFromRAMInverse(void);
void wucy_disp_Sleep(void);
void wucy_disp_WakeUp(void);
void wucy_disp_VRAM_SetAll(void);
void wucy_disp_VRAM_ClearAll(void);
uint16_t wucy_disp_HEXcolor2DispPixelData(uint32_t hexColor);

#ifdef __cplusplus
}
#endif


#endif /* WUCY_KERNEL_INCLUDE_WUCYOS_H_ */
