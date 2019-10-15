/*
 * gfx.h
 *
 *  Created on: 2019-09-24
 *      Author: lukas.neverauskis
 */
/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
	<https://github.com/therram/wucy>

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
#ifndef COMPONENTS_GFX_INCLUDE_GFX_H_
#define COMPONENTS_GFX_INCLUDE_GFX_H_

#include <stdint.h>
#include <string.h>

#include "window.h"

/* format 0xAARRGGBB where 0xAA - alpha byte, 0xRR - Red byte, 0xGG - Green byte, 0xBB - blue byte. */
typedef enum{

	COLOR_AQUA = 	0xFF00FFFF,
	COLOR_BLACK = 	0xFF000000,
	COLOR_BLUE = 	0xFF0000FF,
	COLOR_FUCHSIA = 0xFFFF00FF,
	COLOR_GRAY = 	0xFF808080,
	COLOR_GREEN = 	0xFF008000,
	COLOR_LIME = 	0xFF00FF00,
	COLOR_MAROON = 	0xFF800000,
	COLOR_NAVY = 	0xFF000080,
	COLOR_OLIVE = 	0xFF808000,
	COLOR_PURPLE = 	0xFF800080,
	COLOR_RED = 	0xFFFF0000,
	COLOR_SILVER = 	0xFFC0C0C0,
	COLOR_TEAL = 	0xFF008080,
	COLOR_WHITE = 	0xFFFFFFFF,
	COLOR_YELLOW = 	0xFFFFFF00,

}c_hex_t;

const c_hex_t colorPalette16[16];
const c_hex_t colorPalette14[14];

typedef enum{

	REF_BOTTOM_L = 0, /* default - left bottom left */
	REF_BOTTOM_R,
	REF_TOP_L,
	REF_TOP_R,
	REF_CENTER

}gfx_ref_e;


#ifdef GFX_PRIV_ACCESS

void gfx_SetAll(window_t * wnd);
void gfx_ClearAll(window_t * wnd);

void gfx_draw_SinglePixel(window_t * wnd, c_hex_t color, gfx_pos_t x, gfx_pos_t y);
void gfx_draw_HLine(window_t * wnd, pixelData_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length);
void gfx_draw_VLine(window_t * wnd, pixelData_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length);

void gfx_draw_WindowFrame(window_t * wnd, c_hex_t color, gfx_pos_t thickness);

void gfx_draw_Rectangle(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);
void gfx_draw_Box(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);



#endif /* GFX_PRIV_ACCESS */

#endif /* COMPONENTS_GFX_INCLUDE_GFX_H_ */
