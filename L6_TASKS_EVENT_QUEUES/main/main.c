/** idf example
 *
 * This should be an example for tasks ... 
 * this example is not finished jet
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
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"


static const char* TAG = "L6_Task_Queues";

// event groupe things
static TaskHandle_t writeTaskHandle;
static TaskHandle_t readTaskHandle;
static QueueHandle_t xQueue;
static const int queueSize = 5;

/**
 * Read from queue
*/
static void write_task (void* arg)
{
	const TickType_t xTicksToWait = ( TickType_t )0xff;
	
	// run forever like a highlander
	while(1)
	{
		// generate random value
		uint32_t value = esp_random()%100;
		
		// try to send something
		if(!xQueueSendToBack(xQueue, (void*)&value, xTicksToWait)){
			ESP_LOGW(TAG,"q - Queue Full");
			vTaskDelay(500); // wait longer if queue is full
		}else {
			ESP_LOGI(TAG,"w <- %i", value);
		}
		
		vTaskDelay(500);
	}
	
}



/** 
 *  Write to queue
*/
static void read_task(void* arg)
{
	const TickType_t xTicksToWait = ( TickType_t )0xff;
	uint32_t value;
	int i = 1200;
	// run forever like a highlander
	while(1)
	{
		// increase read speed 
		if(i > 100){ 
			i-=50;
		}
		
		// try to receive some data
		if( xQueueReceive( xQueue, &value, xTicksToWait ) ){
			ESP_LOGI(TAG,"r -> %i", value);
		} else {
			ESP_LOGW(TAG,"q - Queue empty!");
		}
		
		// wait a while 
		vTaskDelay(i);
	}
}

void app_main(void)
{
	
	// Create a queue of queueSize x uint32_t
	xQueue = xQueueCreate( queueSize, sizeof( uint32_t ) );
	if( xQueue == 0 )
	{
		ESP_LOGE(TAG,"Error during queue creation");
	}
	
	//Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
	
	// activate random number genneration without wifi
	// deactivate before the use of wifi or I2C
	ESP_LOGD(TAG,"Init Random without RF");
	bootloader_random_enable();
				
			
	ESP_LOGI(TAG,"Create write task");
	xTaskCreate(write_task, "Write Task", 2048, NULL , 10, &writeTaskHandle);
	configASSERT( writeTaskHandle );
	
	ESP_LOGI(TAG,"Create read task");
	xTaskCreate(read_task, "Read Task", 2048, NULL, 10, &readTaskHandle);
	configASSERT( readTaskHandle );
	
}


