/*
 * hal.c
 *
 *  Created on: 2019-09-17
 *      Author: lukas.neverauskis
 */

/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
	<https://github.com/therram/wucy>

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

#define WUCY_PRIV_ACCESS

#include "ssd1351.h"

#include <wucy_api.h>




/* USER PRIVATE INCLUDES START */

	/* USER PRIVATE INCLUDES END */

ssd1351_t ssd1351;

ssd1351_spi_t ssd1351_Interface = {

	.CLK = 	19,
	.RES = 	21,
	.DC = 	22,
	.MOSI = 23,
	.MISO = -1, /* -1 for "not used" */
	.EN = 	-1,	/* -1 for "not used" */
	.CS = 	-1 	/* used to be 22, but because this OLED will constantly
				stream data at full speed, no other device should be
				conencted, therefore chip select pin becomes
				redunant and should be just tied to ground. */

};


/*
 * common types
 *  pixel_vram_t
 *  window_t
 *  c_hex_t
 *  gfx_pos_t
 */

// ============================================================================
//								display driver module
//  ===========================================================================

inline void wucy_disp_RenderNewFrame(uint8_t * data) {
	ssd1351_SendData(&ssd1351, data, DISP_REF_W, DISP_REF_H, DISP_RANGE_W, DISP_RANGE_H);
}

inline int8_t wucy_disp_Init(void) {
	return ssd1351_InitViaSPI(&ssd1351, &ssd1351_Interface);
}

inline void wucy_disp_DeInit(void) {
	ssd1351_DeInit(&ssd1351);
}

inline void wucy_disp_PowerUp(void) {
	ssd1351_PowerOn(&ssd1351);
}

inline void wucy_disp_PowerDown(void) {
	ssd1351_PowerOff(&ssd1351);
}

inline void wucy_disp_PixelsAllOn(void) {
	ssd1351_DisplayMode(&ssd1351, DISPLAY_ALL_ON);
}

inline void wucy_disp_PixelsAllOff(void) {
	ssd1351_DisplayMode(&ssd1351, DISPLAY_ALL_OFF);
}

inline void wucy_disp_PixelsFromRAM(void) {
	ssd1351_DisplayMode(&ssd1351, DISPLAY_NORMAL);
}

inline void wucy_disp_PixelsFromRAMInverse(void) {
	ssd1351_DisplayMode(&ssd1351, DISPLAY_INVERSE);
}

inline void wucy_disp_Sleep(void) {
	ssd1351_Sleep(&ssd1351, SLEEP_ON);
}

inline void wucy_disp_WakeUp(void) {
	ssd1351_Sleep(&ssd1351, SLEEP_OFF);
}

inline uint16_t wucy_disp_HEXcolor2DispPixelData(uint32_t hexColor) {
	return ssd1351_color_HEX2PixelData(hexColor);
}



// ============================================================================
//								window module
//  ===========================================================================
/*

int8_t wucy_wnd_FramingStart(uint8_t fps) {
	return window_FramingStart(&Windows, fps);
}

int8_t wucy_wnd_FramingStop(void) {
	return window_FramingStop(&Windows);
}

inline void wucy_wnd_MainframeSetAll(void) {
	window_mf_SetAll(&Windows);
}
inline void wucy_wnd_MainframeClearAll(void) {
	window_mf_ClearAll(&Windows);
}

inline int8_t wucy_wnd_Init(void) {
	return window_Init(&Windows);
}

inline int8_t wucy_wnd_DeInit(void) {
	return window_DeInit(&Windows);
}


inline int8_t wucy_wnd_Create(window_t * wnd, wnd_fcn_t WndDrawFcn, layer_e layer,
		gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {
	return window_Create(&Windows, wnd, WndDrawFcn, layer, x, y, w, h);
}

inline int8_t wucy_wnd_Delete(window_t * wnd) {
	return window_Delete(&Windows, wnd);
}

inline void wucy_wnd_RedrawRequest(window_t * wnd) {
	window_RedrawRequest(wnd);
}

inline void wucy_wnd_PixelSet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y, pixel_vram_t data) {
	window_PixelSet(wnd, x, y, data);
}

inline pixel_vram_t wucy_wnd_PixelGet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y) {
	return window_PixelGet(wnd, x, y);
}

inline void wucy_wnd_SetPosition(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y) {
	window_SetPosition(wnd, x, y);
}

inline int8_t wucy_wnd_SetDimensions(window_t * wnd,
		gfx_pos_t w, gfx_pos_t h) {
	return window_SetDimensions(wnd, w, h);
}

inline gfx_geo_t wucy_wnd_GeoGet(window_t * wnd) {
	return window_GeoGet(wnd);
}
*/


// ============================================================================
//								gfx module
//  ===========================================================================

/*
inline void wucy_gfx_SetAll(window_t * wnd) {
	gfx_SetAll(wnd);
}
inline void wucy_gfx_ClearAll(window_t * wnd) {
	gfx_ClearAll(wnd);
}
inline void wucy_gfx_DrawSinglePixel(window_t * wnd,
		c_hex_t color, gfx_pos_t x, gfx_pos_t y) {
	gfx_draw_SinglePixel(wnd, color, x, y);
}
inline void wucy_gfx_DrawHLine(window_t * wnd, pixel_vram_t pxl_data,
		gfx_pos_t x, gfx_pos_t y, gfx_pos_t length) {
	gfx_draw_HLine(wnd, pxl_data, x, y, length);
}
inline void wucy_gfx_DrawWindowFrame(window_t * wnd,
		c_hex_t color, gfx_pos_t thickness) {
	gfx_draw_WindowFrame(wnd, color, thickness);
}
inline void wucy_gfx_DrawRect(window_t * wnd, c_hex_t color,
		gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {
	gfx_draw_Rectangle(wnd, color, ref, x, y, w, h);
}
inline void wucy_gfx_DrawBox(window_t * wnd, c_hex_t color,
		gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {
	gfx_draw_Box(wnd, color, ref, x, y, w, h);
}
*/


