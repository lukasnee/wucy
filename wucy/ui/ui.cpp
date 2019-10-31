/*
 * UI.c
 *
 *  Created on: 2019-10-01
 *      Author: lukas.neverauskis
 */
/*
	Software for project WUCY - wearable open source personal computer.
	based on ESP32 running FreeRTOS on custom Therram kernel.
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


#include <wucy_api.h>

extern "C" {

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

}

#include <cstdio>
#include <cstring>
#include <cmath>

#include "../ui/include/ui.hpp"

#include <window.hpp>

using namespace std;


#define RGB_STATE_HOLD_LOW 0
#define RGB_STATE_INCREASE z % 255
#define RGB_STATE_DECREASE 255 - z % 255
#define RGB_STATE_HOLD_HIGH 255

#define BOX_SIZE 10

Mainframe mf(DISP_WIDTH, DISP_HEIGHT, DISP_PIXEL_SIZE);

Window sq(DISP_WIDTH*2/3, DISP_HEIGHT*2/3, DISP_PIXEL_SIZE),
		bg(DISP_WIDTH, DISP_HEIGHT, DISP_PIXEL_SIZE);


c_hex_t color = COLOR_BLACK;


float i = 0.0, islow = 0.0;
uint8_t lvl = 0;

void rotation(void * p){

	while(1){

		static uint16_t colorTick = 0, cp = 0;

		colorTick++;

		if(colorTick >= 250) {

			colorTick = 0;

			wucy_hal_PinWrite(27, lvl ^= 1);

			color = sq.colorPalette14[cp++];
			if(cp >= 14)
				cp = 0;

		}

		i += 6.28 / 1000;
		if(i > 6.28) {

			i -= 6.28;
		}



		islow += 6.28 / 5000;
		if(islow > 6.28)islow -= 6.28;

		vTaskDelay(1 + 0 / portTICK_PERIOD_MS);

	}
}

static void testDraw(void * p) {

	//sq.SetPosition(0 /*+ sin(islow)*64*/, 0 /*+ sin(islow)*64*/);

	sq.SetPosition(5, 5);

	sq.setDimensions(10 + (sin(islow) + 1) / 2 * 110, 10 + (sin(islow) + 1) / 2 * 110);

	float ii = i;

	sq.setDrawColor(color);

	for(int16_t d = 4; d <= sq.GetW()/2; d +=6){

		sq.drawCircle(
			sq.GetW()/2 + cos(ii) * sq.GetW()/4,
			sq.GetH()/2 + sin(ii) * sq.GetH()/4,
			d);
	}

	sq.setDrawColor((c_hex_t)0xFF0000);
	sq.fillRect(0, 0, BOX_SIZE, BOX_SIZE);	// 	bottom left - RED

	sq.setDrawColor((c_hex_t)0x00FF00);
	sq.fillRect(sq.GetW() - BOX_SIZE, 0, BOX_SIZE, BOX_SIZE);	//	bottom right - GREEN

	sq.setDrawColor((c_hex_t)0x0000FF);
	sq.fillRect(0, sq.GetH() - BOX_SIZE, BOX_SIZE, BOX_SIZE);	//	top left - BLUE

	sq.setDrawColor((c_hex_t)0xFF00FF);
	sq.fillRect(sq.GetW() - BOX_SIZE, sq.GetH() - BOX_SIZE, BOX_SIZE, BOX_SIZE);	//	top right - PURPLE

/*
	wucy_gfx_DrawBox(&SquareTest, 0xFF0000, REF_BOTTOM_L, 	0, 				0, 				10, 10);
	wucy_gfx_DrawBox(&SquareTest, 0x00FF00, REF_BOTTOM_R, 	DISP_RANGE_H, 	0, 				10, 10); 		//	bottom right - GREEN
	wucy_gfx_DrawBox(&SquareTest, 0x0000FF, REF_TOP_L, 		0, 				DISP_RANGE_W, 	10, 10); 		//	top left - BLUE
	wucy_gfx_DrawBox(&SquareTest, 0xFF00FF, REF_TOP_R, 		DISP_RANGE_H, 	DISP_RANGE_W, 	10, 10);
*/

//	for(int16_t d = 4; d <= sq.GetW()/2; d +=6){
//
//
//		sq.drawRect(
//			sq.GetW()/2 + cos(ii) * sq.GetW()/4 - d/2,
//			sq.GetW()/2 + sin(ii) * sq.GetW()/4 - d/2,
//			d, d);
//	}

	sq.setDrawColor(COLOR_TEAL);
	sq.drawFrame(2);

}

#include<cstdio>
#include "spook.h"

static void bgDraw(void * p) {


//	bg.setDrawColor(COLOR_YELLOW);
//
//	for(uint8_t y = 0; y < 128; y += 8) {
//
//		for(uint8_t x = 0; x < 128; x += 8) {
//
//			bg.drawRect(x, y, 8, 8);
//
//		}
//	}

uint32_t color = 0xFF000000;
uint32_t add = spook[0x0A];

	for(uint8_t y = 127; y; y --) {

		for(uint8_t x = 0; x < 128; x ++) {


			color = *(uint32_t *)(spook + add);

			color |= 0xFF000000;
			add += 3;

			bg.setDrawColor((c_hex_t)color);
			bg.drawDot(x, y);

		}
	}

//	bg.setCursor(5, 70);
//	bg.setTextSize(2);
//
//	bg.setTextColor(COLOR_AQUA);
//	bg.println("AdafruitGFX");
//	bg.print("HACKED");


	//bg.drawBitmap(0, 0, spook, 128, 128);
}

#include "Picopixel.h"

void txtDraw(void * p) {

	Window * wnd = (Window*)p;

	wnd->setFont(NULL);

	wnd->setTextColor(COLOR_BLUE, COLOR_AQUA);
	wnd->setCursor(0, 6 + 10 + (cos(islow) + 1) / 2 * 50);
	wnd->setTextSize(1);
	wnd->setTextColor(COLOR_WHITE);
	wnd->setTextWrap(1);

	wnd->print("Edward Snowden is an American "
			"whistleblower who copied and "
			"leaked highly classified information "
			"from the National Security Agency in "
			"2013 when he was a Central Intelligence "
			"Agency employee and subcontractor. "
			"His new book Permanent Record is "
			"now available.\0");

}

void wucy_ui_Init(void) {


	mf.addWindow(&sq, testDraw, 3, 0, 0);
	mf.addWindow(&bg, bgDraw, 1, 0, 0);

	Window * txt = new Window(DISP_WIDTH, DISP_HEIGHT, DISP_PIXEL_SIZE);

	txt->setTransperancy(1);

	mf.addWindow(txt, txtDraw, 2, 0, 0);

	xTaskCreate(rotation, "rotation", 1024, NULL, WUCY_PRIOR_HIGHEST - 3, NULL);

	mf.framingStart(45);
}

//	ucg_Init(&ucg, ucg_dev_ssd1351_18x128x128_ilsoft, ucg_ext_ssd1351_18, ucg_com_my_own_hal);
//	ucg_SetFontMode(&ucg, UCG_FONT_MODE_TRANSPARENT);
//	ucg_ClearScreen(&ucg);
//	ucg_SetRotate270(&ucg);
//	ucg_SetFont(&ucg, ucg_font_ncenR10_tr);

//	//vTaskDelay(1000 / portTICK_PERIOD_MS);
//	ucg_SetColor(&ucg, 0, 255, 255, 255);
//	//ucg_DrawString(&ucg, 0, 25, 1, "Ucglib Box3D");
//
//	uint16_t r = 255;
//	uint16_t g = 0;
//	uint16_t b = 0;
//
//	float i = 0.0;
//	uint16_t z = 0;
//
//	while (1) {
//
//		z++;
//
//		if(z < 256){
//			r = RGB_STATE_HOLD_HIGH;
//			g = RGB_STATE_INCREASE;
//			b = RGB_STATE_HOLD_LOW;
//		}
//		else if(z < 512){
//			r = RGB_STATE_DECREASE;
//			g = RGB_STATE_HOLD_HIGH;
//			b = RGB_STATE_HOLD_LOW;
//		}
//		else if(z < 768){
//			r = RGB_STATE_HOLD_LOW;
//			g = RGB_STATE_HOLD_HIGH;
//			b = RGB_STATE_INCREASE;
//		}
//		else if(z < 1024){
//			r = RGB_STATE_HOLD_LOW;
//			g = RGB_STATE_DECREASE;
//			b = RGB_STATE_HOLD_HIGH;
//		}
//		else if(z < 1280){
//			r = RGB_STATE_INCREASE;
//			g = RGB_STATE_HOLD_LOW;
//			b = RGB_STATE_HOLD_HIGH;
//		}
//		else if(z < 1536){
//			r = RGB_STATE_HOLD_HIGH;
//			g = RGB_STATE_HOLD_LOW;
//			b = RGB_STATE_DECREASE;
//		}
//		else{
//			z = 0;
//		}
//
//		ucg_SetColor(&ucg, 0, r, g, b);

//		  calc_and_draw(w, v>>3);
//
//		  v+=3;
//		  v &= 511;
//
//		  w++;
//		  w &= 63;
//		  //vTaskDelay(0 / portTICK_PERIOD_MS);
//
//		  ucg_SetColor(&ucg, 0, 0, 0, 0);
//		  ucg_DrawBox(&ucg, x_min, y_min, x_max-x_min+1, y_max-y_min+1);

		//ucg_ClearScreen(&ucg);

//		i += 0.005;
//		if(i > 6.28)i -= 6.28;
//		ucg_DrawLine(&ucg, 64, 64, 64 + cos(i)*91, 64 + sin(i)*91);

//	}




