/** idf example
 *
 * Logging example
 * Loglevel can be either configured by menuconfig or by LOG_LOCAL_LEVEL define.
 * Also a module specific loglevel is possible, 
 * but it must be lower or equal to the general defined loglevel. 
 *
 *
 * Some parts are copied from the official idf examples found on 
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/
 * 
 * Modified by SteFam @ http://www.fambach.net
 *
 * GPLv3 
 * Blah blah blah, free to use. 
 * I invested some time and mony in those examples if you want to delete this header 
 * please do not forget to mention at least my website, thanks!
*/

static const char* TAG = "LoggingExample";

// Undocument this to override the log level defined by menuconfig
// #define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "sdkconfig.h"




void app_main(void)
{
	// Undocument this if you want a module specific log level
	//esp_log_level_set(TAG,ESP_LOG_INFO);
	
	ESP_LOGE(TAG, "Error Message: %d", 1);
	ESP_LOGW(TAG, "Warn Message: %d", 2 );
	ESP_LOGI(TAG, "Information Message: %d", 3);
	ESP_LOGD(TAG, "Debug Message: %d", 4);
	ESP_LOGV(TAG, "Verbose Message: %d", 5);
	
}

