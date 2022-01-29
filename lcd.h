/*
  name: Kelemen Gergo
    date: 2021. 07. 19.
  
 */

#ifndef _LCD_H
#define _LCD_H
#endif

#include <stdio.h>
#include "pico/stdlib.h"

/* HD44780 driver IC */
void init_gps_lcd(void);
void lcd_init();
void lcd_clock();
void lcd_data(uint8_t data);
void lcd_cmd(uint8_t cmd);
void lcd_puts (char*s);
void lcd_busy();
void lcd_setcursor( uint8_t x, uint8_t y );
void lcd_shift_r();
void lcd_shift_l();
void lcd_clr();
/* HD44780 driver IC */

