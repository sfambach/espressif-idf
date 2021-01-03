/** idf example
 *
 * 2. Example Check for error 
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


#include <stdio.h>
#include <esp_system.h>

void app_main(void)
{
	
	//* setup timer
	printf("Simply give ok code to check function and see whats happend\n");	
	ESP_ERROR_CHECK(ESP_OK);
	printf("And hopefully you see nothing, try ESP_ERR_INVALID_MAC \n");
	ESP_ERROR_CHECK(ESP_ERR_INVALID_MAC);
	
}

