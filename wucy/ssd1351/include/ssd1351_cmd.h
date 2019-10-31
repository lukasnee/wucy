/*
 * ssd1351_cmd.h
 *
 *  Created on: 2019-09-25
 *      Author: lukas.neverauskis
 */
/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
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

#ifndef WUCY_GFX_SSD1351_INCLUDE_SSD1351_CMD_H_
#define WUCY_GFX_SSD1351_INCLUDE_SSD1351_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================================ */
/* |						SSD1351 fundamental commands 						  |	*/
/* ================================================================================ */

 /*  Note: (D/C# = 0, R/W#(WR#)= 0, E(RD#) = 1) unless specific setting is stated
  *  Single byte command (D/C# = 0),
  *  Multiple byte command (D/C# = 0 for first byte, D/C# = 1 for other bytes)
  *
  *  A[:], B[:], C[:], ... - Following Command Data bytes
  */

#define SSD1351_CMD_SET_ADDRESS_COLUMN 0x15			/* Following data:
 	 	 	 	 	 	 	 	 	 	 	 	 	 * A[6:0]: Start Address. [reset=0]
													 * B[6:0]: End Address. [reset=127]
													 * Range from 0 to 127
													 */

#define SSD1351_CMD_SET_ADDRESS_ROW 0x75 			/* Following data:
													 * A[6:0]: Start Address. [reset=0]
													 * B[6:0]: End Address. [reset=127]
													 * Range from 0 to 127
													 * */

#define SSD1351_CMD_RAM_WRITE 0x5C 					/* Enable MCU to write Data into RAM  */

#define SSD1351_CMD_RAM_READ 0x5D 					/* Enable MCU to read Data from RAM  */

#define SSD1351_CMD_CONFIG_REG 0xA0					/* Following data byte bits configuration:
													 * A[0]=0b, Horizontal address increment [reset]
													 * A[0]=1b, Vertical address increment
													 *
													 * A[1]=0b, Column address 0 is mapped to SEG0 [reset]
													 * A[1]=1b, Column address 127 is mapped to SEG0
													 *
													 * A[2]=0b, Color sequence: A ¯ B ¯ C [reset]
													 * A[2]=1b, Color sequence is swapped: C ¯ B ¯ A
													 *
													 * A[3]=0b, Reserved
													 * A[3]=1b, Reserved
													 *
													 * A[4]=0b, Scan from COM0 to COM[N –1] [reset]
													 * A[4]=1b, Scan from COM[N-1] to COM0. Where N is the Multiplex ratio.
													 *
													 * A[5]=0b, Disable COM Split Odd Even
													 * A[5]=1b, Enable COM Split Odd Even [reset]
													 *
													 * A[7:6] Set Color Depth,
													 *  00b / 01b: 65k color [reset]
													 *  10b: 262k color
													 *  11b 262k color, 16-bit format 2
													 * */

#define SSD1351_CMD_SET_DISPLAY_START_LINE 0xA1 	/* Set vertical scroll by RAM from 0~127. [reset=00h]  */

#define SSD1351_CMD_SET_DISPLAY_OFFSET 0xA2 		/* Set vertical scroll by Row from 0-127. [reset=60h]
													 * Note:
													 * (1) This command is locked by Command FDh by default. To
													 * unlock it, please refer to Command FDh.
													 * */

#define SSD1351_CMD_DISPLAY_ALL_OFF 0xA4 			/* All pixels off. */

#define SSD1351_CMD_DISPLAY_ALL_ON 0xA5 			/* All pixels fully on, GS63 (Gray Scale 64) */

#define SSD1351_CMD_DISPLAY_NORMAL 0xA6 			/* Reset to normal display [reset]. */

#define SSD1351_CMD_DISPLAY_INVERSE 0xA7 			/* Inverse Display (GS0 -> GS63, GS1 -> GS62, ....). Note. GS - Gray Scale */

#define SSD1351_CMD_FUNCTION_SELECT 0xAB 			/* Following data:
													 * A[0]=0b, Disable internal VDD regulator (for power save
													 * during sleep mode only)
													 * A[0]=1b, Enable internal VDD regulator [reset]
													 * A[7:6]=00b, Select 8-bit parallel interface [reset]
													 * A[7:6]=01b, Select 16-bit parallel interface
													 * A[7:6]=11b, Select 18-bit parallel interface
													 * */

#define SSD1351_CMD_NOP  0xAD 						/* Command for no operation.  */

#define SSD1351_CMD_SLEEP_MODE_ON 0xAE 				/* Sleep mode On (Display OFF)  */

#define SSD1351_CMD_SLEEP_MODE_OFF 0xAF 			/*  Sleep mode OFF (Display ON)  */

// #define SSD1351_CMD_NOP 0xB0 						/* Command for no operation.  */

#define SSD1351_CMD_PHASE_PERIODS 0xB1 				/* A[3:0] Phase 1 period of 5~31 DCLK(s) clocks
													 * [reset=0010b]
													 * A[3:0]:
													 * 0-1 invalid
													 * 2 = 5 DCLKs
													 * 3 = 7 DCLKs
													 * :
													 * 15 = 31DCLKs
													 *
													 * A[7:4] Phase 2 period of 3~15 DCLK(s) clocks
													 * [reset=1000b]
													 * A[7:4]:
													 * 0-2 invalid
													 * 3 = 3 DCLKs
													 * 4 = 4 DCLKs
													 * :
													 * 15 =15DCLKs
													 *
													 * Note:
													 * (1) 0 DCLK is invalid in phase 1 & phase 2
													 * (2)This command is locked by Command FDh by default. To
													 * unlock it, please refer to Command FDh.
													 */

#define SSD1351_CMD_DISPLAY_ENHANCEMENT 0xB2 		/* A[7:0] = 00h, B[7:0] = 00h, C[7:0] = 00h normal [reset]
													 * A[7:0] = A4h, B[7:0] = 00h, C[7:0] = 00h enhance display performance.
 	 	 	 	 	 	 	 	 	 	 	 	 	 */

#define SSD1351_CMD_CLOCK_DIV_AND_OSC_FREQ 0xB3 	/* A[3:0] [reset=0001], divide by DIVSET where
													 * A[3:0] DIVSET
													 * 0000 divide by 1
													 * 0001 divide by 2
													 * 0010 divide by 4
													 * 0011 divide by 8
													 * 0100 divide by 16
													 * 0101 divide by 32
													 * 0110 divide by 64
													 * 0111 divide by 128
													 * 1000 divide by 256
													 * 1001 divide by 512
													 * 1010 divide by 1024
													 * >=1011 invalid
													 * A[7:4] Oscillator frequency, frequency increases as level
													 *  increases [reset=1101b]
													 * Note:
													 * (1) This command is locked by Command FDh by default. To
													 * unlock it, please refer to Command FDh.
													 */

#define SSD1351_CMD_SET_SEGMENT_LOW_VOLTAGE 0xB4 	/* A[1:0]=00 External VSL [reset]
													 * A[1:0]=01,10,11 are invalid
													 * Note:
													 * (1) When external VSL is enabled, in order to avoid distortion
													 *  in display pattern, an external circuit is needed to connect
													 * between VSL and VSS as shown in Figure 14-1
													 */

#define SSD1351_CMD_SET_GPIO 0xB5 					/* A[1:0] GPIO0: 00 pin HiZ, Input disabled
													 *  01 pin HiZ, Input enabled (floating)
													 *  10 pin output LOW [reset]
													 *  11 pin output HIGH
													 *
													 * A[3:2] GPIO1: 00 pin HiZ, Input disabled
													 *  01 pin HiZ, Input enabled (floating)
													 *  10 pin output LOW [reset]
													 *  11 pin output HIGH
													 * */

#define SSD1351_CMD_SET_SECND_PRECHRG_PERIOD 0xB6 	/* A[3:0] Set Second Pre-charge Period
													 *  0000b invalid
													 *  0001b 1 DCLKS
													 *  0010b 2 DCLKS
													 *  ....
													 *  1000 8 DCLKS [reset]
													 *  ....
													 *  1111 15 DCLKS
													 * */

#define SSD1351_CMD_LUT_FOR_GS_PULSE_WIDTH 0xB8 	/* The next 63 data bytes define Gray Scale (GS) Table by
													 * setting the gray scale pulse width in unit of DCLK’s
													 * (ranges from 0d ~ 180d)
													 *
													 * A1[7:0]: Gamma Setting for GS1,
													 * A2[7:0]: Gamma Setting for GS2,
													 *  :
													 * A62[7:0]: Gamma Setting for GS62,
													 * A63[7:0]: Gamma Setting for GS63
													 *
													 * Note:
													 * (1] 0 <= Setting of GS1 < Setting of GS2 < Setting of GS3.....
													 * < Setting of GS62 < Setting of GS63
													 * (2) GS0 has only pre-charge but no current drive stages.
													 * (3) GS1 can be set as only pre-charge but no current drive
													 * stage by input gamma setting for GS1 equals 0.
													 * */

#define SSD1351_CMD_LUT_USE_BUILT_IN_LINEAR 0xB9 	/* Reset to default Look Up Table (LUT):
													 *  GS1 = 0 DCLK
													 *  GS2 = 2 DCLK
													 *  GS3 = 4 DCLK
													 *  GS4 = 6 DCLK
													 *  ...
													 *  GS62 = 122 DCLK
													 *  GS63 = 124 DCLK
													 * */

#define SSD1351_CMD_SET_PRECHARGE_VOLTAGE 0xBB 		/* Set pre-charge voltage level.[reset = 17h]
													 *
													 * 		A[4:0] 		Hex code 		pre-charge voltage
													 * 		00000 		00h 			0.20 x VCC
													 * 		: 			: 				:
													 * 		11111 		1Fh 			0.60 x VCC
													 *
													 * Note:
													 * (1)This command is locked by Command FDh by default. To
													 * unlock it, please refer to Command FDh.
													 */

#define SSD1351_CMD_SET_VCOMH_VOLTAGE 0xBE 			/* Set COM deselect voltage level [reset = 05h]
													 *
													 * 		A[2:0] 	Hex code 	V COMH
													 * 		000 	00h 		0.72 x VCC
													 * 		: 		: 			:
													 * 		101 	05h 		0.82 x VCC [reset]
													 * 		: 		: 			:
													 * 		111 	07h 		0.86 x VCC
													 *
													 * 	Note:
													 * 	(1)This command is locked by Command FDh by default. To
													 * 	unlock it, please refer to Command FDh.
													 */

#define SSD1351_CMD_CONTRAST_CURR_FOR_COLORS 0xC1 	/* A[7:0] Contrast Value Color A [reset=10001010b]
													 * B[7:0] Contrast Value Color B [reset=01010001b]
													 * C[7:0] Contrast Value Color C [reset=10001010b]
													 * */

#define SSD1351_CMD_CONTRAST_CURR_CTRL_MASTER 0xC7 	/* A[3:0] :
													 * 0000b reduce output currents for all colors to 1/16
													 * 0001b reduce output currents for all colors to 2/16
													 * ....
													 * 1110b reduce output currents for all colors to 15/16
													 * 1111b no change [reset]
													 * */

#define SSD1351_CMD_SET_MUX_RATIO 0xCA 				/* A[6:0] MUX ratio 16MUX ~ 128MUX, [reset=127],
 	 	 	 	 	 	 	 	 	 	 	 	 	 * (Range from 15 to 127)
 	 	 	 	 	 	 	 	 	 	 	 	 	 * */

// #define SSD1351_CMD_NOP 0xD1						/* Command for No Operation */

// #define SSD1351_CMD_NOP 0xE3						/* Command for No Operation */

#define SSD1351_CMD_SET_COMMAND_LOCK 0xFD 			/* A[7:0]: MCU protection status [reset = 12h]
													 * A[7:0] = 12b, Unlock OLED driver IC MCU interface from
													 * entering command [reset]
													 *
													 * A[7:0] = 16b, Lock OLED driver IC MCU interface from
													 * entering command
													 *
													 * A[7:0] = B0b, Command A2,B1,B3,BB,BE,C1 inaccessible
													 * in both lock and unlock state [reset]
													 * A[7:0] = B1b, Command A2,B1,B3,BB,BE,C1 accessible if
													 * in unlock state
													 *
													 * 	Note
													 * 	(1) The locked OLED driver IC MCU interface prohibits all
													 * 	commands and memory access except the FDh command.
													 */

/* ================================================================================ */
/* |					SSD1351 Graphic acceleration commands					  |	*/
/* ================================================================================ */

/* Note:
 * Set (GAC) (D/C# = 0, R/W#(WR#)= 0, E(RD#) = 1) unless specific setting is stated
 * Single byte command (D/C# = 0),
 * Multiple byte command (D/C# = 0 for first byte, D/C# = 1 for other bytes)
 */

#define SSD1351_CMD_HORIZONTAL_SCROLL 0x96 			/* A[7:0] = 00000000b No scrolling
													 *
													 * A[7:0] = 00000001b to 00111111b
													 *  Scroll towards SEG127 with 1 column offset
													 *
													 * A[7:0] = 01000000b to 11111111b
													 *  Scroll towards SEG0 with 1 column offset
													 *
													 * B[6:0] : start row address
													 *
													 * C[7:0] : number of rows to be H-scrolled
													 *  B+C <= 128
													 *
													 * D[6:0] : Reserved (reset=00h)
													 *
													 * E[1:0] : scrolling time interval
													 *  00b test mode
													 *  01b normal
													 *  10b slow
													 *  11b slowest
													 *
													 * Note:
													 * (1) Operates during display ON.
													 */

#define SSD1351_CMD_MOVING_STOP 0x9E 				/* Stop horizontal scroll
													 * Note:
													 * (1) After sending 9Eh command to stop the scrolling
													 * action, the ram data needs to be rewritten
													 * */

#define SSD1351_CMD_MOVING_START 0x9F 				/* Start horizontal scroll */

	 /* Index notes:
	  * 	(1) After executed the graphic command, waiting time is required for update GDDRAM content.
	  * 	VCI =2.4~3.5V, waiting time = 500ns/pixel.
	  * 	(2) “*” stands for “Don’t care”.
	  */

#ifdef __cplusplus
}
#endif

#endif /* WUCY_GFX_SSD1351_INCLUDE_SSD1351_CMD_H_ */
