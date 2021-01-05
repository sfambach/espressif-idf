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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define ESP_INTR_FLAG_DEFAULT 0
static const char* TAG = "L6_INPUT";
static bool enabled[] = {CONFIG_SW0_ENABLE,CONFIG_SW1_ENABLE,CONFIG_SW2_ENABLE};
static int pin[] = {CONFIG_SW0_PIN,CONFIG_SW1_PIN,CONFIG_SW2_PIN};
static bool debounce[] = {true,true,true};
#ifdef CONFIG_LONG_PRESS_ENABLED
static uint64_t press_time[] = {0,0,0};
#endif
static xQueueHandle gpio_evt_queue = NULL;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
	// get argument(s)
    uint32_t index = (uint32_t) arg;
	
	// create an event
	xQueueSendFromISR(gpio_evt_queue, &index, NULL);
}

/** button tasks 
 *  check for gpio events 
*/
static void button_task(void* arg)
{
    uint32_t index; // =(uint32_t) arg;
	    
	while(true) {

        if(xQueueReceive(gpio_evt_queue, &index, portMAX_DELAY)) {
			//printf("GPIO[%d] intr, val: %d\n", pin[index], gpio_get_level(pin[index]));
			bool level = gpio_get_level(pin[index]);
			if(debounce[index] != level){
			
				debounce[index] = level;
				if(level){
					
#ifdef CONFIG_LONG_PRESS_ENABLED
					int32_t elapsed_time= (int32_t)(esp_timer_get_time()-press_time[index]);
					elapsed_time /=1000;
					//elapsed_time -= CONFIG_LONG_PRESS_TIME_MS; // if > 0 a long press was done
					ESP_LOGD(TAG,"GPIO[%d] OFF ms[%i] %spress \n", pin[index], elapsed_time, (elapsed_time - CONFIG_LONG_PRESS_TIME_MS> 0? "long" : "short"));
					press_time[index]=0;
#else 
					ESP_LOGD(TAG,"GPIO[%d] OFF \n", pin[index]);
#endif
					
				} else {
					ESP_LOGD(TAG,"GPIO[%d] ON\n", pin[index]);
					
#ifdef CONFIG_LONG_PRESS_ENABLED
					press_time[index]=esp_timer_get_time();
#endif
				}

			}else {
				//ESP_LOGD(TAG,"Bounce %i",pin[index]);
			}
		} else {
			ESP_LOGD(TAG, "Timeout waiting for Interrupt, go on next round");			
		}
	}
}


void app_main(void)
{
	//install gpio isr service
	ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
	
		
	//* init switches and set interupt routines
	for(int i =0; i <3;i++){
		if(enabled[i]){
			// set up pin 
			ESP_LOGI(TAG,"SW%i Activated - Pin =%i ",i,pin[i]);
			ESP_ERROR_CHECK(gpio_reset_pin(pin[i]));
			
			ESP_ERROR_CHECK(gpio_set_direction(pin[i], GPIO_MODE_INPUT));
			
			//change gpio intrrupt type for one pin
			ESP_ERROR_CHECK(gpio_set_intr_type(pin[i], GPIO_INTR_ANYEDGE));

			// enable interupt
			ESP_ERROR_CHECK(gpio_intr_enable(pin[i]));
			
			// set level
			ESP_ERROR_CHECK(gpio_intr_enable(pin[i]));
			
			// pull mode 
			ESP_ERROR_CHECK(gpio_set_pull_mode(pin[i],GPIO_PULLDOWN_ONLY));
		
			// discrete functions for pull mode
			// enable pullup
			//ESP_ERROR_CHECK(gpio_pullup_en(pin[i]));
			
			// disable pulldown
			//ESP_ERROR_CHECK(gpio_pulldown_dis(pin[i]));

				   
			//hook isr handler for specific gpio pin
			ESP_ERROR_CHECK(gpio_isr_handler_add(pin[i], gpio_isr_handler, (void*)i));
	
		}
	
	}
	
	//create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	
    //start gpio task to handle the buttons
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
	
}


