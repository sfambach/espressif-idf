/** idf example
 *
 * Example 5 for custom configuration for menuconfig
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
#include "esp_system.h"

static const char* text = CONFIG_OUTPUT_TEXT;

void app_main(void)
{
    // single console output 
	printf("%s \n",text);
	
}


