#include <stdio.h> 
#include "tasks.hpp" 
#include "utilities.h" 
#include "io.hpp"

int main(void) {
	printf("Hello World\n");
	while(1)
	{
		if (SW.getSwitch(1)) { /* Check if button 1 is pressed */
			LE.on(1); /* Turn on LED # 1 */
        		LD.setNumber(1); /* LED display will show "1" */
			printf("LED 1 ON!\n");
			delay_ms(1000);
		}
		if (SW.getSwitch(2)) { /* Check if button 1 is pressed */
			LE.on(2); /* Turn on LED # 1 */
        		LD.setNumber(2); /* LED display will show "1" */
			printf("LED 2 ON!\n");
			delay_ms(1000);
		}
    		if (SW.getSwitch(3)) { /* Check if button 1 is pressed */
			LE.on(3); /* Turn on LED # 1 */
        		LD.setNumber(3); /* LED display will show "1" */
			printf("LED 3 ON!\n");
			delay_ms(1000);
		}
    		if (SW.getSwitch(4)) { /* Check if button 1 is pressed */
			LE.on(4); /* Turn on LED # 1 */
        		LD.setNumber(4); /* LED display will show "1" */
			printf("LED 4 ON!\n");
			delay_ms(1000);
		}
    		else {
        		LE.setAll(0); /* Turn off all LEDs */
        		LD.clear(); /* Clear the display */
		}
    }
}
