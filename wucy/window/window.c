/*
 * window.c
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

#define WINDOW_PRIV_ACCESS
#include "window.h"

#include "wucyOS.h"

static void window_mf_SetPixel(windows_t * windows,
		gfx_pos_t x, gfx_pos_t y, pixelData_t data) {

	if(x >= 0 && y >= 0 && x < windows->Mainframe.geo.w && y < windows->Mainframe.geo.h) {

		*(VRAM_DRAW + windows->Mainframe.geo.w * y + x) = data;

	}
}

static pixelData_t window_mf_GetPixel(windows_t * windows, gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < windows->Mainframe.geo.w && y < windows->Mainframe.geo.h) {

		return *(VRAM_DRAW + windows->Mainframe.geo.w * y + x);
	}
	return 0;
}



static int8_t window_LayerWindows(windows_t * windows) {

	/* todo add transperancy merging feature */

	for (window_t * wndCur = windows->List.tqh_first; wndCur != NULL;
			wndCur = wndCur->Windows.tqe_next) {

		for(gfx_pos_t y_wnd = 0, y_mf = wndCur->geo.y;
				y_mf < wndCur->geo.y + (wndCur->geo.h > DISP_HEIGHT ? DISP_HEIGHT : wndCur->geo.h);
				y_mf++) {

			for(gfx_pos_t x_wnd = 0, x_mf = wndCur->geo.x;
					x_mf < wndCur->geo.x + (wndCur->geo.w > DISP_WIDTH ? DISP_WIDTH : wndCur->geo.w);
					x_mf++) {

				window_mf_SetPixel(windows, x_mf, y_mf, wucy_wnd_PixelGet(wndCur, x_wnd, y_wnd));

				x_wnd++;
			}

			y_wnd++;
		}
	}
	return 1;
}



inline void window_mf_SetAll(windows_t * windows) {

	memset(VRAM_DRAW, 0xFF, DISP_FRAMEBUFF_SIZE);

}



inline void window_mf_ClearAll(windows_t * windows) {

	memset(VRAM_DRAW, 0x00, DISP_FRAMEBUFF_SIZE);

}




int8_t window_Init(windows_t * windows) {

	windows->Mainframe.Ping = wucy_hal_Malloc(MALLOC_SPECIALIZED_DMA, DISP_FRAMEBUFF_SIZE);
	windows->Mainframe.Pong = wucy_hal_Malloc(MALLOC_SPECIALIZED_DMA, DISP_FRAMEBUFF_SIZE);
	windows->Mainframe.geo.x = 0;
	windows->Mainframe.geo.y = 0;
	windows->Mainframe.geo.w = DISP_WIDTH;
	windows->Mainframe.geo.h = DISP_HEIGHT;

	if(windows->Mainframe.Ping != NULL && windows->Mainframe.Pong != NULL) {

		return 0;
	}
	else {
		window_DeInit(windows);
		return -1;
	}
}



int8_t window_DeInit(windows_t * windows) {

	free(windows->Mainframe.Ping);
	free(windows->Mainframe.Pong);

	memset(windows, 0x00, sizeof(windows_t));

	return 1;
}



uint8_t window_NewFrame(windows_t * windows) {

	static uint8_t prevState = PING_SEND_PONG_DRAW;

	/* display state is flipped when frame buffer transmission finishes */
	if(prevState != windows->Mainframe.State) {

		prevState = windows->Mainframe.State;

		return 1; /* success: new frame began rendering on display */
	}

	return 0; /* error: previous frame rendering not yet finished */
}



void window_UpdateAll(windows_t * windows){

	windows->Mainframe.State = !windows->Mainframe.State;

	window_LayerWindows(windows);
	wucy_disp_RenderNewFrame((uint8_t *)VRAM_SEND);

}




int8_t window_Create(windows_t * windows, window_t * wnd,
		layer_e layer, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h){

	wnd->FrameBuff = wucy_hal_Malloc(MALLOC_SIMPLE, w * h * DISP_PIXEL_SIZE);

	if(wnd == NULL) {
		return -1; /* error: heap overflow */
	}

	wnd->geo.x = x;
	wnd->geo.y = y;
	wnd->geo.w = w;
	wnd->geo.h = h;
	wnd->layer = layer;

	if(windows->List.tqh_first == NULL) {

		TAILQ_INIT(&windows->List);

		TAILQ_INSERT_HEAD(&windows->List, wnd, Windows);

		return 1; /* success: inserted at head */
	}

	for (window_t * wndCur = windows->List.tqh_first; wndCur != NULL;
			wndCur = wndCur->Windows.tqe_next) {

		/* sort from lowest to highest head to tail */
		if(wnd->layer < wndCur->layer) {

			/* todo make so it adds before higher layer, after same layer,
			 * so the same number layers have priority by creation time */

			TAILQ_INSERT_BEFORE(wndCur, wnd, Windows);
			return 2; /* success: inserted between head and tail */
		}
	}

	TAILQ_INSERT_TAIL(&windows->List, wnd, Windows);

	return 3; /* success: inserted at tail */

}
int8_t window_Delete(windows_t * windows, window_t * wnd){

	free(wnd->FrameBuff);
	TAILQ_REMOVE(&windows->List, wnd, Windows);
	memset(wnd, 0x00, sizeof(window_t));

	return 1;
}



/* for general window types */
inline void window_PixelSet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y, pixelData_t data) {

	if(x >= 0 && y >= 0 && x < wnd->geo.w && y < wnd->geo.h) {

		*( wnd->FrameBuff + wnd->geo.w * y + x) = data;

	}
}



/* for general window types */
inline pixelData_t window_PixelGet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < wnd->geo.w && y < wnd->geo.h) {

	return *( wnd->FrameBuff + wnd->geo.w * y + x);

	}
	return 0;
}



void window_SetPosition(window_t * wnd, gfx_pos_t x, gfx_pos_t y) {

	wnd->geo.x = x;
	wnd->geo.y = y;

}



int8_t window_SetDimensions(window_t * wnd, gfx_pos_t w, gfx_pos_t h) {

	free(wnd->FrameBuff);

	wnd->FrameBuff = wucy_hal_Malloc(MALLOC_SIMPLE, w * h * DISP_PIXEL_SIZE);

	if(wnd == NULL) {

		wnd->FrameBuff = wucy_hal_Malloc(MALLOC_SIMPLE, wnd->geo.w * wnd->geo.h * DISP_PIXEL_SIZE);

		if(wnd == NULL){

			return -2; /* critical error: frame buffer lost */
		}

		return -1; /* error: new frame buffer dimensions are to high for new buffer allocation */
	}

	wnd->geo.w = w;
	wnd->geo.h = h;

	return 1;

}



inline gfx_geo_t window_GeoGet(window_t * wnd) {

	return wnd->geo;

}
