/*
 * config.h
 *
 *  Created on: 2019-09-30
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

#ifndef WUCY_KERNEL_INCLUDE_WUCY_CONFIG_H_
#define WUCY_KERNEL_INCLUDE_WUCY_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define WND_CLEAR_WINDOWS
#define DISP_MAX_AVAILABLE_FPS 100 /* range:[0 255] */
#define DISP_SPI_CLOCK_FREQ 14500000

#ifdef __cplusplus
}
#endif

#endif /* WUCY_KERNEL_INCLUDE_WUCY_CONFIG_H_ */
