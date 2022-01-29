/*
  name: Kelemen Gergo
    date: 2022. 01. 29.
  
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"

int main() {
    /* inits */
    stdio_init_all();
    init_gps_lcd();
    lcd_init();

    /* example string to do smh*/

    char welcome_text[]= "Hello world!";
    char example_text_one[] = "This is a basic";
    char example_text_two[] = "example";
    char example_text_three[] = "of the LCD driver.";

    lcd_puts(welcome_text);

    sleep_ms(5000);

    lcd_clr();
    lcd_setcursor(3,1);
    lcd_puts(example_text_one); lcd_setcursor(7,2);
    lcd_puts(example_text_two); lcd_setcursor(1,3);
    lcd_puts(example_text_three);

    while (true) {

        sleep_ms(20);
        
    }

    return 0;
}



