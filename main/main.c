
#include <wucy.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"


void app_main(void)
{

	wucy_Start();

	/* todo only works if task scheduler is already running and
	 * this function is inside a task. Need to create a specific kernel task */
	while(1){

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}


}
