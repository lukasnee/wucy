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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" {

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

}

#include "Adafruit_GFX.h"


typedef uint16_t pixelData_t;

#define MAX_AVAILABLE_FPS 255

/* format 0xAARRGGBB where 0xAA - alpha byte, 0xRR - Red byte, 0xGG - Green byte, 0xBB - blue byte. */
typedef enum{

	COLOR_AQUA = 	0xFF00FFFF,
	COLOR_BLACK = 	0xFF000000,
	COLOR_BLUE = 	0xFF0000FF,
	COLOR_FUCHSIA = 0xFFFF00FF,
	COLOR_GRAY = 	0xFF808080,
	COLOR_GREEN = 	0xFF008000,
	COLOR_LIME = 	0xFF00FF00,
	COLOR_MAROON = 	0xFF800000,
	COLOR_NAVY = 	0xFF000080,
	COLOR_OLIVE = 	0xFF808000,
	COLOR_PURPLE = 	0xFF800080,
	COLOR_RED = 	0xFFFF0000,
	COLOR_SILVER = 	0xFFC0C0C0,
	COLOR_TEAL = 	0xFF008080,
	COLOR_WHITE = 	0xFFFFFFFF,
	COLOR_YELLOW = 	0xFFFFFF00,

}c_hex_t;

typedef enum{

	REF_BOTTOM_L = 0, /* default - left bottom left */
	REF_BOTTOM_R,
	REF_TOP_L,
	REF_TOP_R,
	REF_CENTER

}gfx_ref_e;

/* pixel coordinate on the display type */
typedef int16_t gfx_pos_t;

typedef void (*wnd_fcn_t)( void * );

typedef enum {
	LAYER_VERY_BOTTOM = 0,
	LAYER_VERY_TOP = 0xFF

}layer_e;

class Geo {	/* geometry */

protected:

	gfx_pos_t geo_x; /* x position on display (reference to left edge of frame) */
	gfx_pos_t geo_y; /* y position on display (reference to bottom edge of frame) */

	/* coordinates x and y together references bottom left corner of the frame */

	gfx_pos_t geo_w; /* width (of frame) in pixels */
	gfx_pos_t geo_h; /* height (of frame) in pixels */

public:

	Geo(gfx_pos_t x, gfx_pos_t y, gfx_pos_t w = 0, gfx_pos_t h = 0) :
	geo_x(x), geo_y(y), geo_w(w), geo_h(h) {};

	virtual ~Geo(void){};


	void SetX(gfx_pos_t x) { geo_x = x; }
	void SetY(gfx_pos_t y) { geo_y = y; }

	virtual void SetPosition(gfx_pos_t x, gfx_pos_t y) { geo_x = x; geo_y = y; }

	void SetW(gfx_pos_t w) { geo_w = w; }
	void SetH(gfx_pos_t h) { geo_h = h; }

	virtual int8_t setDimensions(gfx_pos_t w, gfx_pos_t h) { geo_w = w; geo_h = h; return 0;}

	gfx_pos_t GetX(void) { return geo_x; }
	gfx_pos_t GetY(void) { return geo_y; }
	gfx_pos_t GetW(void) { return geo_w; }
	gfx_pos_t GetH(void) { return geo_h; }

	gfx_pos_t GetMidH(void) { return (geo_x + geo_w / 2); }
	gfx_pos_t GetMidV(void) { return (geo_y + geo_h / 2); }
};


class Window : public Geo, public Adafruit_GFX {

private:
	uint8_t 		pixelSize;
	uint32_t 		frameBuffSize;
	pixelData_t * 	FrameBuff;

	uint8_t transperancy:1; /* 0 - draw black color; 1 - skip drawing black color */

	uint8_t layer; /* frame priority: drawn behind higher priority frames
	and in front of lower priority frames */
	uint8_t RedrawRequest:1;
	wnd_fcn_t RedrawFcn;

public:

    void 			setRedrawRequest(uint8_t state = 1) { RedrawRequest = state; };
    uint8_t 		getRedrawRequest() { return RedrawRequest; };

	/* todo currently redraw gets requested whenever anything changes in framebuffer, make it smart and optimal */

    void 			Redraw(Window * wnd = NULL) { RedrawRequest = 0;  RedrawFcn(wnd); };
    void 			setRedrawFcn(wnd_fcn_t fcn) { RedrawFcn = fcn; };
    wnd_fcn_t 		getRedrawFcn() { return RedrawFcn; };

    void 			setTransperancy(uint8_t state = 1) { transperancy = state; };
    uint8_t 		getTransperancy() { return transperancy; }

    void 			setLayer(uint8_t l) { layer = l; };
    uint8_t 		getLayer() { return layer; };


    Window(gfx_pos_t w, gfx_pos_t h, uint8_t pixelSize);
    ~Window();

	  void 			startWrite(void);
	  void         	clearAll(void);
	  void         	setDisplay(void);
//	  void         	invertDisplay(boolean i);

	  void         	drawPixel(int16_t x, int16_t y, pixelData_t color);
	  virtual void 	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	  virtual void 	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

	  pixelData_t   getPixel(int16_t x, int16_t y);
	  pixelData_t   *getBuffer(void) { return this->FrameBuff; };

	  int8_t 		setDimensions(gfx_pos_t w, gfx_pos_t h);
	  void 			drawFrame(gfx_pos_t thickness);
	  void 			drawDot(int16_t x, int16_t y);

	  void 	setDrawColor(c_hex_t color);
	  void 	setTextColor(c_hex_t color);
	  void 	setTextColor(c_hex_t color, c_hex_t background);


	  static const c_hex_t colorPalette16[16];
	  static const c_hex_t colorPalette14[14];
};

typedef enum {
	PING_DRAW_PONG_SEND = 0, PING_SEND_PONG_DRAW = 1
} BufferState_e;

typedef struct{

	uint8_t Framing:1;
	uint8_t FirstFrame:1;
	uint8_t LayeringDone:1;
	uint8_t TransmissionDone:1;
	uint8_t FpsLimiterAllows:1;

	BufferState_e BufferState:1;

	TimerHandle_t FPSLimiter_th;

}wnd_state_t;

typedef enum{ FPS_LIMITER_RESET = 0,
	FPS_LIMITER_ELAPSED}fps_limiter_e;

typedef struct{

	uint8_t Fps;

}wnd_cfg_t;

class Mainframe : public Geo {

private:

	std::vector<Window *> Windows;

	uint8_t 		pixelSize;
	uint32_t 		frameBuffSize;

	pixelData_t * Ping;	/* buffer 1 */
	pixelData_t * Pong;	/* buffer 2 */

	void 			SetPixel(gfx_pos_t x, gfx_pos_t y, pixelData_t data);
	pixelData_t 	GetPixel(gfx_pos_t x, gfx_pos_t y);

	static std::vector<Mainframe *> Mfs;

	static wnd_cfg_t 	Config;
	static wnd_state_t 	Status;

	static TaskHandle_t LayeringTaskH;
	static void 		Layering(void * p);

	static TaskHandle_t RenderingTaskH;
	static void 		Rendering(void * p);

	static TaskHandle_t FramingTaskH;
	static void 		Framing(void * p);

#define FBUFF_DRAW(mf_p) (!mf_p->Status.BufferState ? mf_p->Pong : mf_p->Ping)
#define FBUFF_SEND(mf_p) (mf_p->Status.BufferState ? mf_p->Pong : mf_p->Ping)

public:

	Mainframe(gfx_pos_t w, gfx_pos_t h, uint8_t pixelSize);

	~Mainframe();

	int8_t 				addWindow(Window * wnd, wnd_fcn_t WndDrawFcn, uint8_t l = LAYER_VERY_TOP, gfx_pos_t x = 0, gfx_pos_t y = 0);
	int8_t 				removeWindow(Window * wnd);

	int8_t 				framingStart(uint8_t fps);
	int8_t 				framingStop();

	void 				setAll();
	void 				clearAll();

	void 				FPSLimiterPass() { Status.FpsLimiterAllows = 1; };
	TaskHandle_t 		getFramingTaskH() { return FramingTaskH; };

};

void FPSLimiterCallback(TimerHandle_t xTimer = NULL);


#endif /* WUCY_GFX_INCLUDE_WINDOW_H_ */
