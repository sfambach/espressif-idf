/** idf example
 *
 * Timed gpio example (Blink like application)
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
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

static const char* TAG ="L6";
static const int LED_PIN = CONFIG_LED_PIN;
static const int DELAY   = CONFIG_LED_BLINK_DELAY_MS;


// method declaration for timer callbacks
static void timer_callback(void* arg);

void app_main(void)
{
	//* setup gpio pins
	 gpio_reset_pin(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
	
	
	//* setup timer
	
	//** create arguments for timer task
	const esp_timer_create_args_t periodic_timer_args = {
			/* name of the callback function defined before */
			.callback = &timer_callback,
			/* name is optional, but may help identify the timer when debugging */
			.name = "periodic"
    };
	
	//** create time itself
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));	
	
	//** start the timer periodic
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, DELAY*1000));
	
	//** for a single shot do this
	// ESP_ERROR_CHECK(esp_timer_start_single(periodic_timer, DELAY*1000));
	
}

static bool curState = 0;
static void timer_callback(void* arg)
{
	curState = !curState;
	ESP_LOGV(TAG,"Current LED State = %s", (curState?"TRUE":"FALSE"));
	gpio_set_level(LED_PIN, !curState);
}
