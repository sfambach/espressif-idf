/** idf example
 *
 * Playing with tasks and message buffer.
 * This example uses two taks and a message buffer to transfer messages from one task to another in a safe manner.
 * Only one reader and one writer is allowed, in case of more than one writer the send function should be part of 
 * a critical ares safed by a semaphor or mutex
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
#include "freertos/message_buffer.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"


static const char* TAG = "L6_Task";

// event groupe things
static TaskHandle_t senderHandle;
static TaskHandle_t receiverHandle;

uint16_t recDelay = 500;


#define MESSAGE_BUFFER_SIZE 1000
static MessageBufferHandle_t bufferHandle;
TickType_t timeOut;



// some messages ... 
static const char* sendString[12] = {
"Sore was I ere I saw Eros",
"A man, a plan, a canal -- Panama",
"Never a foot too far, even.",
"Euston saw I was not Sue.",
"Live on evasions? No, I save no evil.",
"Red Roses run no risk, sir, on nurses order.",
"Salisbury moor, sir, is roomy. Rub Silas.",
"Marge, let's went. I await news telegram.",
"A new order began, a more Roman age bred Rowena.",
"I, man, am regal; a German am I.",
"Tracy, no panic in a pony-cart.",
"Egad! Loretta has Adams as mad as a hatter. Old age!"};


/** tasks 
 *  Simply produce some output
*/
static void send(void* arg)
{
	while (1) {
		size_t xBytesSent;
		
		for(int i = 0 ; i < 12; i++){
			 
			xBytesSent =  xMessageBufferSend( bufferHandle, ( void * ) sendString[i], strlen( sendString[i]), timeOut );
			if(xBytesSent < 1){
				ESP_LOGE(TAG, "Error while sending only sent %i o %i bytes",xBytesSent,strlen( sendString[i] ));
				recDelay -= 50;
			} else {
				ESP_LOGI(TAG,"Send Bytes %i Content: %s" ,xBytesSent, sendString[i] );
			}
		
			// delay 100 - 500 ms					
			vTaskDelay(pdMS_TO_TICKS(100+esp_random()%400));				
		}
	}
	// Do not forgett to stop the task.
	// If delete is missing an exception will be thrown
	ESP_LOGD(TAG,"Stopping sender");
	vTaskDelete( senderHandle);
	
}

static void receive(void* arg){
	
	char rxBuffer[50];
	while(1){
		
		int length = xMessageBufferNextLengthBytes(bufferHandle);
		size_t xReceivedBytes = xMessageBufferReceive( bufferHandle,
											( void * ) rxBuffer,
											length,
											timeOut );
		
		// fix null termination		
		rxBuffer[length]='\0';
		
		if(xReceivedBytes > 0 ){ 
			ESP_LOGI(TAG,"Received Bytes %i Content: %s" ,xReceivedBytes,(char*)rxBuffer);
		} else {
			ESP_LOGW(TAG,"No data received");
		}
		
		// delay 500 - 1000 ms
		vTaskDelay(pdMS_TO_TICKS(recDelay+esp_random()%500));
	}

	// Do not forgett to stop the task.
	// If delete is missing an exception will be thrown
	ESP_LOGD(TAG,"Stopping receiver ");
	vTaskDelete( receiverHandle);
}


void app_main(void)
{
	//Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));
	timeOut = pdMS_TO_TICKS( 500 );
	
	// activate random number genneration without wifi
	// deactivate before the use of wifi or I2C
	ESP_LOGD(TAG,"Init Random without RF");
	bootloader_random_enable();
	
	// create stream buffer
	ESP_LOGD(TAG,"Creating stream buffer!");

	bufferHandle = xMessageBufferCreate( MESSAGE_BUFFER_SIZE);

        if( bufferHandle == NULL )
        {
           ESP_LOGE(TAG,"FAILED to create buffer!");
        }
        else
        {
           ESP_LOGI(TAG,"Buffer created");
        }
	
	ESP_LOGD(TAG,"Creating receiver");
	xTaskCreate(receive, "Receiver", 2048, NULL, 10, &receiverHandle);
	configASSERT( receiverHandle );
	
				
	ESP_LOGD(TAG,"Creating sender");
	xTaskCreate(send, "Sender", 2048, NULL, 10, &senderHandle);	
	configASSERT( senderHandle );
	
}

