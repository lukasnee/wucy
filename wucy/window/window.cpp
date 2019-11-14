/*
 * window.cpp
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

using namespace std;

#include "window.hpp"

#include <wucy_api.h>



// MAINFRAME



TaskHandle_t Mainframe::LayeringTaskH = NULL;

void Mainframe::Layering(void * p) {

	Mainframe * mf;
	Window * win;

	/* todo transperancy merging feature */

	while (Status.Framing) {

		/* cycle through all mainframes */
		for (auto MfCur = Mfs.cbegin(); MfCur != Mfs.cend(); ++MfCur) {

			mf = *MfCur;
			/* auto clear frame */
			mf->clearAll();

			for (auto wndCur = mf->Windows.begin(); wndCur != mf->Windows.end(); ++wndCur) {

				win = *wndCur;

				if (win->getRedrawRequest() && win->getRedrawFcn() != NULL) {
#ifdef WND_CLEAR_WINDOWS
					win->clearAll(); /* sometimes unnecessary, improves performance */
#endif
					win->Redraw(win);
				}

				for (gfx_pos_t y_wnd = 0, y_mf = win->GetY();
						y_mf < mf->GetH() && y_mf < win->GetY() + win->GetH();
						y_mf++) {

					for (gfx_pos_t x_wnd = 0, x_mf = win->GetX();
							x_mf < mf->GetW() && x_mf < win->GetX() + win->GetW();
							x_mf++) {

						if(!(win->getTransperancy() && win->getPixel(x_wnd, y_wnd) == 0x0000)) {

							 mf->SetPixel(x_mf, y_mf, win->getPixel(x_wnd, y_wnd));

						}

						x_wnd++;
					}
					y_wnd++;
				}
			}
		}

		Status.LayeringDone = 1;
		vTaskResume(FramingTaskH);

		vTaskSuspend(NULL);

	}
	vTaskDelete(NULL);
}




TaskHandle_t Mainframe::RenderingTaskH = NULL;

void Mainframe::Rendering(void * p) {

	Mainframe * mf;

	while(Status.Framing) {

		for (auto MfCur = Mfs.cbegin(); MfCur != Mfs.cend(); ++MfCur) {

			mf = *MfCur;

			/* begin transmission */
			wucy_disp_RenderNewFrame((uint8_t *)FBUFF_SEND(mf));

			/* ... task idles here until transmission is finished ... */

		}

		Status.TransmissionDone = 1;
		vTaskResume(FramingTaskH);

		vTaskSuspend(NULL);

	}
	vTaskDelete(NULL);
}



TaskHandle_t Mainframe::FramingTaskH = NULL;

void Mainframe::Framing(void * p) {

	Mainframe * mf;

	while(Status.Framing) {

		for (auto MfCur = Mfs.cbegin(); MfCur != Mfs.cend(); ++MfCur) {

			mf = *MfCur;

			if (Status.FirstFrame ||
					( mf->Status.LayeringDone && mf->Status.TransmissionDone && mf->fpsLimiterPasses())) {

				Status.FirstFrame = 0;

				/* reset framing condition flags */
				Status.LayeringDone = 0;
				Status.TransmissionDone = 0;

				/* count framerate for inspection */
				mf->fpsCountFrame();

				/* switch frame buffer state */
				flipFramebuffers();

				/* begin timer for limiter */
				mf->fpsLimiterReset();

				/* Before rendering new frame to display,
				 * enable layering task which has lower
				 * priority by a single step. Eventualy
				 * current task will pause on transmission
				 * and pass on to layering task */

				vTaskResume(RenderingTaskH);
				vTaskResume(LayeringTaskH);

			}
		}

		vTaskSuspend(NULL);

	}

	vTaskDelete(NULL);

}


inline void Mainframe::SetPixel(gfx_pos_t x, gfx_pos_t y, pixelData_t data) {

	if(x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

		*(FBUFF_DRAW(this) + GetW() * y + x) = data;

	}
}
inline pixelData_t Mainframe::GetPixel(gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < GetW() && y < GetH()) {

		return *(FBUFF_DRAW(this) + GetW() * y + x);
	}
	return 0;
}

vector<Mainframe *> Mainframe::Mfs;

/*wnd_cfg_t Mainframe::Config = { 0 };*/

wnd_state_t Mainframe::Status = {

		.Framing = 0,
		.FirstFrame = 0,
		.LayeringDone = 0,
		.TransmissionDone = 0,

		.BufferState = PING_DRAW_PONG_SEND,

};



Mainframe::Mainframe(gfx_pos_t w, gfx_pos_t h, uint8_t pixelSize) :
		Geo(0, 0, w, h),
		pixelSize(pixelSize),
		frameBuffSize(w * h * pixelSize) {

	_fps.pos = FPS_RIGHT_TOP_CORNER;
	_fps.Limit = 0;
	_fps.Counter = 0;
	_fps.Current = 0;
	_fps.LimiterAllows = 0;

	_fps.counterTimHndlr = NULL;
	_fps.limiterTimHndlr = NULL;
	_fps.str.assign("---FPS");
	_fps.window = NULL;


	Mfs.push_back(this);

	Status.Framing = 0;

	Ping = (pixelData_t*) wucy_hal_SpecMalloc(MALLOC_SPECIALIZED_DMA, frameBuffSize);
	Pong = (pixelData_t*) wucy_hal_SpecMalloc(MALLOC_SPECIALIZED_DMA, frameBuffSize);

	if (Ping == NULL || Pong == NULL) {
		delete this;
	}
}


Mainframe::~Mainframe() {

	Status.Framing = 0;

	stopFpsLimiter();

	free(Ping);
	free(Pong);

	for (auto MfCur = Mfs.cbegin(); MfCur != Mfs.cend(); ++MfCur) {

		if(*MfCur == this) {

			Mfs.erase(MfCur);

			break;
		}
	}
}




void Mainframe::setAll() {

	memset((uint8_t*)FBUFF_DRAW(this), 0xFF, frameBuffSize);
}



void Mainframe::clearAll() {
	memset((uint8_t*)FBUFF_DRAW(this), 0x00, frameBuffSize);

}



int8_t Mainframe::addWindow(Window * wnd, uint8_t l, gfx_pos_t x, gfx_pos_t y, bool InitialDraw) {

	wnd->setLayer(l);

	wnd->SetX(x);
	wnd->SetY(y);

	wnd->setRedrawRequest(InitialDraw);

	if(!Windows.empty()) {

		for (auto wndCur = Windows.cbegin(); wndCur != Windows.cend(); ++wndCur) {

			/* sort from lowest to highest head to tail */
			if(wnd->getLayer() < (*wndCur)->getLayer()) {

				/* todo make so it adds before higher layer, after same layer,
				 * so the same number layers have priority by creation time */
			//	std::vector<Window,allocator<Window>>::const_iterator

				Windows.insert(wndCur, wnd);

				return 2; /* success: inserted between head and tail */
			}
		}
	}
	Windows.push_back(wnd);

	return 3; /* success: inserted at tail */

}


int8_t Mainframe::removeWindow(Window *wnd) {

	for (auto wndCur = Windows.cbegin(); wndCur != Windows.cend(); ++wndCur) {

		if(*wndCur == wnd) {

			Windows.erase(wndCur);

			return 0; /* element found and erased */
		}
	}

	return -1; /* element was not found */
}



int8_t Mainframe::framingStart(uint8_t fps) { /* unlimited fps as default */

	//window_FramingStop(windows); /* clean windows framing handle */

	Status.Framing = 1;
	Status.FirstFrame = 1;

	startFpsLimiter(fps);

	xTaskCreate(Layering, "mf_lr\0", 1024, NULL, WUCY_WNDS_LAYER_TASK_PRIOR, &LayeringTaskH);
	vTaskSuspend(LayeringTaskH);

	xTaskCreate(Rendering, "wnds_rndr\0", 1024, NULL, WUCY_WNDS_RENDER_TASK_PRIOR, &RenderingTaskH);
	vTaskSuspend(RenderingTaskH);

	xTaskCreate(Framing, "wnds_frm\0", 1024, NULL, WUCY_WNDS_FRAME_TASK_PRIOR, &FramingTaskH);

	return 0; /* success */
}



int8_t Mainframe::framingStop() {

	stopFpsLimiter();

	vTaskDelete(FramingTaskH);
	vTaskDelete(LayeringTaskH);
	vTaskDelete(RenderingTaskH);

	Status.Framing = 0;

	memset(&(Status), 0x00, sizeof(wnd_state_t));

	return 0; /* success */
}




//fps_t Mainframe::_fps = {
//
//	.Counter = 0,
//	.Current = 0,
//	.Limit = 0,
//	.LimiterAllows = 1,
//	.limiterTimHndlr = NULL,
//	.counterTimHndlr = NULL,
//	.str = "---FPS",
//	.pos = FPS_RIGHT_TOP_CORNER,
//	.window = NULL,
//};

void Mainframe::fpsLimiterCallback(TimerHandle_t xTimer) {

	Mainframe * mf = (Mainframe * )pvTimerGetTimerID(xTimer);

	/* indicate the timer has expired (ready for new frame) */
	mf->fpsLimiterExpired();

}

void Mainframe::fpsCounterCallback(TimerHandle_t xTimer) {

	Mainframe * mf = (Mainframe * )pvTimerGetTimerID(xTimer);

	/* proccess count sequence */
	mf->fpsCalculateAndDraw();
}

/* begin limiter timer process */
void Mainframe::startFpsLimiter(uint8_t fps) {

	changeFpsLimit(fps);

	/* run autoreload 1.000 sec timer for framerate counting */
	Mainframe::_fps.counterTimHndlr = xTimerCreate("fps_cnt\0",
			COUNTER_REFRESH_RATE_IN_MS / portTICK_PERIOD_MS, pdTRUE,
			this, Mainframe::fpsCounterCallback);

	xTimerStart(Mainframe::_fps.counterTimHndlr, 0);

}

/* stpo timer proccess */
void Mainframe::stopFpsLimiter() {

	_fps.LimiterAllows = 0;

	xTimerDelete(Mainframe::_fps.limiterTimHndlr, 0);
	xTimerDelete(Mainframe::_fps.counterTimHndlr, 0);

}

/* restart timer */
void Mainframe::fpsLimiterReset() {

	if(getfpsLimit()) {

		_fps.LimiterAllows = 0;

		/* reset and run timer again */
		if(xTimerIsTimerActive(Mainframe::_fps.limiterTimHndlr) == pdFALSE)
			xTimerStart(Mainframe::_fps.limiterTimHndlr, 0);
	}
	else {

		vTaskDelay(1);
	}
}
void Mainframe::changeFpsLimit(uint8_t fps) {

	Mainframe::_fps.Limit = fps;

	if(getfpsLimit() > 0) {

		if(Mainframe::_fps.limiterTimHndlr != NULL) {

			xTimerDelete(Mainframe::_fps.limiterTimHndlr, 0);
		}

		Mainframe::_fps.limiterTimHndlr = xTimerCreate("fps_lim\0",
				1000 / getfpsLimit() / portTICK_PERIOD_MS, pdFALSE,
				this, Mainframe::fpsLimiterCallback);

		xTimerStart(Mainframe::_fps.counterTimHndlr, 0);

	}
	else {

		_fps.LimiterAllows = 1;

	}
}




void Mainframe::fpsCalculateAndDraw() {


	uint16_t w = 0, h = 0;
	int16_t x1 = 0, y1 = 0;

	/* calculate current fps */
	_fps.Current = (uint32_t)(_fps.Counter) * 1000 / COUNTER_REFRESH_RATE_IN_MS;
	/* reset current count for new fps sample */
	_fps.Counter = 0;

	_fps.str.assign(std::to_string(_fps.Current) + "FPS");
	//std::sprintf(_fps.str, "%dFPS", _fps.Current);

	if(_fps.window != NULL) {

	_fps.window->setBounds(0, 0);
	_fps.window->getTextBounds(_fps.str, 0, 0, &x1, &y1, &w, &h);

	_fps.window->setDimensions(w, h);

	_fps.window->fillAll(COLOR_BLACK);

	_fps.window->setCursor(0, h - 1);

	_fps.window->setTextColor(COLOR_WHITE);
	_fps.window->print(getFpsPrintOut());

	switch (_fps.pos) {

			case FPS_LEFT_TOP_CORNER:
				_fps.window->SetPosition(0, 0);
				break;

			case FPS_RIGHT_TOP_CORNER:
				_fps.window->SetPosition(DISP_WIDTH - w, 0);
				break;

			case FPS_LEFT_BOTTOM_CORNER:
				_fps.window->SetPosition(0, DISP_HEIGHT - h);
				break;

			case FPS_RIGHT_BOTTOM_CORNER:
				_fps.window->SetPosition(DISP_WIDTH - w, DISP_HEIGHT - h);
				break;

		}
	}
}


void Mainframe::fpsSetVisability(bool state, fps_draw_e pos) {


	/* create window */
	if(_fps.window == NULL && state) {

		_fps.window = new Window(40, 10, DISP_PIXEL_SIZE, NULL);

		if(_fps.window != NULL) {

			_fps.window->setFont(&wucyFont8pt7b/*trixel_square4pt7b*/);
			_fps.window->setTextSize(1);

			/* initial draw (later refreshes every sec) */
			fpsCalculateAndDraw();

			addWindow(_fps.window, 255, 0, 0, true);

		}
	}
	else if (_fps.window != NULL && !state){

		 removeWindow(_fps.window);

		delete _fps.window;

		_fps.window = NULL;

	}

	_fps.pos = pos;
};



void Mainframe::fpsLimiterExpired() {

	_fps.LimiterAllows = 1;

	/* resume framing task to check if all conditions pass for new frame */
	if (eTaskGetState(FramingTaskH) == eSuspended) {

		vTaskResume(FramingTaskH);
	}

};



Window::Window(gfx_pos_t w, gfx_pos_t h, uint8_t pixelSize, wnd_fcn_t fcn) :

		Geo(0, 0, 0, 0),
		Adafruit_GFX(w, h),

		pixelSize(pixelSize),
		frameBuffSize(w * h * pixelSize),
		FrameBuff(NULL),
		transperancy(0),
		layer(0),
		RedrawRequest(0),
		RedrawFcn(fcn)

	{

	if (w && h) {

		setDimensions(w, h);

	}
	else {

		delete this;
	}
}



Window::~Window(){

	free(FrameBuff);
}



/* this is called whenever adafruitGFX draws anything. Request redrawing window */
void Window::startWrite(void) {

	RedrawRequest = 1;

};



void Window::clearAll(void) {

	memset((uint8_t*) FrameBuff, 0x00,
			GetW() * GetH() * pixelSize);

}



void Window::setAll(void) {

	memset((uint8_t*) FrameBuff, 0xFF,
			GetW() * GetH() * pixelSize);

}



void Window::fillAll(c_hex_t color) {

	setDrawColor(color);

	fillRect(0, 0, GetW(), GetH());

}



#define swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

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


int8_t Window::setDimensions(gfx_pos_t w, gfx_pos_t h) {

	/* allocate new buffer if length changes */
	if( w * h != GetW()*GetH()) {

		delete FrameBuff;

		FrameBuff = new pixelData_t[w * h];

		/* if unable to change buffer dimensions and previous dimensions were different */
		if(FrameBuff == NULL) {

			/* reallocate freed memory buffer */
			FrameBuff = new pixelData_t[GetW() * GetH()];

			/* if malloc failed */
			if(FrameBuff == NULL){

				return -2; /* critical error: frame buffer lost */
			}

			return -1; /* error: new frame buffer dimensions are to high for new buffer allocation */
		}

		SetW(w);
		SetH(h);

		WIDTH =_width = w;
		HEIGHT = _height = h;
		frameBuffSize = w * h * pixelSize;
	}

	return 0;

}

/* todo might become unused and useless */
void Window::drawFrame(gfx_pos_t thickness) {

	for(gfx_pos_t t = 0; t < thickness; t++) {

		drawRect(t, t, GetW() - t*2, GetH() - t*2);

	}
}

void Window::drawDot(int16_t x, int16_t y) {

	drawPixel(x, y, drawcolor);

}

void Window::setDrawColor(c_hex_t color) {

	drawcolor = (uint16_t)wucy_disp_HEXcolor2DispPixelData(color);

}
/* transperant */
void Window::setTextColor(c_hex_t color) {

	textcolor = textbgcolor = (uint16_t)wucy_disp_HEXcolor2DispPixelData(color);

}

/* with background */
void Window::setTextColor(c_hex_t color, c_hex_t background) {

	textcolor = (uint16_t)wucy_disp_HEXcolor2DispPixelData(color);
	textbgcolor = (uint16_t)wucy_disp_HEXcolor2DispPixelData(background);

}


const c_hex_t Window::colorPalette16[16] = {

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
const c_hex_t Window::colorPalette14[14] = {

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



/* todo maybe */
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

