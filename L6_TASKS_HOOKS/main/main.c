/** Example
 *
 * Play with tasks, ticks and hooks
 * This example ist the frist I wrote myself, 
 * because of the missing espressif examples
 * 
 * Created by SteFam @ http://www.fambach.net
 *
 * GPLv3 
 * Blah blah blah, free to use. 
 * I invested some time and money in those examples if you want to delete this header 
 * please do not forget to mention at least my website, thanks!
 *
 *
*/


#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"

static TaskHandle_t calcTask;
static TaskHandle_t tickTask1;
static TaskHandle_t tickTask2;
static const char* TAG = "L6_Task";


/******************** the hooks *************************/

/** Hook 1*/
static bool hook1(void)
{
	ESP_LOGI(TAG,"Hook 1");
	return true;
	
}

/** Hook 2*/
static bool hook2(void)
{
	ESP_LOGI(TAG,"Hook 2");
	return true;
	
}


/******************** the tick *************************/

// tick counters
static int ctick1 = 0;
static int ctick2 = 0;

/** the tick */
static void tick1(void)
{
	if(ctick1 %100 == 0){
		xTaskResumeFromISR(tickTask1);		
	}
	ctick1++;
}


static void tick2(void)
{
	if(ctick2 %100 == 0){
		xTaskResumeFromISR(tickTask2);
		
	}
	ctick2++;
}


static void tick_task(void* args){
	while(1){
		// suspend our selves
		vTaskSuspend(NULL);
		
		// print the tick to log.
		uint32_t i = (uint32_t) args;
		ESP_LOGI(TAG,"100x Tick %i",i);
	}
}

/******************** calc task *************************/
/**
 * Another calculation task
 * so some calculation to keep the cpu bussy
*/
static void calc_task (void* arg)
{
	while(1){
		uint32_t result = 10;
		for(uint32_t i = 0 ; i < 9000; i++){

			result += i+i*1;
			vTaskDelay(pdMS_TO_TICKS(1));	
		}
		ESP_LOGI(TAG,"2 - result generated");
		vTaskDelay(pdMS_TO_TICKS(esp_random()%100));
	}
}

/******************** main task *************************/

void app_main(void)
{
	
	bootloader_random_enable();
	
	// register two hooks
	ESP_ERROR_CHECK(esp_register_freertos_idle_hook(hook1));
	ESP_ERROR_CHECK(esp_register_freertos_idle_hook(hook2));
	
	// register two ticks
	ESP_ERROR_CHECK(esp_register_freertos_tick_hook(tick1));
	ESP_ERROR_CHECK(esp_register_freertos_tick_hook(tick2));
	
	// create tick tasks
	xTaskCreate(tick_task, "Tick Task1", 2048, 1 , 9000, &tickTask1);
	configASSERT( tickTask1 );
	xTaskCreate(tick_task, "Tick Task2", 2048, 2 , 9000, &tickTask2);
	configASSERT( tickTask2 );
	
	
	
	//ESP_LOGI(TAG,"Create calculation task");
	xTaskCreate(calc_task, "Calc Task", 3048, NULL , 9000000, &calcTask);
	configASSERT( calcTask );
	
	while(1){
		
		uint32_t result = 10;
		for(uint32_t i = 0 ; i < 9000000; i++){

			result += i+i*1;
			
			
		}
		ESP_LOGI(TAG,"1 - result generated");
		vTaskDelay(pdMS_TO_TICKS(esp_random()%100));
	}
}


