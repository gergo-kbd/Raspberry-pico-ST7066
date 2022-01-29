/*
  name: Kelemen Gergo
    date: 2021. 07. 19.
  
 */

#include <stdio.h>
#include "pico/stdlib.h"

//data pins
#define D4 2
#define D5 3
#define D6 4
#define D7 5
// busy flag pin = DATA 7 pin
#define BSY_PIN 5

// control pins
#define RS 6
#define RW  7
#define E 8

// for gpio mask
#define FIRST_GPIO 2

// set position defines
#define LCD_ADR_LINE1         0x00
#define LCD_ADR_LINE2         0x40
#define LCD_ADR_LINE3         0x10
#define LCD_ADR_LINE4         0x50
#define LCD_START_DDADR       0x80


// Ive used 4 bit mode always 8 bit is waste of cable/gpio IMO
// 2/1 lines but in my case its 4/2 beacuse I used a 4 row lcd

#define LCD_BITMODE     0           // 4 bit
#define LCD_NUMBER_OF_LINES     1 // 4 row (in docs its 2)
#define LCD_FONT_SIZE   0 // 5x8 font size, 5x11 only usable in 2 or 1 row mode

#define LCD_E 1 // entire display on
#define LCD_CUR 0 // cursor on
#define LCD_BL 0 // cursor position on

/* DISPLAY SIZE 16 x 2 or 16 x 4 */

void init_gps_lcd(void){

    gpio_init(D4);
    gpio_init(D5);
    gpio_init(D6);
    gpio_init(D7);

    gpio_init(RS);
    gpio_init(RW);
    gpio_init(E);

    gpio_set_dir(D4, 1);
    gpio_set_dir(D5, 1);
    gpio_set_dir(D6, 1);
    gpio_set_dir(D7, 1);

    gpio_set_dir(RS, 1);
    gpio_set_dir(RW, 1);
    gpio_set_dir(E, 1);
    
}

void lcd_init(){

    sleep_ms(50); // wait

    gpio_put(RW, 0);
    gpio_put(RS, 0);

    gpio_put_masked(0x0F<<FIRST_GPIO, 0x02);

    lcd_clock(); lcd_clock(); lcd_clock();
    lcd_cmd(0x20 | (LCD_BITMODE<<4) | (LCD_NUMBER_OF_LINES<<3) | (LCD_FONT_SIZE<<2));
    lcd_cmd(0x20 | (LCD_BITMODE<<4) | (LCD_NUMBER_OF_LINES<<3) | (LCD_FONT_SIZE<<2));
    lcd_cmd(0x20 | (LCD_BITMODE<<4) | (LCD_NUMBER_OF_LINES<<3) | (LCD_FONT_SIZE<<2));
    //lcd_cmd(0x28);lcd_cmd(0x28);lcd_cmd(0x28);

    lcd_cmd(0x02); lcd_cmd(0x01); 

    lcd_busy();

    // send it three times to make sure its initialized properly
    
    unsigned char three_times = 3;
    while(three_times){
        lcd_cmd(0x08 | (LCD_E<<2) | (LCD_CUR<<1) | (LCD_BL<<0));
        three_times -- ;
    }

    lcd_cmd(0x01);
    lcd_cmd(0x02);
}

void lcd_clock(){

    gpio_put(E, 1);
    sleep_us(2);
    gpio_put(E, 0);
    sleep_us(2);
}

void lcd_cmd(uint8_t cmd){

    
    uint8_t felso = cmd >> 4 ;
    uint8_t also = cmd & 0x0F ;
    
    gpio_put(RS, 0);
    gpio_put(RW, 0);
    gpio_put(E, 0);

    gpio_put_masked(0x0F<<FIRST_GPIO, 0x00); // clr
    gpio_put_masked(0x0F<<FIRST_GPIO, felso<<FIRST_GPIO); // 4 bit
    lcd_clock();

    gpio_put_masked(0x0F<<FIRST_GPIO, 0x00); // clr
    gpio_put_masked(0x0F<<FIRST_GPIO, also<<FIRST_GPIO); // 4 bit
    lcd_clock();

    sleep_ms(1);

}

void lcd_data(uint8_t data){

    lcd_busy();
    uint8_t felso = data >> 4 ;
    uint8_t also = data & 0x0F ;

    gpio_put(RS, 1);
    gpio_put(RW, 0);
    gpio_put(E, 0);

    gpio_put_masked(0x0F<<FIRST_GPIO, 0x00); // clr
    gpio_put_masked(0x0F<<FIRST_GPIO, felso<<FIRST_GPIO); // 4 bit
    lcd_clock();

    gpio_put_masked(0x0F<<FIRST_GPIO, 0x00); // clr
    gpio_put_masked(0x0F<<FIRST_GPIO, also<<FIRST_GPIO); // 4 bit
    lcd_clock();
}

void lcd_busy(){

	uint8_t busy;
	gpio_set_dir(BSY_PIN, 0); // dir in for busy flag read
	gpio_put(RS, 0); // status info read
    gpio_put(RW, 1);

	
	do{
		busy=0;
		gpio_put(E, 1); // EN 1, nem tudjuk ezelott mi volt
		sleep_us(1);
		busy = gpio_get(BSY_PIN); // BF read
		
        gpio_put(E, 0);
        sleep_us(2);
		gpio_put(E, 1);
        sleep_us(2);
        gpio_put(E, 0);
        sleep_us(2);
		
	}while(busy);
	gpio_put(RW, 0); //R/W<-0 write
	gpio_set_dir(D7, 1); // dir out
	
}

void lcd_clr()
{
    lcd_cmd(0x01);
}

void lcd_puts(char *string){
	while (*string != '\0'){
        lcd_data(*string ++);
    }
}

void lcd_setcursor( uint8_t x, uint8_t y )
{
	uint8_t position;
	
	switch (y)
	{
		case 1:    // 1. sor
		position = LCD_START_DDADR + LCD_ADR_LINE1 + x;
		break;
		
		case 2:    // 2. sor
		position = LCD_START_DDADR + LCD_ADR_LINE2 + x;
		break;
		
		case 3:    // 3. sor
		position = LCD_START_DDADR + (LCD_ADR_LINE3 + 4) + x;
		break;
		
		case 4:    // 4. sor
		position = LCD_START_DDADR + (LCD_ADR_LINE4+ 4) + x;
		break;
		
		default:
		return;                                   
	}

	lcd_cmd(position); // beallitas
    lcd_cmd(position); // beallitas
    lcd_cmd(position); // beallitas
}

void lcd_shift_r()
{
    lcd_cmd(0x1C);
}

void lcd_shift_l()
{
    lcd_cmd(0x18);
}