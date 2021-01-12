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
#include "freertos/stream_buffer.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "bootloader_random.h"


static const char* TAG = "L6_Task";

// event groupe things
static TaskHandle_t senderHandle;
static TaskHandle_t receiverHandle;
static StreamBufferHandle_t streamBuffer;
TickType_t x5s;

static const char sendString[40] = "Go hang a salami - I'm a Lasagna Hog!\n";




/** tasks 
 *  Simply produce some output
*/
static void send(void* arg)
{
	size_t xBytesSent;

		xBytesSent =  xStreamBufferSend( streamBuffer, ( void * ) sendString , sizeof( sendString ), x5s );
		if(xBytesSent != sizeof( sendString )){
			ESP_LOGE(TAG, "Error while sending only sent %i o %i bytes",xBytesSent,sizeof( sendString ));
		}
							
						

	// Do not forgett to stop the task.
	// If delete is missing an exception will be thrown
	ESP_LOGD(TAG,"Stopping sender");
	vTaskDelete( senderHandle);
	
}

static void receive(void* arg){
	
	char rxBuffer[40];

		size_t xReceivedBytes = xStreamBufferReceive( streamBuffer,
											( void * ) rxBuffer,
											sizeof( rxBuffer ),
											x5s );
		if(xReceivedBytes > 0 ){ 
			ESP_LOGI(TAG,"Rec. Bytes %i Content: %s" ,xReceivedBytes,rxBuffer);
		} else {
			ESP_LOGW(TAG,"No data received");
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
	x5s = pdMS_TO_TICKS( 5000 );

	
	// activate random number genneration without wifi
	// deactivate before the use of wifi or I2C
	ESP_LOGD(TAG,"Init Random without RF");
	bootloader_random_enable();
	
	// create stream buffer
	ESP_LOGD(TAG,"Creating stream buffer!");
	
	// buffer can contain 4 bytes maximum, this is very small let's see if it works
	const size_t xStreamBufferSizeBytes = 40;
	// trigger receiver if 1 byte received (10 = 10 Bytes received)
	const size_t xTriggerLevel = 1;
	
	streamBuffer = xStreamBufferCreate( xStreamBufferSizeBytes, xTriggerLevel );

        if( streamBuffer == NULL )
        {
           ESP_LOGE(TAG,"FAILED to create stream buffer!");
        }
        else
        {
            ESP_LOGI(TAG,"Streambuffer created");
        }
	
	
	ESP_LOGD(TAG,"Creating receiver");
	xTaskCreate(receive, "Receiver", 2048, NULL, 10, &receiverHandle);
	configASSERT( receiverHandle );
	
				
	ESP_LOGD(TAG,"Creating sender");
	xTaskCreate(send, "Sender", 2048, NULL, 10, &senderHandle);	
	configASSERT( senderHandle );
		
			
	
			
		
	
}


