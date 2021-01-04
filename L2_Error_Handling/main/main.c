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
#include <esp_err.h>

void app_main(void)
{
	
	//* setup timer
	printf("Simply give ok code to check function and see whats happend\n");	
	ESP_ERROR_CHECK(ESP_OK);
	
	printf("And hopefully you see nothing between this and the last output\n");
	
		
	printf(esp_err_to_name(0x106));
	 
	char text[30];
	esp_err_to_name_r(0x105,text,30);
	printf(text);
	
	
	// undocument this to produce an error
	//ESP_ERROR_CHECK(ESP_ERR_INVALID_MAC);
	
}

