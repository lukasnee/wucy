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

#include "ui.h"
#include "wucyOS.h"



#define RGB_STATE_HOLD_LOW 0
#define RGB_STATE_INCREASE z % 255
#define RGB_STATE_DECREASE 255 - z % 255
#define RGB_STATE_HOLD_HIGH 255


	window_t SquareTest, blob;
c_hex_t color = 0;


float i = 0.0, islow = 0.0;

void rotation(void * p){


	while(1){


		static uint16_t colorTick = 0, cp = 0;

		colorTick++;

		if(colorTick >= 250) {

			colorTick = 0;

			color = colorPalette16[cp++];
			if(cp >= 16)
				cp = 0;

		}

		i += 6.28 / 1000;
		if(i > 6.28)i -= 6.28;



		islow += 6.28 / 5000;
		if(islow > 6.28)islow -= 6.28;

		vTaskDelay(1 + 0 / portTICK_PERIOD_MS);


	}
}

static void SquareTestDraw(void * p) {


	wucy_hal_PinWrite(25, 1);

	wucy_wnd_SetPosition(&SquareTest, 32 + sin(islow)*32, 32 + sin(islow)*32);
	//ssd1351_Window_SetDimensions(&SquareTest, 64 + cos(islow)*32, 64 + cos(islow)*32);
	wucy_gfx_ClearAll(&SquareTest);

	//gfx_draw_Box(&window, 0x000000, REF_BOTTOM_L,  0, 0, 128, 128);
	/*todo make first gfx wnd parameter automatic from layering sequence */
	wucy_gfx_DrawBox(&SquareTest, 0xFF0000, REF_BOTTOM_L, 	0, 				0, 				10, 10); 		// 	bottom left - RED
	wucy_gfx_DrawBox(&SquareTest, 0x00FF00, REF_BOTTOM_R, 	DISP_RANGE_H, 	0, 				10, 10); 		//	bottom right - GREEN
	wucy_gfx_DrawBox(&SquareTest, 0x0000FF, REF_TOP_L, 		0, 				DISP_RANGE_W, 	10, 10); 		//	top left - BLUE
	wucy_gfx_DrawBox(&SquareTest, 0xFF00FF, REF_TOP_R, 		DISP_RANGE_H, 	DISP_RANGE_W, 	10, 10); 		//	top right - PURPLE

	float ii = i;

	for(int16_t d = 4; d <= 64; d +=6){

		wucy_gfx_DrawRect(&SquareTest, color, REF_CENTER, 64 + cos(ii)*32, 64 + sin(ii)*32, d, d);
	}

	wucy_gfx_DrawWindowFrame(&SquareTest, COLOR_TEAL, 2);

	wucy_hal_PinWrite(25, 0);

}

static void blobDraw(void * p) {

	wucy_gfx_ClearAll(&blob);

	for(uint8_t y = 0; y < 128; y += 8) {

		for(uint8_t x = 0; x < 128; x += 8) {

			wucy_gfx_DrawRect(&blob, COLOR_YELLOW, REF_BOTTOM_L, x, y, 8, 8);
		}
	}
}


void wucy_UI_Init(void) {

	wucy_wnd_Create(&SquareTest, SquareTestDraw, 2, 0, 0, 128, 128);
	wucy_wnd_Create(&blob, blobDraw, 1, 0, 0, 128, 128);

	xTaskCreate(rotation, "rotation", 1024, NULL, WUCY_USER_TASK_MAX_PRIOR - 3, NULL);


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




