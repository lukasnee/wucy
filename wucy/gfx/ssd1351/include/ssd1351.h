/*
 * ssd1351.h
 *
 *  Created on: 2019-09-05
 *      Author: lukas.neverauskis
 */

/*
	ssd1351 - software driver for ssd1351 display driver.
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ssd1351_cmd.h"



/* ================================================================================ */
/* |						User SSD1351 configurations							  |	*/
/* ================================================================================ */

#define SSD1351_WIDTH 128
#define SSD1351_HEIGHT 128

#define SSD1351_R_BD 5
#define SSD1351_G_BD 6
#define SSD1351_B_BD 5

#define PIXEL_SIZE 2 /* in bytes */



#define SPI_CLOCK_FREQ 14500000
#define CLK_DIV_RATIO 0x00   /* cmd B3h A[3:0] */

#define PHASE_1_PERIOD_VAL 5  		/* range: 5 -31 */
#define PHASE_2_PERIOD_VAL 3		/* range: 3 -15 */
#define DCLK_CURR_DRV_PERIOD 50 	/* phase 4 current drive stage */
#define MUX_RATIO 127 				/* Range: 15 - 127 (16MUX ~ 128MUX accordingly)*/

/* ================================================================================ */
/* |								SSD1351 HAL				  					  |	*/
/* ================================================================================ */

/* formatting display settings */

#define SSD1351_RANGE_H (SSD1351_WIDTH - 1)
#define SSD1351_RANGE_W (SSD1351_HEIGHT - 1)

#define SSD1351_A_MAX ((1 << 8) - 1)
#define SSD1351_R_MAX ((1 << SSD1351_R_BD) - 1)
#define SSD1351_G_MAX ((1 << SSD1351_G_BD) - 1)
#define SSD1351_B_MAX ((1 << SSD1351_B_BD) - 1)

#define TOTAL_PIXELS SSD1351_WIDTH * SSD1351_HEIGHT
#define VRAM_SIZE TOTAL_PIXELS * PIXEL_SIZE

#define AVAILABLE_FRAMERATE (SPI_CLOCK_FREQ / SSD1351_WIDTH / SSD1351_HEIGHT / PIXEL_SIZE / 8) /* for 14.5Mhz SPI ~ 55 fps */

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

typedef enum{ MALLOC_SIMPLE, MALLOC_SPECIALIZED_DMA}malloc_type_e;

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

	pixel_vram_t b:SSD1351_B_BD;
	pixel_vram_t g:SSD1351_G_BD;
	pixel_vram_t r:SSD1351_R_BD;

}pixel_t;

/* format 0xAARRGGBB where 0xAA - alpha byte, 0xRR - Red byte, 0xGG - Green byte, 0xBB - blue byte. */
typedef uint32_t c_hex_t;

typedef struct{

	uint8_t a; /* alpha transperancy: 0 - fully opaque, 0xFF - fully transperant */
	uint8_t r; /* red */
	uint8_t g; /* green */
	uint8_t b; /* blue */

}rgba_t;


/* pixel coordinate on the display type */
typedef int16_t gfx_pos_t;

typedef struct{

	gfx_pos_t x; /* x position on display (reference to left edge of frame) */
	gfx_pos_t y; /* y position on display (reference to bottom edge of frame) */

	/* coordinates x and y together references bottom left corner of the frame */

	gfx_pos_t w; /* width of frame in pixels */
	gfx_pos_t h; /* height of frame in pixels */

}gfx_geo_t;

typedef struct{

	gfx_geo_t geo; /* geometry */

	uint8_t layer; /* frame priority: drawn behind higher priority frames
	and in front of lower priority frames */

	pixel_vram_t * VRAM;	/* background buffer */

}window_t;


typedef struct{

	interface_t 				Pin;
	ssd1351_status_t 			Status;
	window_t 	 				MainFrame; /* background image (full vram frame buffer) */

}ssd1351_t;

/* ================================================================================ */
/* |								SSD1351 Functions		  					  |	*/
/* ================================================================================ */


void ssd1351_display_Update(ssd1351_t * disp, uint8_t * data, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void ssd1351_display_UpdateAll(ssd1351_t * display);


int8_t ssd1351_Init_via_SPI(ssd1351_t * display, ssd1351_spi_t * interface);
int8_t ssd1351_Init_via_8080(ssd1351_t * display, ssd1351_8_bit_parallel_t * interface);
void ssd1351_DeInit(ssd1351_t * display);
void ssd1351_PowerOn(ssd1351_t * display);
void ssd1351_PowerOff(ssd1351_t * display);

void ssd1351_DisplayMode(ssd1351_t * display, display_mode_e mode);
void ssd1351_Sleep(ssd1351_t * display, sleep_mode_e mode);

pixel_vram_t ssd1351_color_rgba2PixelVRAM(rgba_t rgba);

pixel_vram_t ssd1351_color_hex2PixelVRAM(c_hex_t color_hex);

void ssd1351_PixelDataSet(window_t * window, gfx_pos_t x, gfx_pos_t y, pixel_vram_t data);
pixel_vram_t ssd1351_PixelDataGet(window_t * window, gfx_pos_t x, gfx_pos_t y);



#endif /* COMPONENTS_SSD1351_FAST_H_ */
