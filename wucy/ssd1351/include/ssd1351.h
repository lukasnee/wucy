/*
 * ssd1351.h
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

/* ================================================================================ */
/* |							SSD1351 DRIVER GUIDE							  |	*/
/* ================================================================================ */
/*
 *   To use this library, first edit user_hal.c - configure contents of USER_HAL_...()
 *   callback functions as they are described.
 *
 *   explanation:
 *   For this program to work you need to give it an access to GPIO and communication
 *   interface. This is done by configuring user_hal.c so wrapping this lib's software
 *   API layer around your specific hardware framework usually called HAL.
 *
 *   This library was designed on ESP32 (ESP-IDF framework) so there's already
 *   an example of user_hal.c code constructed for this system, you can use it as is,
 *   or as a guide for your implementation.
 *
 *	definitions
 *
 *	HAL - Hardware Abstraction Layer.
 */


#ifndef COMPONENTS_SSD1351_FAST_H_
#define COMPONENTS_SSD1351_FAST_H_


#include <stdint.h>

#include "ssd1351_cmd.h"


#define USE_WINDOWS


/* ================================================================================ */
/* |						User SSD1351 configurations							  |	*/
/* ================================================================================ */


#define CLK_DIV_RATIO 0x00   /* cmd B3h A[3:0] */

#define PHASE_1_PERIOD_VAL 5  		/* range: 5 -31 */
#define PHASE_2_PERIOD_VAL 3		/* range: 3 -15 */
#define DCLK_CURR_DRV_PERIOD 50 	/* phase 4 current drive stage */
#define MUX_RATIO 127 				/* Range: 15 - 127 (16MUX ~ 128MUX accordingly)*/

/* ================================================================================ */
/* |								SSD1351 HAL				  					  |	*/
/* ================================================================================ */

/* formatting display settings */

#define SSD1351_WIDTH 128
#define SSD1351_HEIGHT 128

#define DISP_RED_BIT_DEPTH 5
#define DISP_GREEN_BIT_DEPTH 6
#define DISP_BLUE_BIT_DEPTH 5

#define PIXEL_SIZE 2 /* in bytes */

#define DISP_A_MAX ((1 << 8) - 1) /* display alpha (transperancy) max value */

#define DISP_R_MAX ((1 << DISP_RED_BIT_DEPTH) - 1) /* display red gray scale max value */
#define DISP_G_MAX ((1 << DISP_GREEN_BIT_DEPTH) - 1) /* display green gray scale max value */
#define DISP_B_MAX ((1 << DISP_BLUE_BIT_DEPTH) - 1) /* display blue gray scale max value */

#define TOTAL_PIXELS DISP_WIDTH * DISP_HEIGHT
#define VRAM_SIZE TOTAL_PIXELS * PIXEL_SIZE

#define AVAILABLE_FRAMERATE (DISP_SPI_CLOCK_FREQ / DISP_WIDTH / DISP_HEIGHT / PIXEL_SIZE / 8) /* for 14.5Mhz SPI ~ 55 fps */

#define PHASE_1_PERIOD 		((PHASE_1_PERIOD_VAL - 1) / 2)
#define PHASE_2_PERIOD 		(PHASE_2_PERIOD_VAL)
#define PHASE_PERIODS_CMD 	(uint8_t)((PHASE_2_PERIOD << 4) | PHASE_1_PERIOD)
#define DISPLAY_CLOCKS_PER_ROW (PHASE_1_PERIOD + PHASE_2_PERIOD + DCLK_CURR_DRV_PERIOD)

#define CLK_DIV_RATIO_VAL 	(1 << CLK_DIV_RATIO)

#define OSC_FREQ (CLK_DIV_RATIO_VAL * DISPLAY_CLOCKS_PER_ROW * MUX_RATIO / AVAILABLE_FRAMERATE) /* cmd  B3h A[7:4] */

#define DIV_AND_OSC_CMD (uint8_t)((CLK_DIV_RATIO << 0) | (OSC_FREQ << 4)) /* formats config byte for B3h command */

typedef uint8_t cmd_list_t;
#define CMD_LIST_TERMINATE 0x00

typedef enum{DC_COMMAND = 0, DC_DATA} dc_e;


typedef enum{
	DISPLAY_ALL_OFF = SSD1351_CMD_DISPLAY_ALL_OFF,
	DISPLAY_ALL_ON = SSD1351_CMD_DISPLAY_ALL_ON,
	DISPLAY_NORMAL = SSD1351_CMD_DISPLAY_NORMAL,
	DISPLAY_INVERSE = SSD1351_CMD_DISPLAY_INVERSE
}display_mode_e;

typedef enum{
	SLEEP_ON = SSD1351_CMD_SLEEP_MODE_ON,
	SLEEP_OFF = SSD1351_CMD_SLEEP_MODE_OFF
}sleep_mode_e;


/* display management type structure START */

typedef struct{

	int32_t CLK, MOSI, MISO, CS, DC, RES, EN;

}ssd1351_spi_t;

/*todo: parallel 8080 support not added yet */
typedef struct{

	int32_t WR, RD, CS, DC, RES, EN;
	uint8_t * DATA_BUS;/* todo: not quite sure how it will work yet*/

}ssd1351_8_bit_parallel_t;

typedef struct{

	ssd1351_spi_t * 			SPI;
	ssd1351_8_bit_parallel_t * 	Prll;

} interface_t; /* interface pinout */

typedef enum{UNINITIALIZED, INITIALIZED}ssd1351_init_e;
typedef enum{SSD1351_IF_SPI, SSD1351_IF_8_BIT} ssd1351_hal_ci_e; /* HAL Communication Interfaces (CI) */
typedef enum{EN_ACTIVE_HIGH, EN_ACTIVE_LOW} ssd1351_en_polarity_e;

typedef struct{

	ssd1351_init_e 			Init;
	ssd1351_hal_ci_e 		Interface;
	ssd1351_en_polarity_e 	EN_Polarity;

}ssd1351_status_t;

/* display management type structure END */


#define WINDOW_SIZE(WINDOW) (WINDOW->x * WINDOW->y)
#define WINDOW_LEFT_EDGE(WINDOW) (WINDOW->x)
#define WINDOW_RIGHT_EDGE(WINDOW) (WINDOW->x + WINDOW->w - 1)
#define WINDOW_BOTTOM_EDGE(WINDOW) (WINDOW->y)
#define WINDOW_TOP_EDGE(WINDOW) (WINDOW->y + WINDOW->h - 1)


typedef uint16_t pixel_vram_t;

typedef struct{

	pixel_vram_t b:DISP_BLUE_BIT_DEPTH;
	pixel_vram_t g:DISP_GREEN_BIT_DEPTH;
	pixel_vram_t r:DISP_RED_BIT_DEPTH;

}pixel_t;




typedef struct{

	uint8_t a; /* alpha transperancy: 0 - fully opaque, 0xFF - fully transperant */
	uint8_t r; /* red */
	uint8_t g; /* green */
	uint8_t b; /* blue */

}rgba_t;

typedef struct{

	interface_t 				Pin;
	ssd1351_status_t 			Status;

}ssd1351_t;

#ifndef USE_WINDOWS
/* pixel coordinate on the display type */
typedef int16_t gfx_pos_t;

typedef struct{

	gfx_pos_t x; /* x position on display (reference to left edge of frame) */
	gfx_pos_t y; /* y position on display (reference to bottom edge of frame) */

	/* coordinates x and y together references bottom left corner of the frame */

	gfx_pos_t w; /* width of frame in pixels */
	gfx_pos_t h; /* height of frame in pixels */

}gfx_geo_t;
#endif

#ifdef SSD1351_PRIV_ACCESS

/* ================================================================================ */
/* |								SSD1351 Functions		  					  |	*/
/* ================================================================================ */

void ssd1351_SendData(ssd1351_t * disp, uint8_t * data, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

int8_t ssd1351_InitViaSPI(ssd1351_t * disp, ssd1351_spi_t * interface);
int8_t ssd1351_InitVia8080(ssd1351_t * disp, ssd1351_8_bit_parallel_t * interface);

void ssd1351_DeInit(ssd1351_t * disp);

void ssd1351_PowerOn(ssd1351_t * disp);
void ssd1351_PowerOff(ssd1351_t * disp);

void ssd1351_DisplayMode(ssd1351_t * disp, display_mode_e mode);
void ssd1351_Sleep(ssd1351_t * disp, sleep_mode_e mode);

pixel_vram_t ssd1351_color_HEX2PixelData(uint32_t color_hex);

/* todo OLD BAD CODE, NEEDS FIXING */
#ifndef USE_WINDOWS

void ssd1351_SetAll(ssd1351_t * disp);
void ssd1351_ClearAll(ssd1351_t * disp);

void ssd1351_PixelSet(ssd1351_t * disp, gfx_pos_t x, gfx_pos_t y, pixel_vram_t data);
pixel_vram_t ssd1351_PixelGet(ssd1351_t * disp, gfx_pos_t x, gfx_pos_t y);
#endif /* USE_WINDOWS */

#endif /* SSD1351_PRIV_ACCESS */

#endif /* COMPONENTS_SSD1351_FAST_H_ */
