/*
 * gfx.h
 *
 *  Created on: 2019-09-24
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
#ifndef COMPONENTS_GFX_INCLUDE_GFX_H_
#define COMPONENTS_GFX_INCLUDE_GFX_H_

#include "ssd1351.h"

typedef enum{

	REF_BOTTOM_L, /* default - left bottom left */
	REF_BOTTOM_R,
	REF_TOP_L,
	REF_TOP_R,
	REF_CENTER

}gfx_ref_e;

void gfx_draw_SinglePixel(window_t * wnd, c_hex_t color, gfx_pos_t x, gfx_pos_t y);

void gfx_draw_HLine(window_t * wnd, pixel_vram_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length);

void gfx_draw_VLine(window_t * wnd, pixel_vram_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length);

void gfx_draw_Rectangle(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);

void gfx_draw_Box(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);




#endif /* COMPONENTS_GFX_INCLUDE_GFX_H_ */
