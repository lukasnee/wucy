/*
 * user_hal.c
 *
 *  Created on: 2019-09-05
 *      Author: lukas.neverauskis
 */

/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
	<https://github.com/therram/wucy>

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
#include <wucy_api.h>
#include <wucy_hal.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
//#include "heap/include/esp_heap_caps.h"
#include "driver/spi_master.h"

#include "esp_system.h"
//#include "esp_spi_flash.h"
#include "driver/gpio.h"

	/* USER PRIVATE INCLUDES END */

/* USER DEFINES START */

	/* USER DEFINES END */



/* USER PIRVATE VARIABLES START */

	/* USER PIRVATE VARIABLES END */





/* ================================================================================ */
/* |								Common HAL									  |	*/
/* |							(Must be filled in)								  |	*/
/* ================================================================================ */

/* @brief Initialize pin as output.
 *
 * @param 	pin Pin to initialize.
 *
 */
inline void wucy_hal_PinInit(uint32_t pin, w_pin_dir_e direction, w_pin_pull_e pull_mode) {

	/* USER CODE START */

	gpio_set_direction((int32_t)pin, (gpio_mode_t)direction);
	gpio_set_pull_mode((int32_t)pin, (gpio_pull_mode_t) pull_mode);


	/* USER CODE END */

}






/* @brief 	Write digital output to GPIO.
 *
 * @param[in] disp Display handler.
 * @param[in] pin number to write.
 * @param[in] state - 	0 - Set output to low.
 * 						1 - Set output to high.
 *
 */
inline void wucy_hal_PinWrite(uint32_t pin, uint8_t state) {

	/* USER CODE START */

	gpio_set_level(pin, state);

	/* USER CODE END */

}




/* @brief 	Read digital output from GPIO.
 *
 * @param[in] disp Display handler.
 *
 * @return pin state: 0(GND) or 1(VCC)
 *
 */
uint8_t wucy_hal_PinRead(uint32_t pin) {

	return gpio_get_level(pin);

}





/* @brief 	Dummy delay by given milliseconds.
 *
 * @param 	ms Milliseconds to delay.
 *
 */
inline void wucy_hal_DelayMs(uint32_t ms) {

	/* NOTE !
	 * Initialize selected Communication Interface elsewhere
	 * before the initialization of display.
	 */

	/* USER CODE START */

	vTaskDelay(ms / portTICK_PERIOD_MS);

	/* USER CODE END */

}



/* @brief Specialized memory allocation of full frame buffer sized by
 * VRAM_SIZE definition (in bytes), use it for malloc "size" parameter.
 * This memory block will be transmitted to display's RAM trough interface
 * selected by you.
 *
 * If you do not need any optimization - use simple malloc() or
 * pvPortMalloc() if FreeRTOS is used. Otherwise, read below.
 *
 * @param malloc_type[in] Can be MALLOC_SIMPLE or MALLOC_SPECIALIZED_DMA. Adapt both.
 * If the interface is SPI and you choose to use DMA channel, then it is sometimes
 * necessary to allocate memory in DMA capable memory field for DMA to function
 * properly. Therefore, this function is for SPECIALIZED allocation and is intended
 * to let the user optimize data transmission.
 *
 * @return 	pointer to allocated memory block.
 *
 */
inline void * wucy_hal_SpecMalloc(w_malloc_type_e malloc_type, uint32_t size) {

 /* Use VRAM_SIZE definition as size for memory allocation  */

	switch(malloc_type){

	case MALLOC_SPECIALIZED_DMA:

		/* USER CODE START */

		return heap_caps_malloc(size, MALLOC_CAP_DMA);

		/* USER CODE END */
		break;
	}

	/* if no specialization used, declare single malloc function here
	 * and leave switch() unfilled.
	 */
	/* USER CODE START */



	/* USER CODE END */

	return NULL; /* todo: error */
}






/* ================================================================================ */
/* |								SPI interface HAL							  |	*/
/* |							(Filling in is optional)						  |	*/
/* ================================================================================ */



//#include "esp_heap_caps.h"

/* todo pin should be defined in ssd1351 lib and
 * referenced here in spi_bus_config_t and spi_pre_transfer_callback etc. */

spi_transaction_t spi_transaction;
esp_err_t retval;


esp_err_t ret;

spi_bus_config_t buscfg = {

	.max_transfer_sz = DISP_FRAMEBUFF_SIZE,
	.mosi_io_num = 23,
	.sclk_io_num = 19,

    .miso_io_num = -1,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,

};


/* This function is called (in irq context!) just before a transmission starts. */
void spi_pre_transfer_callback(spi_transaction_t *t)
{
	uint8_t dc = (uint8_t)t->user;
    gpio_set_level(22, dc);  /*todo figure out what to do before spi message */
}


spi_device_handle_t spi_oled;

spi_device_interface_config_t devcfg = {

    .clock_speed_hz = DISP_SPI_CLOCK_FREQ,           		//Clock out at 26 MHz
    .mode = 0,                                				//SPI mode 0
    .queue_size = 1,                          				//We want to be able to queue 7 transactions at a time
    .pre_cb = spi_pre_transfer_callback,  					//Specify pre-transfer callback to handle D/C line
	.spics_io_num = -1, 									/* CS pin */
};






/* @brief 	[optional] Routine called just before initialization of the display on SPI interface.
 * 			Usually for peripheral setup.
 *
 */
inline void wucy_hal_SPI_Init(void) {

	/* NOTE !
	 * Initialize selected communication interface elsewhere
	 * before the initialization of the display.
	 */
	/* USER CODE START */

	spi_bus_initialize(VSPI_HOST, &buscfg, 1); /* using SPI3 and dma channel 1 */

	spi_bus_add_device(VSPI_HOST, &devcfg, &spi_oled);

	//spi_bus_initialize(HSPI_HOST,


	/* USER CODE END */

}






/* @brief 	[optional] Routine called just after deinitialization of display on SPI interface.
 *
 */
inline void wucy_hal_SPI_DeInit(void) {

	/* USER CODE START */

	spi_bus_remove_device(spi_oled);

	/* USER CODE END */

}






/* @brief 	Transmit data via SPI.
 *
 * @param[in] 	data Pointer to data array.
 * @param[in] 	size Number of bytes to send.
 * @param[in] 	(optional) dc
 * 								0 - command
 * 								1 - data
 *
 */
inline void wucy_hal_SPI_Transmit(uint8_t * data, uint32_t size, uint8_t dc) {

	/* USER CODE START */

	memset(&spi_transaction, 0, sizeof(spi_transaction));       		/* Clean transaction structure */
	spi_transaction.length = size * 8;            						/* bytes to transmit (in bits). */
	spi_transaction.tx_buffer = data;              						/* data buffer to transmit */
	spi_transaction.user = (void*)dc;								/* Pass CD level so the transmit callback would set it accordingly  */

	retval = spi_device_transmit(spi_oled, &spi_transaction);			/* Begin transmission */
	//retval = spi_device_polling_transmit(spi_oled, &spi_transaction);
	assert(retval == ESP_OK);   										/* Should have had no issues. */

	/* USER CODE END */

}


/* ================================================================================ */
/* |						8080 Parallel 8-bit interface HAL					  |	*/
/* |							 (Filling in is optional)						  |	*/
/* ================================================================================ */




/* @brief 	[optional] Routine called just before initialization of display on parallel interface.
 * 			Usually for peripheral setup.
 *
 */
inline void wucy_hal_Parallel_Init(void) {

	/* NOTE !
	 * Initialize selected communication interface elsewhere
	 * before the initialization of the display.
	 */

	/* USER CODE START */

	/* USER CODE END */

}






/* @brief 	[optional] Routine called just after deinitialization of display on parallel interface.
 *
 */
inline void wucy_hal_Parallel_DeInit(void) {

	/* USER CODE START */

	/* USER CODE END */

}






/* @brief 	Transmit data via Parallel port.
 *
 * @param 	data Pointer to data array.
 * @param 	size Number of bytes to send.
 *
 */
inline void wucy_hal_Parallel_Transmit(uint8_t * data, uint32_t size, uint8_t dc) {



	/* USER CODE START */



	/* USER CODE END */

}
