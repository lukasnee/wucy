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





#include "ui_menu.hpp"


void wucy_ui_Init(void) {

	/* inputs */

	wucy_hal_PinInit(27, W_PIN_DIR_INPUT, W_PIN_PULL_UP);
	Button_Init(&ROTEN_OK, 27, BTN_TYPE_NORMAL);
	Button_Enable(&ROTEN_OK);

	/* display gui setup */

//	mf.addWindow(&test, 		3, 0, 0);
//	mf.addWindow(&background, 	1, 0, 0);

	mf.addWindow(&text, 2, 0, 64, true);

	mf.addWindow(&Bbox, 4, 64, 0);
	//Bbox.addBouncers(50);

	Bbox.addBouncers(10);


	menu.setTitle("menu");

	menu.addOption(1, OPT_ASTUONI, "view fps", ui_view_fps_cb);
	menu.addOption(4, OPT_PENKI, "fonts", ui_fonts_cb);
	menu.addOption(5, OPT_SESI, "scroll up", ui_scroll_up_cb);
	menu.addOption(6, OPT_SEPTYNI, "scroll down", ui_scroll_down_cb);

	menu.addOption(8, OPT_DEVYNI, "...", NULL);
	menu.addOption(9, OPT_DESIMT, "eol", NULL);
	menu.addOption(10, OPT_VIENAS, "balls >", ui_balls_cb);
	menu.addOption(11, OPT_DU, "add 5", ui_add_balls_cb);
	menu.addOption(12, OPT_TRYS, "remove 5", ui_remove_balls_cb);
	menu.addOption(13, OPT_KETURI, "clear all", ui_clear_balls_cb);

	menu.hideOption(OPT_DU);
	menu.hideOption(OPT_TRYS);
	menu.hideOption(OPT_KETURI);

	menu.showSelectMark();
	mf.addWindow(&menu, 5, 0, 0);

	xTaskCreate(ui_one_ms_timer, "ui_tim_1", 3*1024, NULL, WUCY_PRIOR_HIGHEST - 3, NULL);

	mf.framingStart(0);

}







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

		vTaskDelay(1 + 0 / portTICK_PERIOD_MS);

	}
}








#include "Picopixel.h"

void ui_draw_text(void * p) {

	Window * tw = (Window*)p; /* text window */

	tw->fillAll(COLOR_BLACK);

	tw->setFont(txtfont);
	tw->setTransperancy(1);
	tw->setTextSize(1);
	tw->setTextWrap(1);
	tw->setCursor(5, tw->getCharMaxHeight() + 5 + scrollBias);
	tw->setBounds(tw->GetW() - 10, 0);
	tw->setTextColor(COLOR_YELLOW);

	//tw->setCursor(0, 6 + 10 + (cos(islow) + 1) / 2 * 50);

	tw->println("The quick brown fox jumps over the lazy dog");

	for(uint16_t c = 0; c <= 0xFF; c++) {

		tw->write(c);
	}

	tw->setDrawColor(COLOR_RED);
	tw->drawFrame(2);

}





