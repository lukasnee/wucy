/*
 * gfx.c
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


#include "gfx.h"

void fill_block_bg(ssd1351_t * display, c_hex_t color, pxl_pos_t x, pxl_pos_t y, pxl_pos_t w, pxl_pos_t h) {

	pixel_vram_t pxl_data = 0;

	int16_t xt = 0, yt = 0, dx = 0, dy = 0;

	if(x <= SSD1351_RANGE_W && y <= SSD1351_RANGE_H){

		pxl_data = color_hex2PixelVRAM(color);

		if(w < 0) xt = x + w;
		else xt = x;

		if(h < 0) yt = y + h;
		else yt = y;

		dx = xt + abs(w);
		if(dx > SSD1351_RANGE_W) dx = SSD1351_RANGE_W;

		dy = yt + abs(h);
		if(dy > SSD1351_RANGE_H) dy = SSD1351_RANGE_H;
	}

	for(uint16_t yc = yt; yc <= dy; yc++) {

		for(uint16_t xc = xt; xc <= dx; xc++) {

			ssd1351_PixelDataSet(display->MainFrame, xc, yc, pxl_data);

		}
	}
}
//
