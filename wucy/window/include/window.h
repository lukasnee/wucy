/*
 * window.h
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

#ifndef WUCY_GFX_INCLUDE_WINDOW_H_
#define WUCY_GFX_INCLUDE_WINDOW_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

typedef uint16_t pixelData_t;

/* pixel coordinate on the display type */
typedef int16_t gfx_pos_t;

typedef void (*wnd_fcn_t)( void * );

typedef struct{

	gfx_pos_t x; /* x position on display (reference to left edge of frame) */
	gfx_pos_t y; /* y position on display (reference to bottom edge of frame) */

	/* coordinates x and y together references bottom left corner of the frame */

	gfx_pos_t w; /* width of frame in pixels */
	gfx_pos_t h; /* height of frame in pixels */

}gfx_geo_t;

typedef enum {
	LAYER_VERY_BOTTOM = 0,
	LAYER_VERY_TOP = 0xFF

}layer_e;

typedef struct Window {

	gfx_geo_t geo; /* geometry */
	layer_e layer; /* frame priority: drawn behind higher priority frames
	and in front of lower priority frames */
	pixelData_t * FrameBuff;

	uint8_t RedrawCondition:1;
	wnd_fcn_t RedrawFcn;

    TAILQ_ENTRY(Window) Windows;

}window_t;


#ifdef WINDOW_PRIV_ACCESS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

typedef TAILQ_HEAD(, Window) windowsList_t;
typedef struct{

	gfx_geo_t geo; /* geometry */

	pixelData_t * Ping;	/* buffer 1 */
	pixelData_t * Pong;	/* buffer 2 */

	enum{ PING_DRAW_PONG_SEND,
		PING_SEND_PONG_DRAW} State:1;

}mainframe_t;

typedef struct{

	uint8_t Framing:1;
	uint8_t FirstFrame:1;

	TimerHandle_t 	FPSLimiter_th;

	uint8_t LayeringDone:1;
	uint8_t TransmissionDone:1;
	uint8_t FpsLimterAllows:1;

}wnd_state_t;

typedef enum{ FPS_LIMITER_RESET = 0,
	FPS_LIMITER_ELAPSED}fps_limiter_e;

typedef struct{

	uint8_t Fps;

}wnd_config_t;

typedef struct{

	wnd_fcn_t Fcn;
	TaskHandle_t Handler;

}wnds_task_t;

typedef struct{

	windowsList_t 	List;
	mainframe_t 	Mainframe; 	/* full frame data buffer */
	wnd_state_t 	Status;
	wnd_config_t 	Config;

	wnds_task_t LayeringTask;
	wnds_task_t FramingTask;
	wnds_task_t RenderingTask;

}windows_t;

#define VRAM_DRAW (!windows->Mainframe.State ? windows->Mainframe.Pong : windows->Mainframe.Ping)
#define VRAM_SEND ( windows->Mainframe.State ? windows->Mainframe.Pong : windows->Mainframe.Ping)

int8_t window_FramingStart(windows_t * windows, uint8_t fps);
int8_t window_FramingStop(windows_t * windows);

void window_mf_SetAll(windows_t * windows);
void window_mf_ClearAll(windows_t * windows);

int8_t window_Init(windows_t * windows);
int8_t window_DeInit(windows_t * windows);

int8_t window_Create(windows_t * windows, window_t * wnd, wnd_fcn_t WndDrawFcn, layer_e layer, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);
int8_t window_Delete(windows_t * windows, window_t * wnd);

void window_RedrawRequest(window_t * wnd);

void window_PixelSet(window_t * wnd, gfx_pos_t x, gfx_pos_t y, pixelData_t data);
pixelData_t window_PixelGet(window_t * wnd, gfx_pos_t x, gfx_pos_t y);

void window_SetPosition(window_t * wnd, gfx_pos_t x, gfx_pos_t y);
int8_t window_SetDimensions(window_t * wnd, gfx_pos_t w, gfx_pos_t h);

gfx_geo_t window_GeoGet(window_t * wnd);

#endif /* WINDOW_PRIV_ACCESS */
#endif /* WUCY_GFX_INCLUDE_WINDOW_H_ */
