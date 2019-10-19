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

#include "window.hpp"

#include "wucyOS.h"

#include "Adafruit_GFX.h"

#define swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))



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


/* WINDOW */


int8_t Window::Window(gfx_pos_t w, gfx_pos_t h, wnd_fcn_t WndDrawFcn) {

	if(WndDrawFcn && w && h) {

		RedrawFcn = WndDrawFcn;

		return SetDimensions(w, h);
	}
	return -1;
}



void Window::~Window(void){

	free(FrameBuff);
}


void Window::clearDisplay(void) {

	memset((uint8_t*) FrameBuff, 0x00,
			GetW() * GetH() * DISP_PIXEL_SIZE);

}



void Window::setDisplay(void) {

	memset((uint8_t*) FrameBuff, 0xFF,
			GetW() * GetH() * DISP_PIXEL_SIZE);

}


/* INTENDED FOR SINGLE PIXEL COLORING, INEFFICIENT IF USED TO DRAW GRAPHICS WITH SOLID COLOR.
 * In this case to be efficient make a temporary pixelData_t variable and declare it with
 * color_hex2PixelVRAM(color_hex) to prepare formatted VRAM data,
 * then repeat ssd1351_PixelDataSet() to "draw" multiple pixels.
 * */
inline void Window::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
		// Pixel is in-bounds. Rotate coordinates if needed.
		switch (getRotation()) {
		case 1:
			swap(x, y);
			x = WIDTH - x - 1;
			break;
		case 2:
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
			break;
		case 3:
			swap(x, y);
			y = HEIGHT - y - 1;
			break;
		}

		if (x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

			*(FrameBuff + GetW() * y + x) = color;

		}
	}
}



void Window::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

	for(gfx_pos_t xc = x; xc < x + w; xc++) {

		drawPixel(xc, y, color);
	}
}



void Window::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

	for(gfx_pos_t yc = y; yc < y + h; yc++) {

		drawPixel(x, yc, color);
	}
}



inline pixelData_t Window::getPixel(int16_t x, int16_t y) {


	if(x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

	return *( FrameBuff + GetW() * y + x);

	}
	return 0;
}


int8_t Window::SetDimensions(gfx_pos_t w, gfx_pos_t h) {

	free(FrameBuff);

	FrameBuff = wucy_hal_Malloc(MALLOC_SIMPLE, w * h * DISP_PIXEL_SIZE);

	/* if unable to change buffer dimensions and previous dimensions were different */
	if(FrameBuff == NULL && GetW() != w && GetH() != h) {

		/* reallocate freed memory buffer */
		FrameBuff = wucy_hal_Malloc(MALLOC_SIMPLE, GetW() * GetH() * DISP_PIXEL_SIZE);

		/* if malloc failed */
		if(FrameBuff == NULL){

			return -2; /* critical error: frame buffer lost */
		}

		return -1; /* error: new frame buffer dimensions are to high for new buffer allocation */
	}

	SetW(w);
	SetH(h);

	return 0;

}

void Window::drawFrame(uint16_t color, gfx_pos_t thickness) {

	for(gfx_pos_t t = 0; t < thickness; t++) {

		drawRect(t, t, GetW() - t*2, GetH() - t*2, color);

	}
}

/*
Geo Window::AdjustRectRef(gfx_ref_e ref, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h) {

	Geo g;

	switch(ref) {

	case REF_BOTTOM_L:

		 default reference, theres nothing to change

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

	g.geo_x = x;
	g.geo_y = y;
	g.geo_w = w;
	g.geo_h = h;

	return g;
}
*/

/* 			MAINFRAME			 */



inline void Mainframe::SetPixel(gfx_pos_t x, gfx_pos_t y, pixelData_t data) {

	if(x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

		*(FBUFF_DRAW + GetW() * y + x) = data;

	}
}
inline pixelData_t Mainframe::GetPixel(gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

		return *(FBUFF_DRAW + GetW() * y + x);
	}
	return 0;
}



void Mainframe::Layering(void * p) {

	/* todo transperancy merging feature */

	while (Status.Framing) {

		ClearAll();

		for (auto wndCur = Windows.begin(); wndCur != Windows.end(); ++wndCur) {

			if (wndCur->RedrawCondition) {

				wndCur->RedrawFcn(NULL);
				wndCur->RedrawCondition = 0;
			}

			for (gfx_pos_t y_wnd = 0, y_mf = wndCur->GetY();
					y_mf < DISP_HEIGHT && y_mf < wndCur->GetY() + wndCur->GetH();
					y_mf++) {

				for (gfx_pos_t x_wnd = 0, x_mf = wndCur->GetX();
						x_mf < DISP_WIDTH && x_mf < wndCur->GetX() + wndCur->GetW();
						x_mf++) {

					SetPixel(x_mf, y_mf, wndCur->getPixel(x_wnd, y_wnd));

					x_wnd++;
				}
				y_wnd++;
			}
		}

		Status.LayeringDone = 1;
		vTaskResume(FramingTask);

		wucy_hal_PinWrite(25, 0);
		vTaskSuspend(NULL);
		wucy_hal_PinWrite(25, 1);

	}
	vTaskDelete(NULL);
}



void Mainframe::Rendering(void * p) {

	while(Status.Framing) {

		/* begin timer for limiter */
		wucy_hal_PinWrite(27, 0);
		if(xTimerIsTimerActive(Status.FPSLimiter_th) == pdFALSE)
			xTimerStart(Status.FPSLimiter_th, 0);

		/* begin transmission */
		wucy_disp_RenderNewFrame((uint8_t *)FBUFF_SEND);

		/* task idles here until transmission is finished */

		Status.TransmissionDone = 1;
		vTaskResume(FramingTask);

		if(Status.FpsLimterAllows)
			vTaskDelay(1);

		wucy_hal_PinWrite(26, 0);
		vTaskSuspend(NULL);
		wucy_hal_PinWrite(26, 1);
	}
	vTaskDelete(NULL);
}



void Mainframe::Framing(void * p) {

	while(Status.Framing) {

		if (Status.FirstFrame
				|| ( Status.LayeringDone &&
					Status.TransmissionDone &&
					Status.FpsLimterAllows)
					) {

			Status.FirstFrame = 0;

			/* reset framing condition flags */
			Status.LayeringDone = 0;
			Status.TransmissionDone = 0;
			Status.FpsLimterAllows = 0;

			/* switch frame buffer state */
			Status.BufferState = !Status.BufferState;

			/* Before rendering new frame to display,
			 * enable layering task which has lower priority by a single step.
			 * Eventualy current task will pause on transmission and pass on to layering task */

			vTaskResume(RenderingTask);
			vTaskResume(LayeringTask);

		}

		vTaskSuspend(NULL);

	}
	vTaskDelete(NULL);
}



void Mainframe::FPSLimiterCallback(TimerHandle_t xTimer) {

	wucy_hal_PinWrite(27, 1);

	Status.FpsLimterAllows = 1;

	if (eTaskGetState(FramingTask) == eSuspended)
		vTaskResume(FramingTask);
}



int8_t Mainframe::Mainframe(gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h){

	Status.Framing = 0;

	uint32_t buffSize = GetW() * GetH() * PIXEL_SIZE;

	Ping = wucy_hal_Malloc(MALLOC_SPECIALIZED_DMA, buffSize);
	Pong = wucy_hal_Malloc(MALLOC_SPECIALIZED_DMA, buffSize);


	if(Ping && Pong) {

		return 0; /* success */
	}

	delete this;
	return -1;
}



void Mainframe::~Mainframe() {

	Status.Framing = 0;

	free(Ping);
	free(Pong);

	~Geo();

}



int8_t Mainframe::AddWindow(Window * wnd, layer_e l, gfx_pos_t x, gfx_pos_t y) {


	wnd->layer = l;
	wnd->SetX(x);
	wnd->SetY(y);

	wnd->RedrawCondition = 1;
	//todo first thing to do adjust to c++ whats below

	/*





	if(List.tqh_first == NULL) {

		TAILQ_INIT(&List);

		TAILQ_INSERT_HEAD(&List, wnd, Windows);

		return 1;  success: inserted at head
	}

	for (window_t * wndCur = List.tqh_first; wndCur != NULL;
			wndCur = wndCur->Windows.tqe_next) {

		 sort from lowest to highest head to tail
		if(layer < wndCur->layer) {

			 todo make so it adds before higher layer, after same layer,
			 * so the same number layers have priority by creation time

			TAILQ_INSERT_BEFORE(wndCur, wnd, Windows);
			return 2;  success: inserted between head and tail
		}
	}

	TAILQ_INSERT_TAIL(&List, wnd, Windows);

	return 3;  success: inserted at tail
	*/

}



int8_t Mainframe::RemoveWindow(Window *wnd) {

}



int8_t Mainframe::FramingStart(uint8_t fps) {

	//window_FramingStop(windows); /* clean windows framing handle */

	Status.Framing = 1;
	Status.FirstFrame = 1;
	Config.Fps = fps ? fps : DISP_MAX_AVAILABLE_FPS;

	Status.FPSLimiter_th = xTimerCreate("FPSLimiter\0",
			1000 / Config.Fps / portTICK_PERIOD_MS, pdFALSE, 0, FPSLimiterCallback);

	vTimerSetTimerID(Status.FPSLimiter_th, this);

	xTaskCreate((void (*)(void *))Layering, "mf_lr\0", 1024, NULL, WUCY_WNDS_LAYER_TASK_PRIOR, &(LayeringTask));
	vTaskSuspend(LayeringTask);
	xTaskCreate((void (*)(void *))Rendering, "wnds_rndr\0", 1024, NULL, WUCY_WNDS_RENDER_TASK_PRIOR, &(RenderingTask));
	vTaskSuspend(LayeringTask);
	xTaskCreate((void (*)(void *))Framing, "wnds_frm\0", 1024, NULL, WUCY_WNDS_FRAME_TASK_PRIOR, &(FramingTask));
	return 0; /* success */
}



int8_t Mainframe::FramingStop() {

	xTimerDelete(Status.FPSLimiter_th, 0);

	vTaskDelete(FramingTask);
	vTaskDelete(LayeringTask);
	vTaskDelete(RenderingTask);

	Status.Framing = 0;

	memset(&(Status), 0x00, sizeof(wnd_state_t));

	return 0; /* success */
}



void Mainframe::SetAll() {

	memset((uint8_t*)FBUFF_DRAW, 0xFF, DISP_FRAMEBUFF_SIZE);
}



void Mainframe::ClearAll() {
	memset((uint8_t*)FBUFF_DRAW, 0x00, DISP_FRAMEBUFF_SIZE);

}


