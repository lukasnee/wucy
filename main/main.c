/*
	Software for WUCY - wearable open source general-purpose computer based on ESP32
	running FreeRTOS on custom Therram kernel. <https://github.com/therram/wucy>
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"


#include "ssd1351.h"

	ssd1351_t disp;
	ssd1351_spi_t dispInterface = {

		.CLK = 19,
		.MOSI = 23,
		.MISO = -1, /* -1 for "not used" */
		.CS = 22,
		.DC = 21,
		.RES = 18,
		.EN = -1	/* -1 for "not used" */
	};

/* ================================================================================ */
/* |									MAIN	 							  	  |	*/
/* ================================================================================ */

#define RGB_STATE_HOLD_LOW 0
#define RGB_STATE_INCREASE z % 255
#define RGB_STATE_DECREASE 255 - z % 255
#define RGB_STATE_HOLD_HIGH 255

	uint8_t drawnFlag = 0;
	c_hex_t color = 0xFF0000;
void tos_draw_screen(void *p){


	while(1){

		if(!drawnFlag){
			gpio_set_level(26, 1);
			ssd1351_SendBackground(&disp);
			drawnFlag = 1;
			gpio_set_level(26, 0);

		}
	}

}
float i = 0.0;

void rotation(void * p){


	while(1){


		i += 6.28 / 1000;
		if(i > 6.28)i -= 6.28;
		vTaskDelay(1);


	}
}

void test_fill(void *p){



 	 while(1){
 		 if(drawnFlag){

 			gpio_set_level(25, 1);
			fill_block_bg(&disp, 0x000000, 0, 0, 127, 127);

			fill_block_bg(&disp, 0xFF0000, 0, 0, 10, 10); 			// 	bottom left - RED
			fill_block_bg(&disp, 0x00FF00, 127, 0, -10, 10); 		//	bottom right - GREEN
			fill_block_bg(&disp, 0x0000FF, 0, 127, 10, -10); 		//	top left - BLUE
			fill_block_bg(&disp, 0xFF00FF, 127, 127, -10, -10); 	//	top right - PURPLE


			fill_block_bg(&disp, 0xFFFFFF, 40 + cos(i)*40, 40 + sin(i)*40, 40, 40);

			drawnFlag = 0;
			gpio_set_level(25, 0);
			vTaskDelay(1);

 		 }
 	 }
}


void app_main(void)
{

	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	gpio_set_direction(26, GPIO_MODE_OUTPUT);
	gpio_set_direction(27, GPIO_MODE_OUTPUT);

	ssd1351_Init_via_SPI(&disp, &dispInterface);

	xTaskCreate(tos_draw_screen, "send_bg", 1024, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(test_fill, "test_fill", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
	xTaskCreate(rotation, "rotation", 1024, NULL, configMAX_PRIORITIES - 1, NULL);

	while(1){

		vTaskDelay(500);
		color = color >> 8;
		if(!color)color = 0xFF0000;

	}
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


