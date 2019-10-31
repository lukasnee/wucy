/*
 * ssd1351.c
 *
 *  Created on: 2019-09-05
 *      Author: lukas.neverauskis
 */

/*
	ssd1351 - software driver for ssd1351 display.
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
#define SSD1351_PRIV_ACCESS
#include "ssd1351.h"

#include <wucy_api.h>

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

	/* 0x10 flips horziontally (referencing pin text as upward)*/
	/* 0x20 flips vertical  (referencing pin text as upward) */
	SSD1351_CMD_CONFIG_REG, 1, 0x34,

	SSD1351_CMD_DISPLAY_NORMAL, 0,

	CMD_LIST_TERMINATE};



static void ssd1351_Transmit(ssd1351_t * disp, dc_e dc, uint8_t * data, uint32_t size) {

	switch(disp->Status.Interface){

	case SSD1351_IF_SPI: wucy_hal_SPI_Transmit(data, size, dc); break;
	case SSD1351_IF_8_BIT: wucy_hal_Parallel_Transmit(data, size, dc); break;

	}
}

/* ======================================================================== */
// 						SSD1351 driver user functions
/* ======================================================================== */


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

		ssd1351_Transmit(disp, DC_COMMAND, command_list + pos, 1); /* send command */

		pos++;	/* step to following command data lenght indicator */

		if(*(command_list + pos) != 0){

			ssd1351_Transmit(disp, DC_DATA, command_list + pos + 1, *(command_list + pos));
		}
		pos+= *(command_list + pos) + 1;

	}
	while(*(command_list + pos) != CMD_LIST_TERMINATE);
}


#ifdef WUCY_OPTIMIZE

void ssd1351_SendData(ssd1351_t * disp,
		uint8_t * data, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

		uint8_t pre_vram_dump[] = {
			SSD1351_CMD_SET_ADDRESS_COLUMN, 2, x1, x2, //0x7F -SSD1351_RANGE_H */
			SSD1351_CMD_SET_ADDRESS_ROW, 2, y1, y2,
			SSD1351_CMD_RAM_WRITE, 0,
		CMD_LIST_TERMINATE};

		ssd1351_SendCommand(disp, pre_vram_dump);

		ssd1351_Transmit(disp, DC_DATA, data, DISP_FRAMEBUFF_SIZE);

}
#else


/*todo consider making data type pxl_vram_t?*/
void ssd1351_SendData(ssd1351_t * disp,
		uint8_t * data, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	uint32_t data_size;

	if(x1 <= DISP_RANGE_W && x2 <= DISP_RANGE_W &&
			y1 <= DISP_RANGE_H && y2 <= DISP_RANGE_H) {


		data_size = (x2 - x1 + 1) * (y2 - y1 + 1) * PIXEL_SIZE;

		uint8_t pre_vram_dump[] = {
			SSD1351_CMD_SET_ADDRESS_COLUMN, 2, x1 & 0x7F, x2 & 0x7F, //0x7F -SSD1351_RANGE_H */
			SSD1351_CMD_SET_ADDRESS_ROW, 2, y1 & 0x7F, y2 & 0x7F,
			SSD1351_CMD_RAM_WRITE, 0,
		CMD_LIST_TERMINATE};

		ssd1351_SendCommand(disp, pre_vram_dump);

		ssd1351_Transmit(disp, DC_DATA, data, data_size);

	}

}

#endif


int8_t ssd1351_InitViaSPI(ssd1351_t * disp, ssd1351_spi_t * interface) {

	if(disp->Status.Init == UNINITIALIZED) {

		disp->Pin.SPI = interface;
		disp->Status.Interface = SSD1351_IF_SPI;

		wucy_hal_PinOutputInit(disp->Pin.SPI->RES);
		wucy_hal_PinOutputInit(disp->Pin.SPI->CS);
		wucy_hal_PinOutputInit(disp->Pin.SPI->DC);
		wucy_hal_PinOutputInit(disp->Pin.SPI->EN);

		wucy_hal_SPI_Init();


		disp->Status.Init = INITIALIZED;

		ssd1351_PowerOn(disp);

		return 0;
	}
	return -1;
}
/* todo: not supported yet */
int8_t ssd1351_InitVia8080(ssd1351_t * disp, ssd1351_8_bit_parallel_t * interface) {

	if(disp->Status.Init == UNINITIALIZED) {

		disp->Pin.Prll = interface;
		disp->Status.Interface = SSD1351_IF_8_BIT;

		wucy_hal_PinOutputInit(disp->Pin.Prll->RES);
		wucy_hal_PinOutputInit(disp->Pin.Prll->CS);
		wucy_hal_PinOutputInit(disp->Pin.Prll->DC);
		wucy_hal_PinOutputInit(disp->Pin.Prll->EN);
		wucy_hal_PinOutputInit(disp->Pin.Prll->WR);
		wucy_hal_PinOutputInit(disp->Pin.Prll->RD);

		wucy_hal_Parallel_Init();

		disp->Status.Init = INITIALIZED;

		ssd1351_PowerOn(disp);

		return 0;
	}
	return -1;
}


void ssd1351_DeInit(ssd1351_t * disp){

	ssd1351_PowerOff(disp);
	switch(disp->Status.Interface){

		case SSD1351_IF_SPI: 	wucy_hal_SPI_DeInit(); break;
		case SSD1351_IF_8_BIT: 	wucy_hal_Parallel_DeInit(); break;

	}
}



void ssd1351_PowerOn(ssd1351_t * disp){

	wucy_hal_PinWrite(disp->Pin.SPI->RES, 0);
	wucy_hal_DelayMs(1);
	wucy_hal_PinWrite(disp->Pin.SPI->RES, 1);
	wucy_hal_DelayMs(1);
	//HAL_PinWrite(disp, disp->Pin.SPI->EN, 1); /* enable high voltage Vcc supply (usually from boost converter) */
	wucy_hal_DelayMs(300); /* delay minimum 300 ms before sending screen on command */

	ssd1351_SendCommand(disp, default_settings);
	ssd1351_Sleep(disp, SLEEP_OFF);

}


void ssd1351_PowerOff(ssd1351_t * disp){

	ssd1351_Sleep(disp, SLEEP_ON);
	switch(disp->Status.Interface){

	case SSD1351_IF_SPI:
		wucy_hal_PinWrite(disp->Pin.SPI->RES,
				disp->Status.EN_Polarity == EN_ACTIVE_LOW ? 1 : 0);
		break;
	case SSD1351_IF_8_BIT:
		wucy_hal_PinWrite(disp->Pin.Prll->RES,
				disp->Status.EN_Polarity == EN_ACTIVE_LOW ? 1 : 0);
		break;
	}
}

void ssd1351_DisplayMode(ssd1351_t * disp, display_mode_e mode){

	cmd_list_t cmd[] = {mode, 0, CMD_LIST_TERMINATE};
	ssd1351_SendCommand(disp, cmd);

}


void ssd1351_Sleep(ssd1351_t * disp, sleep_mode_e mode){

	/*  todo SSD1351_CMD_FUNCTION_SELECT (0xAB) data byte must be merged
	 *  with interface selection bits A[7:6] if 16 - bit parallel port used,
	 *  not implemented yet */

	/* default Command for disable internal VDD regulator */
	cmd_list_t regulator[] = {SSD1351_CMD_FUNCTION_SELECT, 1, 0x00, CMD_LIST_TERMINATE};
	cmd_list_t cmd[] = {mode, 0, CMD_LIST_TERMINATE};

	if(mode == SLEEP_ON || mode == SLEEP_OFF){

		ssd1351_SendCommand(disp, cmd);
		if(mode == SLEEP_OFF){
			wucy_hal_DelayMs(1);
			regulator[2] = 0x01; /* Command for enable internal VDD regulator */
		}

		ssd1351_SendCommand(disp, regulator);

	}
}

pixel_vram_t ssd1351_color_HEX2PixelData(uint32_t color_hex) {

	pixel_vram_t pxl_color;

		pxl_color = (((((color_hex & 0x0000FF00) >> 8 ) * DISP_G_MAX) / 0xFF) << 13) |
					(((((color_hex & 0x000000FF) >> 0 ) * DISP_B_MAX) / 0xFF) << 8 ) |
					(((((color_hex & 0x00FF0000) >> 16) * DISP_R_MAX) / 0xFF) << 3 ) |
					(((((color_hex & 0x0000FF00) >> 8 ) * DISP_G_MAX) / 0xFF) >> 3 );

	return pxl_color;
}


/* todo NEEDS FIXING */
#ifndef USE_WINDOWS

inline void ssd1351_SetAll(ssd1351_t * disp) {

	memset(VRAM_DRAW, 0xFF, VRAM_SIZE);

}

inline void ssd1351_ClearAll(ssd1351_t * disp) {

	memset(VRAM_DRAW, 0x00, VRAM_SIZE);

}


/* for mainframe specific window type */
inline void ssd1351_PixelSet(ssd1351_t * disp,
		gfx_pos_t x, gfx_pos_t y, pixel_vram_t data) {

	if(x >= 0 && y >= 0 && x < disp->FrameBuff.geo.w && y < disp->FrameBuff.geo.h) {

		*(VRAM_DRAW + disp->FrameBuff.geo.w * y + x) = data;

	}
}



/* for mainframe specific window type */
inline pixel_vram_t ssd1351_PixelGet(ssd1351_t * disp,
		gfx_pos_t x, gfx_pos_t y) {

	if(x >= 0 && y >= 0 && x < disp->FrameBuff.geo.w && y < disp->FrameBuff.geo.h) {

		return *(VRAM_DRAW + disp->FrameBuff.geo.w * y + x);

	}
	return 0;
}

#endif

