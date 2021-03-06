/** idf example
 *
 * This example shows how to create tasks
 *
 * Some parts are copied from the official idf examples found on 
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/
 * 
 * Modified by SteFam @ http://www.fambach.net
 *
 * GPLv3 
 * Blah blah blah, free to use. 
 * I invested some time and money in those examples if you want to delete this header 
 * please do not forget to mention at least my website, thanks!
*/


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"


static const char* TAG = "L6_Task";

// event groupe things
static TaskHandle_t xHandles[5];
static EventGroupHandle_t xEventGroup;
static EventBits_t uxBits;

#define BIT_0    ( 1 << 0 )
#define BIT_1    ( 1 << 1 )
#define BIT_2    ( 1 << 2 )
#define BIT_3    ( 1 << 3 )
#define BIT_4    ( 1 << 4 )


/** tasks 
 *  Simply produce some output
*/
static void task(void* arg)
{
	// get back task index
    uint32_t task_index = (uint32_t*) arg;
	ESP_LOGI(TAG,"Sub Task %i started!",task_index+1);	
		
	// run at minimum 2 times 
	int count = 2 +  esp_random()%10;
			
	for(int i = 0; i < count ; i++){
				
		ESP_LOGD(TAG,"Task %i (run %i / %i )", task_index+1, i, count);
		
		// random delay 250 - 500 ms
		int delay = 250 +  esp_random()%250;
		
		vTaskDelay(delay);
	}
		
	// Do not forgett to stop the task.
	// If delete is missing an exception will be thrown
	ESP_LOGD(TAG,"Stopping %s ", pcTaskGetName(xHandles[task_index]));
	vTaskDelete( xHandles[task_index] );
	
	// you will never see this message, because task was deleted before!
	ESP_LOGI(TAG,"Sub Task %i finished!",task_index+1);
	
}


void app_main(void)
{
	ESP_LOGI(TAG,"Main Task started!");	
	//Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
	
	// activate random number genneration without wifi
	// deactivate before the use of wifi or I2C
	ESP_LOGD(TAG,"Init Random without RF");
	bootloader_random_enable();
				
	//* init switches and set interupt routines
	for(uint32_t i = 0; i <5;i++){
			
		char buf [20];
		sprintf(buf, "Task_%i",i+1);
			
		ESP_LOGD(TAG,"Creating %s", buf);
		
		//start gpio task to handle the buttons
		xTaskCreate(task, buf, 2048, i, 10, &xHandles[i]);
			
	}
	ESP_LOGI(TAG,"Main Task finihed!");	
}


