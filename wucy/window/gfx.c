/*
 * gfx.c
 *
 *  Created on: 2019-09-24
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

#define GFX_PRIV_ACCESS
#define WINDOW_PRIV_ACCESS

#include "gfx.h"

#include "wucyOS.h"


const c_hex_t colorPalette16[16] = {

	COLOR_AQUA,
	COLOR_BLACK,
	COLOR_BLUE,
	COLOR_FUCHSIA,
	COLOR_GRAY,
	COLOR_GREEN,
	COLOR_LIME,
	COLOR_MAROON,
	COLOR_NAVY,
	COLOR_OLIVE,
	COLOR_PURPLE,
	COLOR_RED,
	COLOR_SILVER,
	COLOR_TEAL,
	COLOR_WHITE,
	COLOR_YELLOW

};

const c_hex_t colorPalette14[14] = {

	COLOR_AQUA,
	COLOR_BLUE,
	COLOR_FUCHSIA,
	COLOR_GRAY,
	COLOR_GREEN,
	COLOR_LIME,
	COLOR_MAROON,
	COLOR_NAVY,
	COLOR_OLIVE,
	COLOR_PURPLE,
	COLOR_RED,
	COLOR_SILVER,
	COLOR_TEAL,
	COLOR_YELLOW

};

static gfx_geo_t gfx_AdjustRectRef(gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {

	gfx_geo_t geo;

	switch(ref) {

	case REF_BOTTOM_L:

		/* default reference, theres nothing to change */

		break;

	case REF_BOTTOM_R:

		x = x - w + 1;

		break;

	case REF_TOP_L:

		y = y - h + 1;

		break;

	case REF_TOP_R:

		x = x - w + 1;
		y = y - h + 1;

		break;

	case REF_CENTER:

		x-= w/2;
		y-= h/2;

		break;
	}

	geo.x = x;
	geo.y = y;
	geo.w = w;
	geo.h = h;

	return geo;
}




inline void gfx_SetAll(window_t * wnd) {

	memset((uint8_t*)wnd->FrameBuff, 0xFF, wnd->geo.w * wnd->geo.h * DISP_PIXEL_SIZE);

}

inline void gfx_ClearAll(window_t * wnd) {

	memset((uint8_t*)wnd->FrameBuff, 0x00, wnd->geo.w * wnd->geo.h * DISP_PIXEL_SIZE);

}



/* INTENDED FOR SINGLE PIXEL COLORING, INEFFICIENT IF USED TO DRAW GRAPHICS WITH SOLID COLOR.
 * In this case to be efficient make a temporary pixelData_t variable and declare it with
 * color_hex2PixelVRAM(color_hex) to prepare formatted VRAM data,
 * then repeat ssd1351_PixelDataSet() to "draw" multiple pixels.
 * */
inline void gfx_draw_SinglePixel(window_t * wnd, c_hex_t color, gfx_pos_t x, gfx_pos_t y) {

	window_PixelSet(wnd, x, y, wucy_disp_HEXcolor2DispPixelData(color));

}

void gfx_draw_HLine(window_t * wnd, pixelData_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length) {

	for(gfx_pos_t xc = x; xc < x + length; xc++) {

		window_PixelSet(wnd, xc, y, pxl_data);
	}
}

void gfx_draw_VLine(window_t * wnd, pixelData_t pxl_data, gfx_pos_t x, gfx_pos_t y, gfx_pos_t length) {

	for(gfx_pos_t yc = y; yc < y + length; yc++) {

		window_PixelSet(wnd, x, yc, pxl_data);
	}
}

void gfx_draw_WindowFrame(window_t * wnd, c_hex_t color, gfx_pos_t thickness) {

	for(gfx_pos_t t = 0; t < thickness; t++) {

		gfx_draw_Rectangle(wnd, color, REF_BOTTOM_L,
				t,
				t,
				wnd->geo.w - t*2,
				wnd->geo.h - t*2);
	}
}





void gfx_draw_Rectangle(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {

	pixelData_t pxl_data = wucy_disp_HEXcolor2DispPixelData(color);

	gfx_geo_t geo = gfx_AdjustRectRef(ref, x, y, w, h);

	gfx_draw_HLine(wnd, pxl_data, geo.x, geo.y, geo.w);
	gfx_draw_HLine(wnd, pxl_data, geo.x, geo.y + geo.h - 1, w);

	gfx_draw_VLine(wnd, pxl_data, geo.x, geo.y, geo.h);
	gfx_draw_VLine(wnd, pxl_data, geo.x + geo.w - 1, geo.y, geo.h);

}






void gfx_draw_Box(window_t * wnd, c_hex_t color, gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {

	pixelData_t pxl_data = wucy_disp_HEXcolor2DispPixelData(color);

	gfx_geo_t geo = gfx_AdjustRectRef(ref, x, y, w, h);

	for(gfx_pos_t y = geo.y; y < geo.y + geo.h; y++) {

		gfx_draw_HLine(wnd, pxl_data, geo.x, y, geo.w);

	}
}
