/** idf example
 *
 * This is an example for the use of semaphores
 * We have some tasks that tries to change the same resource
 * the source is shielded by the semaphore.
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
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"


static const char* TAG = "L6_Task";

// event groupe things
static TaskHandle_t xHandles[5];
SemaphoreHandle_t xSemaphore = NULL; 

static uint32_t specialResource = 0;

// (de)activat this to dis-/en-able the semaphores
#define enable_semaphore

/** tasks 
 *  Simply produce some output
*/
static void task(void* arg)
{
	// get back task index
    uint32_t task_index = (uint32_t*) arg;
		
	// run at minimum 2 times 
	int count = 2 +  esp_random()%10;
			
	for(int i = 0; i < count ; i++){
				
		ESP_LOGD(TAG,"Task %i, try to get the semapore",task_index+1);
		
#ifdef enable_semaphore
		while(! xSemaphoreTake( xSemaphore, 90)){
			ESP_LOGW(TAG,"Task %i, didn't get the semaphore",task_index+1);
		}
#endif
		
		ESP_LOGI(TAG,"Task %i, got the semophore",task_index+1);
		
		//local copy of resource 
		uint32_t localCopy = specialResource;
				
		// a short delay to give the other task the chance 
		// to get the semaphor and change the value ;)
		vTaskDelay(200);

		// simply increase the value
		specialResource = localCopy + 1; 

		ESP_LOGI(TAG,"Task %i, Value = %i -> Give semaphor back",task_index+1,specialResource);
		
#ifdef enable_semaphore		
		// give back the semaphore
		xSemaphoreGive( xSemaphore );
#endif		
		
		// random delay 250 - 500 ms
		int delay = 250 +  esp_random()%250;
		vTaskDelay(delay);
	}
	
	// Do not forgett to stop the task.
	// If delete is missing an exception will be thrown
	ESP_LOGI(TAG,"Stopping %s ", pcTaskGetName(xHandles[task_index]));
	vTaskDelete( xHandles[task_index] );
	
}


void app_main(void)
{
	
	//Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
	
	// activate random number genneration without wifi
	// deactivate before the use of wifi or I2C
	ESP_LOGI(TAG,"Init Random without RF");
	bootloader_random_enable();
				
				
	// init the semaphor
	xSemaphore = xSemaphoreCreateBinary();
	if( xSemaphore == NULL )
	{ 
		ESP_LOGE(TAG,"Error during creation of semaphor");
	} else {
		ESP_LOGI(TAG,"Semaphor created!");
	}
				
				
	//* init switches and set interupt routines
	for(uint32_t i = 0; i <5;i++){
			
		char buf [20];
		sprintf(buf, "Task_%i",i+1);
			
		ESP_LOGI(TAG,"Creating %s", buf);
		
		//start gpio task to handle the buttons
		xTaskCreate(task, buf, 2048, i, 10, &xHandles[i]);
			
	}
	
	// GO GO GO ...
	// before you can use the semaphor you have to release it once
	xSemaphoreGive( xSemaphore );	
}


