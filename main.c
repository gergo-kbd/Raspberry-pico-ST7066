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

// doc TODO
#define LCD_E 2
#define LCD_CUR 1
#define LCD_BL 0


/* DISPLAY SIZE 16 x 2 or 16 x 4 */

/* HD44780 driver IC */
void init_gps_lcd(void);
void lcd_init();
void lcd_clock();
void lcd_data(uint8_t data);
void lcd_cmd(uint8_t cmd);
void lcd_puts (char*s);
void lcd_busy();
void lcd_setcursor( uint8_t x, uint8_t y );
/* HD44780 driver IC */

int main() {
    /* inits */
    stdio_init_all();
    init_gps_lcd();
    lcd_init();

    /* example */

    lcd_puts("Teszt");
    lcd_setcursor(14,1); lcd_puts("123456");
    lcd_setcursor(0,2);
    lcd_puts("Szoveg"); 
    lcd_setcursor(14,2); lcd_puts("!@#$<>"); 

    lcd_setcursor(0,3); lcd_puts("third"); lcd_setcursor(17,3); lcd_puts("row");
    lcd_setcursor(0,4); lcd_puts("fourth"); lcd_setcursor(17, 4); lcd_puts("row");


    while (true) {
        
    }

    return 0;
}

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
    lcd_cmd(0x28);lcd_cmd(0x28);lcd_cmd(0x28);

    lcd_cmd(0x02); lcd_cmd(0x01); 

    lcd_busy();

    // tobb esetben is stabilabb mukodest produkalt ha egy adott parancsot 3x kuldok ki
    // feltelezem hogy ez a pico gyorsasagabol, es a HD44780 lassusagabol adodik
    unsigned char three_times = 3;
    while(three_times){
        lcd_cmd(0x08 | (1<<LCD_E) | (0<<LCD_CUR) | (0<<LCD_BL));
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