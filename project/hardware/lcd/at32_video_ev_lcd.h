/**
  **************************************************************************
  * @file     at32_video_ev_lcd.h
  * @brief    at32_video_ev_lcd header file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#ifndef __AT32_VIDEO_EV_LCD_H
#define __AT32_VIDEO_EV_LCD_H
#include "at32f435_437_board.h"

#define LCD_WIDTH            	           240
#define LCD_HEIGHT           	           320

#define TOUCH_POINT_SIZE           	     3

typedef struct
{
  uint16_t width;			  //lcd width
  uint16_t height;		  //lcd height
  uint16_t id;				    //lcd id
  uint8_t dir;			     //lcd direction
  uint16_t	wramcmd;	  //write ram cmd
  uint16_t setxcmd;   //set x coordinate cmd
  uint16_t setycmd;   //set y coordinate cmd
} lcd_dev_struct;

extern const unsigned char asc2_1206[][12];
extern const unsigned char asc2_1608[][16];
extern const unsigned char asc2_2412[][36];
extern lcd_dev_struct lcddev;
/* point color and background color */
extern uint16_t point_color;
extern uint16_t back_color;

/* scan direction, 0: 0, 1: 90, 2: 180, 3: 270 */
#define L2R_U2D                          0 //left to right, up to down
#define L2R_D2U                          1 //left to right, down to up
#define R2L_U2D                          2 //right to left, up to down
#define R2L_D2U                          3 //right to left, down to up
                            
#define U2D_L2R                          4 //up to down, left to right
#define U2D_R2L                          5 //up to down, right to left
#define D2U_L2R                          6 //down to up, left to right
#define D2U_R2L                          7 //down to up, right to left

#define DFT_SCAN_DIR                     L2R_U2D

/* color value */
#define WHITE         	                  0xFFFF
#define BLACK         	                  0x0000
#define BLUE         	                   0x001F
#define BRED                             0xF81F
#define GRED 			                         0xFFE0
#define GBLUE			                         0x07FF
#define RED           	                  0xF800
#define TOUCH_RED      	                 0x00F8
#define MAGENTA       	                  0xF81F
#define GREEN         	                  0x07E0
#define CYAN          	                  0x7FFF
#define YELLOW        	                  0xFFE0
#define BROWN 			                        0xBC40
#define BRRED 			                        0xFC07
#define GRAY  			                        0x8430

#define DARKBLUE      	                  0x01CF
#define LIGHTBLUE      	                 0x7D7C
#define GRAYBLUE       	                 0x5458

#define LIGHTGREEN     	                 0x841F
#define LIGHTGRAY                        0xEF5B
#define LGRAY 			                        0xC618

#define LGRAYBLUE                        0xA651
#define LBBLUE                           0x2B12

void lcd_wr_reg(uint8_t regval);
void lcd_wr_data(uint8_t data);
void lcd_wr_data16(uint16_t data);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_big_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p);
void lcd_show_char(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);
void lcd_set_window(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend);
void lcd_set_cursor(uint16_t Xpos, uint16_t Ypos);
void lcd_scan_dir(uint8_t dir);
void ili9341_ivo24_initial(void);
void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
uint32_t lcd_pow(uint8_t m, uint8_t n);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
uint16_t lcd_bgr2rgb(uint16_t c);

#endif

