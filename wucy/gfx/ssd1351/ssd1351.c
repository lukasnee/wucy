/*
 * ssd1351.c
 *
 *  Created on: 2019-09-05
 *      Author: lukas.neverauskis
 */

/*
	ssd1351 - software driver for ssd1351 type display.
	Portable, although designed for project WUCY. <https://github.com/therram/wucy>
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

#include "ssd1351.h"
#include "api.h"

/* ================================================================================ */
/* |				SSD1351 User Hardware Abstraction Layer (HAL)				  |	*/
/* ================================================================================ */

//ssd1351_hal_msg_e hal_msg;

uint8_t default_settings[] = {

	SSD1351_CMD_SET_COMMAND_LOCK, 1, 0x12,
	SSD1351_CMD_SET_COMMAND_LOCK, 1, 0xB1,
	SSD1351_CMD_DISPLAY_ALL_OFF, 0,

	SSD1351_CMD_CLOCK_DIV_AND_OSC_FREQ, 1, 0x00/*DIV_AND_OSC_CMD*/,
	SSD1351_CMD_SET_MUX_RATIO, 1, MUX_RATIO,
	SSD1351_CMD_PHASE_PERIODS, 1, PHASE_PERIODS_CMD,

	SSD1351_CMD_SET_DISPLAY_START_LINE, 1, 0x00,
	SSD1351_CMD_SET_DISPLAY_OFFSET, 1, 0x00,

	SSD1351_CMD_SET_GPIO, 1, 0x00,
	SSD1351_CMD_FUNCTION_SELECT, 1, 0x01,
	SSD1351_CMD_SET_SEGMENT_LOW_VOLTAGE, 3, 0xA0, 0xB5, 0x55,
	SSD1351_CMD_CONTRAST_CURR_FOR_COLORS, 3, 0xC8, 0x80, 0xC8,
	SSD1351_CMD_CONTRAST_CURR_CTRL_MASTER, 1, 0x0F,

	SSD1351_CMD_DISPLAY_ENHANCEMENT, 3, 0xA4, 0x00, 0x00,
	SSD1351_CMD_SET_PRECHARGE_VOLTAGE, 1, 0x17,
	SSD1351_CMD_SET_SECND_PRECHRG_PERIOD, 1, 0x01,
	SSD1351_CMD_SET_VCOMH_VOLTAGE, 1, 0x05,

	/* 0x10 flips horziontally (referencing pin text as up)*/
	/* 0x20 flips vertical  (referencing pin text as up) */
	SSD1351_CMD_CONFIG_REG, 1, 0x24,

	SSD1351_CMD_DISPLAY_NORMAL, 0,

	CMD_LIST_TERMINATE};

/* ================================================================================ */
/* |						SSD1351 driver user functions 						  |	*/
/* ================================================================================ */



/* command list format: 												example:
 *
 *
 * uint8_t command_list[0]: "COMMAND BYTE 1"							0x15
 * uint8_t command_list[1]: "LENGTH OF FOLLOWING COMMAND DATA - n"		2
 * uint8_t command_list[2]: "DATA BYTE 1"								0x00
 * uint8_t command_list[3]: "DATA BYTE 2"								0x7F
 * ...
 * uint8_t command_list[4]: "DATA BYTE n"
 * uint8_t command_list[5]: "COMMAND BYTE 2" 							0x75
 * uint8_t command_list[6]: "LENGTH OF FOLLOWING COMMAND DATA - n"		2
 * uint8_t command_list[7]: "DATA BYTE 1"								0x00
 * uint8_t command_list[8]: "DATA BYTE 2"								0x7F
 * ...
 * uint8_t command_list[final]: "COMMAND LIST TERMINATOR - 0x00"		0x00
 * */

static void ssd1351_SendCommand(ssd1351_t * disp, cmd_list_t * command_list) {

	uint32_t pos = 0;

	do{

		HAL_Transmit(disp, DC_COMMAND, command_list + pos, 1); /* send command */

		pos++;	/* step to following command data lenght indicator */

		if(*(command_list + pos) != 0){

			HAL_Transmit(disp, DC_DATA, command_list + pos + 1, *(command_list + pos));
		}
		pos+= *(command_list + pos) + 1;

	}
	while(*(command_list + pos) != CMD_LIST_TERMINATE);
}

#include "driver/gpio.h"
/*todo consider making data type pxl_vram_t?*/
void ssd1351_display_SendData(ssd1351_t * disp,
		uint8_t * data, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	uint32_t data_size;

	if(x1 <= SSD1351_RANGE_W && x2 <= SSD1351_RANGE_W &&
			y1 <= SSD1351_RANGE_H && y2 <= SSD1351_RANGE_H) {


		data_size = (x2 - x1 + 1) * (y2 - y1 + 1) * PIXEL_SIZE;

		uint8_t pre_vram_dump[] = {
			SSD1351_CMD_SET_ADDRESS_COLUMN, 2, x1 & 0x7F, x2 & 0x7F, //0x7F -SSD1351_RANGE_H */
			SSD1351_CMD_SET_ADDRESS_ROW, 2, y1 & 0x7F, y2 & 0x7F,
			SSD1351_CMD_RAM_WRITE, 0,
		CMD_LIST_TERMINATE};

		ssd1351_SendCommand(disp, pre_vram_dump);


		HAL_Transmit(disp, DC_DATA, data, data_size);

	}
}

void ssd1351_UpdateAll(ssd1351_t * disp){

	disp->FrameBuff.State = !disp->FrameBuff.State;
	gpio_set_level(27, disp->FrameBuff.State);

	 ssd1351_display_SendData(disp,
			 (uint8_t *)VRAM_SEND,
			 disp->FrameBuff.geo.x,
			 disp->FrameBuff.geo.y,
			 disp->FrameBuff.geo.w - 1,
			 disp->FrameBuff.geo.h - 1
			 );

}

int8_t ssd1351_Init_via_SPI(ssd1351_t * disp, ssd1351_spi_t * interface) {

	if(disp->Status.Init == UNINITIALIZED) {

		disp->Pin.SPI = interface;
		disp->Status.Interface = SSD1351_IF_SPI;

		HAL_Init(disp);

		disp->Status.Init = INITIALIZED;

		ssd1351_PowerOn(disp);

		return 0;
	}
	return -1;
}

int8_t ssd1351_Init_via_8080(ssd1351_t * disp, ssd1351_8_bit_parallel_t * interface) {

	if(disp->Status.Init == UNINITIALIZED) {

		disp->Pin.Prll = interface;
		disp->Status.Interface = SSD1351_IF_8_BIT;

		HAL_Init(disp);

		disp->Status.Init = INITIALIZED;

		ssd1351_PowerOn(disp);

		return 0;
	}
	return -1;
}


void ssd1351_DeInit(ssd1351_t * disp){

	HAL_DeInit(disp);
	ssd1351_PowerOff(disp);
}



void ssd1351_PowerOn(ssd1351_t * disp){

	HAL_PinWrite(disp, disp->Pin.SPI->RES, 0);
	HAL_DelayMs(1);
	HAL_PinWrite(disp, disp->Pin.SPI->RES, 1);
	HAL_DelayMs(1);
	//HAL_PinWrite(disp, disp->Pin.SPI->EN, 1); /* enable high voltage Vcc supply (usually from boost converter) */
	HAL_DelayMs(300); /* delay minimum 300 ms before sending screen on command */

	ssd1351_SendCommand(disp, default_settings);
	ssd1351_Sleep(disp, SLEEP_OFF);

}


void ssd1351_PowerOff(ssd1351_t * disp){

	ssd1351_Sleep(disp, SLEEP_ON);
	HAL_PinWrite(disp, disp->Pin.SPI->EN, 0);

}

inline void ssd1351_DisplayMode(ssd1351_t * disp, display_mode_e mode){

	cmd_list_t cmd[] = {mode, 0, CMD_LIST_TERMINATE};
	ssd1351_SendCommand(disp, cmd);

}


inline void ssd1351_Sleep(ssd1351_t * disp, sleep_mode_e mode){

	/*  todo SSD1351_CMD_FUNCTION_SELECT (0xAB) data byte must be merged
	 *  with interface selection bits A[7:6] if 16 - bit parallel port used,
	 *  not implemented yet */

	/* default Command for disable internal VDD regulator */
	cmd_list_t regulator[] = {SSD1351_CMD_FUNCTION_SELECT, 1, 0x00, CMD_LIST_TERMINATE};
	cmd_list_t cmd[] = {mode, 0, CMD_LIST_TERMINATE};

	if(mode == SLEEP_ON || mode == SLEEP_OFF){

		ssd1351_SendCommand(disp, cmd);
		if(mode == SLEEP_OFF){
			HAL_DelayMs(1);
			regulator[2] = 0x01; /* Command for enable internal VDD regulator */
		}

		ssd1351_SendCommand(disp, regulator);

	}
}

inline pixel_vram_t ssd1351_color_rgba2PixelVRAM(rgba_t rgba) {

	pixel_vram_t pxl_color;

	pxl_color = ((rgba.g << 13) | (rgba.b << 8) | (rgba.r << 3) | (rgba.g >> 3));

	return pxl_color;

}

rgba_t color_hex2rgba(c_hex_t color_hex) {

	rgba_t rgba;

	rgba.a = (((color_hex & 0xFF000000) >> 24) * SSD1351_A_MAX) / 0xFF;
	rgba.r = (((color_hex & 0x00FF0000) >> 16) * SSD1351_R_MAX) / 0xFF;
	rgba.g = (((color_hex & 0x0000FF00) >> 8) * SSD1351_G_MAX) / 0xFF;
	rgba.b = (((color_hex & 0x000000FF) >> 0) * SSD1351_B_MAX) / 0xFF;

	return rgba;
}

pixel_vram_t ssd1351_color_hex2PixelVRAM(c_hex_t color_hex) {

	return ssd1351_color_rgba2PixelVRAM(color_hex2rgba(color_hex));

}



inline void ssd1351_SetAll(ssd1351_t * disp) {

	memset(VRAM_DRAW, 0xFF, VRAM_SIZE);

}

inline void ssd1351_ClearAll(ssd1351_t * disp) {

	memset(VRAM_DRAW, 0x00, VRAM_SIZE);

}



static int8_t ssd1351_Window_RenderAll(ssd1351_t * disp) {


	for (window_t * wndCur = disp->WindowList->tqh_first; wndCur != NULL;
			wndCur = wndCur->WindowNodes.tqe_next) {

		for(gfx_pos_t y_wnd = 0, y_mf = wndCur->data->geo.y;
				y_mf < wndCur->data->geo.y + wndCur->data->geo.h;
				y_mf++) {

			for(gfx_pos_t x_wnd = 0, x_mf = wndCur->data->geo.x;
					x_mf < wndCur->data->geo.x + wndCur->data->geo.w;
					x_mf++) {

				ssd1351_mainframe_PixelSet(disp, x_mf, y_mf,
						ssd1351_PixelDataGet(wndCur, x_wnd, y_wnd));

				x_wnd++;
			}

			y_wnd++;
		}
	}
	return 1;
}



/* todo isnt used doesnt work with windows functions */
/* for mainframe specific window type */
inline void ssd1351_mainframe_PixelSet(ssd1351_t * disp,
		gfx_pos_t x, gfx_pos_t y, pixel_vram_t data) {

	if(x >= 0 && y >= 0 && x < disp->FrameBuff.geo.w && y < disp->FrameBuff.geo.h) {

		*(VRAM_DRAW + disp->FrameBuff.geo.w * y + x) = data;

	}
}



/* for mainframe specific window type */
inline pixel_vram_t ssd1351_mainframe_PixelGet(ssd1351_t * disp,
		gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < disp->FrameBuff.geo.w && y < disp->FrameBuff.geo.h) {

		return *(VRAM_DRAW + disp->FrameBuff.geo.w * y + x);

	}
	return 0;
}



/* for general window types */
inline void ssd1351_PixelDataSet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y, pixel_vram_t data) {

	if(x >= 0 && y >= 0 && x < wnd->data->geo.w && y < wnd->data->geo.h) {

		*( wnd->data->FrameBuff + wnd->data->geo.w * y + x) = data;

	}
}



/* for general window types */
inline pixel_vram_t ssd1351_PixelDataGet(window_t * wnd,
		gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < wnd->data->geo.w && y < wnd->data->geo.h) {

	return *( wnd->data->FrameBuff + wnd->data->geo.w * y + x);

	}
	return 0;
}



uint8_t ssd1351_NewFrame(ssd1351_t * disp) {

	static uint8_t prevState = 0;

	/* display state is flipped when frame buffer transmission finishes */
	if(prevState != disp->FrameBuff.State) {

		prevState = disp->FrameBuff.State;

		ssd1351_Window_RenderAll(disp);
		ssd1351_UpdateAll(disp);

		return 1; /* success: new frame began rendering on display */
	}

	return 0; /* error: previous frame rendering not yet finished */
}

int8_t ssd1351_Window_Create(window_t * wnd, ssd1351_t * disp,
		layer_e layer, gfx_pos_t x, gfx_pos_t y, gfx_pos_t w, gfx_pos_t h){

	if(wnd == NULL) return -1; /* error: heap overflow */

	wnd->data->FrameBuff = HAL_Malloc(MALLOC_SIMPLE, w * h * PIXEL_SIZE);

	wnd->data->geo.x = x;
	wnd->data->geo.y = y;
	wnd->data->geo.w = w;
	wnd->data->geo.h = h;
	wnd->data->layer = layer;
	wnd->data->display = disp;

	if(disp->WindowList->tqh_first == NULL) {

		TAILQ_INSERT_HEAD(disp->WindowList, wnd, WindowNodes);
		return 1; /* success: inserted at head */
	}

	for (window_t * wndCur = disp->WindowList->tqh_first; wndCur != NULL;
			wndCur = wndCur->WindowNodes.tqe_next) {

		/* sort from lowest to highest head to tail */
		if(wnd->data->layer > wndCur->data->layer) {

			/* todo make so it adds before higher layer, after same layer,
			 * so the same number layers have priority by creation time */

			TAILQ_INSERT_AFTER(disp->WindowList, wndCur, wnd, WindowNodes);
			return 2; /* success: inserted between head and tail */
		}
	}
	TAILQ_INSERT_TAIL(disp->WindowList, wnd, WindowNodes);

	return 3; /* success: inserted at tail */

}
int8_t ssd1351_Window_Delete(window_t * wnd){

	free(wnd->data->FrameBuff);
	TAILQ_REMOVE(disp->WindowList, wnd, WindowNodes);
	memset(wnd, NULL, sizeof(window_t));

	return 1;
}

