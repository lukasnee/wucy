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



static void window_Layering(windows_t * windows) {

		/* todo add transperancy merging feature */

	while(windows->Status.Framing) {

		window_mf_ClearAll(windows);

		for (window_t * wndCur = windows->List.tqh_first; wndCur != NULL;
				wndCur = wndCur->Windows.tqe_next) {

			if(wndCur->RedrawCondition) {

				wndCur->RedrawFcn(NULL);
				wndCur->RedrawCondition = 0;
			}

			for(gfx_pos_t y_wnd = 0, y_mf = wndCur->geo.y;
					y_mf < (wndCur->geo.y + ((wndCur->geo.h > DISP_HEIGHT) ? DISP_HEIGHT : wndCur->geo.h));
					y_mf++) {

				for(gfx_pos_t x_wnd = 0, x_mf = wndCur->geo.x;
						x_mf < (wndCur->geo.x + ((wndCur->geo.w > DISP_WIDTH) ? DISP_WIDTH : wndCur->geo.w));
						x_mf++) {

					window_mf_SetPixel(windows, x_mf, y_mf, wucy_wnd_PixelGet(wndCur, x_wnd, y_wnd));

					x_wnd++;
				}

				y_wnd++;
			}
		}

		windows->Status.LayeringDone = 1;
		vTaskResume(windows->FramingTask.Handler);

		wucy_hal_PinWrite(25, 0);
		vTaskSuspend(NULL);
		wucy_hal_PinWrite(25, 1);

	}
	vTaskDelete(NULL);
}



static void window_Rendering(windows_t * windows) {

	while(windows->Status.Framing) {

		/* begin timer for limiter */
		wucy_hal_PinWrite(27, 0);
		if(xTimerIsTimerActive(windows->Status.FPSLimiter_th) == pdFALSE)
			xTimerStart(windows->Status.FPSLimiter_th, 0);

		/* begin transmission */
		wucy_disp_RenderNewFrame((uint8_t *)VRAM_SEND);
		/*
		 * ...
		 *
		 * task idles here until transmission is finished
		 *
		 * ...
		 *
		 */

		windows->Status.TransmissionDone = 1;
		vTaskResume(windows->FramingTask.Handler);

		if(windows->Status.FpsLimterAllows)
			vTaskDelay(5);

		wucy_hal_PinWrite(26, 0);
		vTaskSuspend(NULL);
		wucy_hal_PinWrite(26, 1);
	}
	vTaskDelete(NULL);
}



static void window_Framing(windows_t * windows) {

	while(windows->Status.Framing) {

		if (windows->Status.FirstFrame
				|| ( windows->Status.LayeringDone &&
					windows->Status.TransmissionDone &&
					windows->Status.FpsLimterAllows)
					) {

			windows->Status.FirstFrame = 0;

			/* reset framing condition flags */
			windows->Status.LayeringDone = 0;
			windows->Status.TransmissionDone = 0;
			windows->Status.FpsLimterAllows = 0;

			/* switch frame buffer state */
			windows->Mainframe.State = !windows->Mainframe.State;

			/* Before rendering new frame to display,
			 * enable layering task which has lower priority by a single step.
			 * Eventualy current task will pause on transmission and pass on to layering task */

			vTaskResume(windows->RenderingTask.Handler);
			vTaskResume(windows->LayeringTask.Handler);

		}

		vTaskSuspend(NULL);

	}
	vTaskDelete(NULL);
}



static void window_FPSLimiterCallback(TimerHandle_t xTimer) {

	wucy_hal_PinWrite(27, 1);
	windows_t * windows = pvTimerGetTimerID(xTimer);
	windows->Status.FpsLimterAllows = 1;

	if (eTaskGetState(windows->FramingTask.Handler) == eSuspended)
		vTaskResume(windows->FramingTask.Handler);
}




int8_t window_FramingStart(windows_t * windows, uint8_t fps) {

	//window_FramingStop(windows); /* clean windows framing handle */

	windows->Status.Framing = 1;
	windows->Status.FirstFrame = 1;
	windows->Config.Fps = fps ? fps : DISP_MAX_AVAILABLE_FPS;

	windows->Status.FPSLimiter_th = xTimerCreate("FPSLimiter\0",
			1000 / windows->Config.Fps / portTICK_PERIOD_MS, pdFALSE, 0, window_FPSLimiterCallback);

	vTimerSetTimerID(windows->Status.FPSLimiter_th, windows);

	windows->LayeringTask.Fcn = (void (*)(void *))window_Layering;
	windows->RenderingTask.Fcn = (void (*)(void *))window_Rendering;
	windows->FramingTask.Fcn = (void (*)(void *))window_Framing;

	xTaskCreate(windows->LayeringTask.Fcn, "wnds_lyr\0", 1024, windows, WUCY_WNDS_LAYER_TASK_PRIOR, &(windows->LayeringTask.Handler));
	vTaskSuspend(windows->LayeringTask.Handler);
	xTaskCreate(windows->RenderingTask.Fcn, "wnds_rndr\0", 1024, windows, WUCY_WNDS_RENDER_TASK_PRIOR, &(windows->RenderingTask.Handler));
	vTaskSuspend(windows->LayeringTask.Handler);
	xTaskCreate(windows->FramingTask.Fcn, "wnds_frm\0", 1024, windows, WUCY_WNDS_FRAME_TASK_PRIOR, &(windows->FramingTask.Handler));
	return 0; /* success */
}



int8_t window_FramingStop(windows_t * windows) {

	xTimerDelete(windows->Status.FPSLimiter_th, 0);
	vTaskDelete(windows->FramingTask.Handler);
	vTaskDelete(windows->LayeringTask.Handler);
	vTaskDelete(windows->RenderingTask.Handler);

	windows->Status.Framing = 0;

	memset(&(windows->Status), 0x00, sizeof(wnd_state_t));

	return 0; /* success */
}



inline void window_mf_SetAll(windows_t * windows) {

	memset((uint8_t*)VRAM_DRAW, 0xFF, DISP_FRAMEBUFF_SIZE);

}



inline void window_mf_ClearAll(windows_t * windows) {

	memset((uint8_t*)VRAM_DRAW, 0x00, DISP_FRAMEBUFF_SIZE);

}




int8_t window_Init(windows_t * windows) {

	windows->Status.Framing = 0;

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

	windows->Status.Framing = 0;

	free(windows->Mainframe.Ping);
	free(windows->Mainframe.Pong);

	memset(windows, 0x00, sizeof(windows_t));

	return 1;
}



/*
uint8_t window_NewFrame(windows_t * windows) {

	static uint8_t prevState = PING_SEND_PONG_DRAW;

	 display state is flipped when frame buffer transmission finishes
	if(prevState != windows->Mainframe.State) {

		prevState = windows->Mainframe.State;

		return 1;  success: new frame began rendering on display
	}

	return 0;  error: previous frame rendering not yet finished
}



void window_UpdateAll(windows_t * windows){

	windows->Mainframe.State = !windows->Mainframe.State;

	window_Layering(windows);
	wucy_disp_RenderNewFrame((uint8_t *)VRAM_SEND);

}
*/




int8_t window_Create(windows_t * windows, window_t * wnd,  wnd_fcn_t WndDrawFcn,
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

	wnd->RedrawCondition = 1;
	wnd->RedrawFcn = WndDrawFcn;

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



void window_RedrawRequest(window_t * wnd) {

	wnd->RedrawCondition = 1;

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
