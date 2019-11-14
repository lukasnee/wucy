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

#include "ui.hpp"

#include <window.hpp>

using namespace std;

/* inputs  */

RotaryEncoder ROTEN(26, 25);
button_t ROTEN_OK;

/* outputs  */

Mainframe mf(DISP_WIDTH, DISP_HEIGHT, DISP_PIXEL_SIZE);

//static void ui_draw_background(void * p);
//Window background(DISP_WIDTH, DISP_HEIGHT, DISP_PIXEL_SIZE, ui_draw_background);

static void ui_draw_test(void * p);
Window test(DISP_WIDTH*2/3, DISP_HEIGHT*2/3, DISP_PIXEL_SIZE, ui_draw_test);

static void ui_draw_text(void * p);
Window text(DISP_WIDTH, DISP_HEIGHT/2, DISP_PIXEL_SIZE, ui_draw_text);

#include "bouncer.hpp"

BouncerBox Bbox(DISP_WIDTH/2, DISP_HEIGHT/2);

#include <object.hpp>


OptionList menu(&wucyFont8pt7b, 20, 20, DISP_WIDTH/2, DISP_HEIGHT/2);

typedef enum {

	OPT_VIENAS,
	OPT_DU,
	OPT_TRYS,
	OPT_KETURI,
	OPT_PENKI,
	OPT_SESI,
	OPT_SEPTYNI,
	OPT_ASTUONI,
	OPT_DEVYNI,
	OPT_DESIMT

}opts_e;


#include "sd_card.hpp"



/* for testing/example purpose */
void ui_one_ms_timer(void * p);

void menu_vienas_cb(void * p) {

	static bool toggle = 0;

	toggle ^= 1;

	wucy_hal_PinWrite(5, toggle);

	if(toggle) {

		menu.hideOption(OPT_DU);
		menu.hideOption(OPT_TRYS);
		menu.hideOption(OPT_KETURI);
		menu.setOptionTitle(OPT_VIENAS, "show");

	}
	else {

		menu.hideOption(OPT_DU, HIDE_FALSE);
		menu.hideOption(OPT_TRYS, HIDE_FALSE);
		menu.hideOption(OPT_KETURI, HIDE_FALSE);
		menu.setOptionTitle(OPT_VIENAS, "hide");

	}

}

void menu_du_cb(void * p) {

	Bbox.addBouncers(5);

}

void menu_trys_cb(void * p) {

	Bbox.removeBouncers(5);
}

void menu_keturi_cb(void * p) {


	Bbox.removeBouncers(0xFF);

}


const GFXfont * txtfont = &wucyFont8pt7b;

void menu_penki_cb(void * p) {

	static uint8_t switcher = 0;

	switcher++;
	switcher %= 3;

	switch(switcher) {

		case 0: txtfont = &wucyFont8pt7b; break;
		case 1: txtfont = &trixel_square4pt7b; break;
		case 2: txtfont = &maniac16pt7b; break;

	}

}

int16_t scrollBias = 0;

void menu_sesi_cb(void *p) {

	vTaskDelay(200);
	while (Button_IsPressed(&ROTEN_OK)) {

		vTaskDelay(20);
		scrollBias += 1;
		text.setRedrawRequest();

	}
}

void menu_septyni_cb(void *p) {

	vTaskDelay(200);
	while (Button_IsPressed(&ROTEN_OK)) {

		vTaskDelay(20);
		scrollBias -= 1;
		text.setRedrawRequest();

	}
}


void menu_astuoni_cb(void * p) {

	static bool toggle = 0;

	toggle = 1;

	mf.fpsSetVisability(toggle);

}


void wucy_ui_Init(void) {

	/* inputs */

	wucy_hal_PinInit(27, W_PIN_DIR_INPUT, W_PIN_PULL_UP);
	Button_Init(&ROTEN_OK, 27, BTN_TYPE_NORMAL);
	Button_Enable(&ROTEN_OK);

	/* display gui setup */

//	mf.addWindow(&test, 		3, 0, 0);
//	mf.addWindow(&background, 	1, 0, 0);

	mf.addWindow(&text, 		2, 0, 64, true);

	mf.addWindow(&Bbox, 4, 64, 0);
	//Bbox.addBouncers(50);

	Bbox.addBouncers(10);

	menu.setTitle("menu");
	menu.addOption(0, OPT_VIENAS, "hide", menu_vienas_cb);
	menu.addOption(1, OPT_DU, "+5", menu_du_cb);
	menu.addOption(2, OPT_TRYS, "-5", menu_trys_cb);
	menu.addOption(3, OPT_KETURI, "clean", menu_keturi_cb);
	menu.addOption(4, OPT_PENKI, "font", menu_penki_cb);
	menu.addOption(5, OPT_SESI, "up", menu_sesi_cb);
	menu.addOption(6, OPT_SEPTYNI, "down", menu_septyni_cb);
	menu.addOption(7, OPT_ASTUONI, "show fps", menu_astuoni_cb);
	menu.addOption(8, OPT_DEVYNI, "--", NULL);
	menu.addOption(9, OPT_DESIMT, "---", NULL);
	menu.showSelectMark();
	mf.addWindow(&menu, 5, 0, 0);

	xTaskCreate(ui_one_ms_timer, "ui_tim_1", 1024, NULL, WUCY_PRIOR_HIGHEST - 3, NULL);

	mf.framingStart(0);


	//sd_test();



}



int8_t number = 0;

c_hex_t color = COLOR_BLACK;

float i = 0.0, islow = 0.0;

void ui_one_ms_timer(void * p) {



	uint16_t colorTick = 0, cp = 0;

	while(1) {

		ROTEN.tick();

		Button_TimeBaseRoutine();

		menu.checkControl();

		switch(ROTEN.getDirection()) {

		case -1:
			Bbox.removeBouncers(1);
			break;

		case 0:
			break;

		case 1:
			Bbox.addBouncers(1);
			break;


		}


		if(!(colorTick % 50))
			Bbox.setRedrawRequest();

		colorTick++;

		if(colorTick >= 250) {

			colorTick = 0;

			color = test.colorPalette14[++cp%14];

		}

		i += 6.28 / 1000;
		if(i > 6.28)i -= 6.28;

		islow += 6.28 / 5000;
		if(islow > 6.28)islow -= 6.28;

		vTaskDelay(1 + 0 / portTICK_PERIOD_MS);

	}
}

#define BOX_SIZE 10

static void ui_draw_test(void * p) {

	//test.fillAll(COLOR_GRAY);

	//sq.SetPosition(0 /*+ sin(islow)*64*/, 0 /*+ sin(islow)*64*/);

	test.SetPosition(5, 5);

	//test.setDimensions(10 + (sin(islow) + 1) / 2 * 110, 10 + (sin(islow) + 1) / 2 * 110);

	float ii = i;

	test.setDrawColor(color);

	for(int16_t d = 4; d <= test.GetW()/2; d +=4) {

		test.drawCircle(
			test.GetW()/2 + cos(ii) * test.GetW()/4,
			test.GetH()/2 + sin(ii) * test.GetH()/4,
			d);
	}
	/* draw rectangular target pattern */

	for(int16_t d = 4; d <= test.GetW()/2; d +=4){


		test.drawRect(
			test.GetW()/2 + cos(ii) * test.GetW()/4 - d/2,
			test.GetW()/2 + sin(ii) * test.GetW()/4 - d/2,
			d, d);
	}

	test.setDrawColor(COLOR_RED);
	test.fillRect(0, 0, BOX_SIZE, BOX_SIZE);	// 	bottom left - RED

	test.setDrawColor(COLOR_GREEN);
	test.fillRect(test.GetW() - BOX_SIZE, 0, BOX_SIZE, BOX_SIZE);	//	bottom right - GREEN

	test.setDrawColor(COLOR_BLUE);
	test.fillRect(0, test.GetH() - BOX_SIZE, BOX_SIZE, BOX_SIZE);	//	top left - BLUE

	test.setDrawColor(COLOR_PURPLE);
	test.fillRect(test.GetW() - BOX_SIZE, test.GetH() - BOX_SIZE, BOX_SIZE, BOX_SIZE);	//	top right - PURPLE



	char numStr[20];
	sprintf(numStr, "num: %d", 132);

	test.setFont(&wucyFont8pt7b);
	test.setTextSize(1);
	test.setTextColor(COLOR_RED);
	test.setCursor(10, 10);
	test.print(numStr);

	test.setDrawColor(COLOR_TEAL);
	test.drawFrame(2);

}


#include "Picopixel.h"

void ui_draw_text(void * p) {

	Window * tw = (Window*)p; /* text window */

	tw->fillAll(COLOR_BLACK);

	tw->setFont(txtfont);
	tw->setTransperancy(1);
	tw->setTextSize(1);
	tw->setTextWrap(1);
	tw->setCursor(5, tw->getCharMaxHeight() + 5 + scrollBias, tw->GetW() - 10, tw->GetH());
	tw->setTextColor(COLOR_YELLOW);

	//tw->setCursor(0, 6 + 10 + (cos(islow) + 1) / 2 * 50);

	tw->println("The quick brown fox jumps over the lazy dog");

	for(uint16_t c = 0; c <= 0xFF; c++) {

		tw->write(c);
	}

	tw->setDrawColor(COLOR_RED);
	tw->drawFrame(2);

}



//#include "spook.h"

//static void ui_draw_background(void * p) {


//	bg.setDrawColor(COLOR_YELLOW);
//
//	for(uint8_t y = 0; y < 128; y += 8) {
//
//		for(uint8_t x = 0; x < 128; x += 8) {
//
//			background.drawRect(x, y, 8, 8);
//
//		}
//	}

//uint32_t color = 0xFF000000;
//uint32_t add = spook[0x0A];
//
//	for(uint8_t y = 127; y; y --) {
//
//		for(uint8_t x = 0; x < 128; x ++) {
//
//
//			color = *(uint32_t *)(spook + add);
//
//			color |= 0xFF000000;
//			add += 3;
//
//			background.setDrawColor((c_hex_t)color);
//			background.drawDot(x, y);
//
//		}
//	}

//	bg.setCursor(5, 70);
//	bg.setTextSize(2);
//
//	bg.setTextColor(COLOR_AQUA);
//	bg.println("AdafruitGFX");
//	bg.print("HACKED");


	//background.drawBitmap(0, 0, spook, 128, 128);
//}



//
//#define RGB_STATE_HOLD_LOW 0
//#define RGB_STATE_INCREASE z % 255
//#define RGB_STATE_DECREASE 255 - z % 255
//#define RGB_STATE_HOLD_HIGH 255


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






