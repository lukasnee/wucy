
/*
 * kernel.c
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

#include <wucy.h>

#include <wucy_api.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

void wucy_ui_Init(void); /* extern from ui.cpp */

void wucy_system(void * p) {

	/* debug init */

//	wucy_hal_PinInit(25, W_PIN_DIR_OUTPUT, W_PIN_PULL_DOWN);
//	wucy_hal_PinWrite(25, 0);
//
//	wucy_hal_PinInit(26, W_PIN_DIR_OUTPUT, W_PIN_PULL_DOWN);
//	wucy_hal_PinWrite(26, 0);

	wucy_hal_PinInit(27, W_PIN_DIR_OUTPUT, W_PIN_PULL_DOWN);
	wucy_hal_PinWrite(27, 0);



	/* system init */

	wucy_disp_Init();

	wucy_ui_Init();



	/* system routine */

	// !!!!! pin 2 used for SD card DAT0

//	wucy_hal_PinInit(2, W_PIN_DIR_OUTPUT, W_PIN_PULL_FLOATING);
//	wucy_hal_PinWrite(2, 0);

	uint8_t heartbeat = 0;

	while(1) {
//
//		wucy_hal_PinWrite(2, heartbeat ^= 1);

		vTaskDelay(250 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);

}

void wucy_Start(void){

	xTaskCreate(wucy_system, "wucy_sys\0", 1024*3, NULL, WUCY_KERNEL_MAX_PRIOR, NULL);

}


