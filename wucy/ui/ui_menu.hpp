/*
 * ui_menu.hpp
 *
 *  Created on: 2019-11-15
 *      Author: Lukas
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



void ui_balls_cb(void * p) {

	static bool toggle = 1;

	toggle ^= 1;

	wucy_hal_PinWrite(5, !toggle);

	if(toggle) {

		menu.hideOption(OPT_DU);
		menu.hideOption(OPT_TRYS);
		menu.hideOption(OPT_KETURI);
		menu.setOptionTitle(OPT_VIENAS, "balls >");

	}
	else {

		menu.hideOption(OPT_DU, HIDE_FALSE);
		menu.hideOption(OPT_TRYS, HIDE_FALSE);
		menu.hideOption(OPT_KETURI, HIDE_FALSE);
		menu.setOptionTitle(OPT_VIENAS, "balls <");

	}

}

void ui_add_balls_cb(void * p) {

	Bbox.addBouncers(5);

}

void ui_remove_balls_cb(void * p) {

	Bbox.removeBouncers(5);
}

void ui_clear_balls_cb(void * p) {


	Bbox.removeBouncers(0xFF);


}


int16_t scrollBias = 0;
const GFXfont * txtfont = &wucyFont8pt7b;

void ui_fonts_cb(void * p) {

	static uint8_t switcher = 0;

	switcher++;
	switcher %= 3;

	switch(switcher) {

		case 0: txtfont = &wucyFont8pt7b; break;
		case 1: txtfont = &trixel_square4pt7b; break;
		case 2: txtfont = &maniac16pt7b; break;

	}
	scrollBias = 0;
}


void ui_scroll_up_cb(void *p) {

	vTaskDelay(200);
	while (Button_IsPressed(&ROTEN_OK)) {

		vTaskDelay(20);
		scrollBias += 1;
		text.setRedrawRequest();

	}
}

void ui_scroll_down_cb(void *p) {

	vTaskDelay(200);
	while (Button_IsPressed(&ROTEN_OK)) {

		vTaskDelay(20);
		scrollBias -= 1;
		text.setRedrawRequest();

	}
}


void ui_view_fps_cb(void * p) {

	static uint8_t sw = 1;


	mf.fpsSetVisability((fps_draw_e)sw);

	sw++;
	sw %= 5;
}
