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

#include <list>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "Adafruit_GFX.h"

typedef uint16_t pixelData_t;

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

c_hex_t colorPalette16[16];
c_hex_t colorPalette14[14];


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

	gfx_pos_t geo_w; /* width of frame in pixels */
	gfx_pos_t geo_h; /* height of frame in pixels */

public:

	void SetX(gfx_pos_t x) { geo_x = x; }
	void SetY(gfx_pos_t y) { geo_y = y; }

	virtual void SetPosition(gfx_pos_t x, gfx_pos_t y) { geo_x = x; geo_y = y; }

	void SetW(gfx_pos_t w) { geo_w = w; }
	void SetH(gfx_pos_t h) { geo_h = h; }

	virtual void SetDimensions(gfx_pos_t w, gfx_pos_t h) { geo_w= w; geo_h= h; }

	gfx_pos_t Get_x(void) { return geo_x; }
	gfx_pos_t Get_y(void) { return geo_y; }
	gfx_pos_t Get_w(void) { return geo_w; }
	gfx_pos_t Get_h(void) { return geo_h; }

	gfx_pos_t GetMidH(void) { return (geo_x + geo_w / 2); }
	gfx_pos_t GetMidV(void) { return (geo_y + geo_h / 2); }
};


class Window : public Geo, public Adafruit_GFX {

private:

	pixelData_t * FrameBuff;


	layer_e layer; /* frame priority: drawn behind higher priority frames
	and in front of lower priority frames */
	uint8_t RedrawCondition:1;
	wnd_fcn_t RedrawFcn;

public:

    void 			RedrawRequest() { RedrawCondition = 1; };
    void 			SetLayer(layer_e l) { layer = l; };


    int8_t Window(wnd_fcn_t WndDrawFcn);
    void ~Window(void);

	  void         	clearDisplay(void);
	  void         	setDisplay(void);
//	  void         	invertDisplay(boolean i);

	  void         	drawPixel(int16_t x, int16_t y, pixelData_t color);
	  virtual void 	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	  virtual void 	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

	  pixelData_t   getPixel(int16_t x, int16_t y);
	  pixelData_t   *getBuffer(void) { return this->FrameBuff; };

	  int8_t 		SetDimensions(gfx_pos_t w, gfx_pos_t h);
	  void 			drawFrame(uint16_t color, gfx_pos_t thickness);
};


typedef struct{

	uint8_t Framing:1;
	uint8_t FirstFrame:1;

	TimerHandle_t FPSLimiter_th;

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

//todo change
#define VRAM_DRAW (!this.State ? this.Pong : this.Ping)
#define VRAM_SEND (this.State ? this.Pong : this.Ping)

class Mainframe{

private:

	pixelData_t * Ping;	/* buffer 1 */
	pixelData_t * Pong;	/* buffer 2 */

	enum{ PING_DRAW_PONG_SEND,
		PING_SEND_PONG_DRAW} State:1;

	std::list<Window> WndList;

	wnd_state_t 	Status;
	wnd_config_t 	Config;

	wnds_task_t 	LayeringTask;
	wnds_task_t 	FramingTask;
	wnds_task_t 	RenderingTask;

	void 		SetPixel(gfx_pos_t x, gfx_pos_t y, pixelData_t data);
	pixelData_t GetPixel(gfx_pos_t x, gfx_pos_t y);

	void 		Layering();
	void 		Rendering();
	void 		Framing();

	static void FPSLimiterCallback(TimerHandle_t xTimer);

public:

	int8_t 		Mainframe();
	int8_t 		~Mainframe();

	int8_t 		AddWindow(Window * wnd, layer_e l, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h);
	int8_t 		RemoveWindow(Window * wnd);

	int8_t 		FramingStart(uint8_t fps);
	int8_t 		FramingStop();

	void 		SetAll();
	void 		ClearAll();


};



#endif /* WUCY_GFX_INCLUDE_WINDOW_H_ */
