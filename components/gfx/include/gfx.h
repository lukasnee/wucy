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

void fill_block_bg(ssd1351_t * display, c_hex_t color, pxl_pos_t x, pxl_pos_t y, pxl_pos_t w, pxl_pos_t h);
#endif /* COMPONENTS_SSD1351_FAST_H_ */



#endif /* COMPONENTS_GFX_INCLUDE_GFX_H_ */
