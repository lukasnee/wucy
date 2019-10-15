/*
 * wucyOS.h
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

#include "hal.h"

/* wucy integrated modules */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"



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


}WUCY_RTOS_TASK_PRIORITES;


/* wucy integrated custom modules */

#ifdef WUCY_PRIV_ACCESS
	#define WINDOW_PRIV_ACCESS
	#define GFX_PRIV_ACCESS
	#define SSD1351_PRIV_ACCESS
#endif

#include "window.h"
#include "gfx.h"
#include "ssd1351.h"

#include "ui.h"

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
void wucy_disp_PixelSet(gfx_pos_t x, gfx_pos_t y, pixel_vram_t data);
pixel_vram_t wucy_disp_PixelGet(gfx_pos_t x, gfx_pos_t y, pixel_vram_t data);
pixel_vram_t wucy_disp_HEXcolor2DispPixelData(uint32_t hexColor);

// ============================================================================
//								window/GFX module
//  ===========================================================================

int8_t wucy_wnd_FramingStart(uint8_t fps);
int8_t wucy_wnd_FramingStop(void);

void wucy_wnd_MainframeSetAll(void);
void wucy_wnd_MainframeClearAll(void);
int8_t wucy_wnd_Init(void);
int8_t wucy_wnd_DeInit(void);
/*
uint8_t wucy_wnd_NewFrame(void);
void wucy_wnd_RenderFrame(void);
*/
int8_t wucy_wnd_Create(window_t * wnd, wnd_fcn_t WndDrawFcn, layer_e layer, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);
int8_t wucy_wnd_Delete(window_t * wnd);
void wucy_wnd_RedrawRequest(window_t * wnd);
void wucy_wnd_PixelSet(window_t * wnd, gfx_pos_t x, gfx_pos_t y, pixel_vram_t data);
pixel_vram_t wucy_wnd_PixelGet(window_t * wnd, gfx_pos_t x, gfx_pos_t y);
void wucy_wnd_SetPosition(window_t * wnd, gfx_pos_t x, gfx_pos_t y);
int8_t wucy_wnd_SetDimensions(window_t * wnd, gfx_pos_t w, gfx_pos_t h);
gfx_geo_t wucy_wnd_GeoGet(window_t * wnd);


void wucy_gfx_SetAll(window_t * wnd);
void wucy_gfx_ClearAll(window_t * wnd);
void wucy_gfx_DrawSinglePixel(window_t * wnd, c_hex_t color, gfx_pos_t x, gfx_pos_t y);
void wucy_gfx_DrawHLine(window_t * wnd, pixel_vram_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length);
void wucy_gfx_DrawWindowFrame(window_t * wnd, c_hex_t color, gfx_pos_t thickness);
void wucy_gfx_DrawRect(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);
void wucy_gfx_DrawBox(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);


// ============================================================================
//								wucyOS kernel
//  ===========================================================================


void wucy_Start(void);

#endif /* WUCY_KERNEL_INCLUDE_WUCYOS_H_ */
