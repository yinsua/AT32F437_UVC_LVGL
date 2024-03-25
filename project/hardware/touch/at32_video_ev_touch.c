/**
  **************************************************************************
  * @file     at32_video_ev_touch.c
  * @brief    at32_video_ev_touch program
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
  
#include "stdlib.h"
#include "at32_video_ev_touch.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"

uint16_t vx = 15542, vy = 11165; //scale factor
uint16_t chx = 140, chy = 146;   //AD initial value

struct tp_pix_ tp_pixad, tp_pixlcd; //current ad value, current pixel value

#define LV_COLOR_DEPTH                   16
#define LV_COLOR_16_SWAP                 1

/**
  * @brief  read data from touch
  * @param  none
  * @retval read data
  */
uint16_t touch_read_data(void)
{
  uint16_t data;
  data = lcd_spi1_data_receive();
  data = (data<<8) | lcd_spi1_data_receive();
  return (data>>4);
}

/**
  * @brief  read data from touch
  * @param  cmd: write cmd
  * @retval read data
  */
uint16_t touch_adc_read(uint8_t cmd)
{
  uint16_t data;
  LCD_CS2_CLR;
  lcd_spi1_write(cmd);
  data = touch_read_data();
  LCD_CS2_SET;
  return data;
}

/**
  * @brief  read x or y coordinate value
  * @param  xy: read register x or y
  * @retval coordinate value
  */
uint16_t touch_xy_read(uint8_t xy)
{
  uint16_t i, j;
  uint16_t buf[READ_TIMES];
  uint16_t sum = 0;
  uint16_t temp;

  for(i = 0; i < READ_TIMES; i++)
  {
    buf[i] = touch_adc_read(xy);
  }

  for(i = 0; i < READ_TIMES - 1; i++)
  {
    for(j = i + 1; j < READ_TIMES; j++)
    {
      if(buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  sum = 0;

  for(i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
  {
    sum += buf[i];
  }

  temp = sum / (READ_TIMES - 2 * LOST_VAL);
  return temp;
}

/**
  * @brief  read coordinate value
  * @param  x: coordinate x
  * @param  y: coordinate y
  * @retval 0: fail, 1: success
  */
uint8_t touch_coor_read(uint16_t *x, uint16_t *y)
{
  uint16_t xtemp, ytemp;
  xtemp = touch_xy_read(CMD_RDX);
  ytemp = touch_xy_read(CMD_RDY);

  if(xtemp < 100 || ytemp < 100)
  {
    return 0;
  }

  *x = xtemp;
  *y = ytemp;
  return 1;
}

/**
  * @brief  read coordinate value twice
  * @param  x: coordinate x
  * @param  y: coordinate y
  * @retval 0: fail, 1: success
  */
uint8_t touch_coor_read_twice(uint16_t *x, uint16_t *y)
{
  uint16_t x1, y1;
  uint16_t x2, y2;

  if(touch_coor_read(&x1, &y1) == 0)
  {
    return(0);
  }

  if(touch_coor_read(&x2, &y2) == 0)
  {
    return(0);
  }

  if(((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))
      && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
  {
    *x = (x1 + x2) >> 1;
    *y = (y1 + y2) >> 1;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  read precise coordinate value
  * @param  none
  * @retval 0: fail, 1: success
  */
uint8_t touch_precise_coor_read(void)
{
  spi_switch(1);
  while(!touch_coor_read_twice(&tp_pixad.x, &tp_pixad.y));
  spi_switch(0);
  return 1;
}

/**
  * @brief  draw a point for adjust
  * @param  x: coordinate x
  * @param  y: coordinate y
  * @param  color: point color
  * @retval none
  */
void touch_adjust_point_draw(uint16_t x, uint16_t y, uint16_t color)
{
  lcd_draw_line(x - 12, y, x + 13, y, color);
  lcd_draw_line(x, y - 12, x, y + 13, color);
  lcd_draw_point(x + 1, y + 1, color);
  lcd_draw_point(x - 1, y + 1, color);
  lcd_draw_point(x + 1, y - 1, color);
  lcd_draw_point(x - 1, y - 1, color);
}

/**
  * @brief  read precise coordinate value
  * @param  none
  * @retval 0: fail, 1: success
  */
uint8_t coor_convert(void)
{
  uint8_t l = 0;
  spi_switch(1);

  if(touch_coor_read_twice(&tp_pixad.x, &tp_pixad.y))
  {
    l = 1;
    tp_pixlcd.x = tp_pixad.x > chx ? ((uint32_t)tp_pixad.x - (uint32_t)chx) * 1000 / vx : ((uint32_t)chx - (uint32_t)tp_pixad.x) * 1000 / vx;
    tp_pixlcd.y = tp_pixad.y > chy ? ((uint32_t)tp_pixad.y - (uint32_t)chy) * 1000 / vy : ((uint32_t)chy - (uint32_t)tp_pixad.y) * 1000 / vy;
  }

  spi_switch(0);
  return l;
}

/**
  * @brief  adjust touch screen to get 4 parameters
  * @param  none
  * @retval none
  */
void touch_adjust(void)
{
  float vx1, vx2, vy1, vy2;
  uint16_t chx1, chx2, chy1, chy2;
  uint16_t lx, ly;
  struct tp_pixu32_ p[4];
  uint8_t  cnt = 0;
 
  point_color = RED;
  lcd_clear(WHITE);
  touch_adjust_point_draw(TOUCH_OFFSET, TOUCH_OFFSET, RED);
  while(1)
  {
    if(PEN_CHECK == RESET)
    {
      if(touch_precise_coor_read())
      {
        p[cnt].x = tp_pixad.x;
        p[cnt].y = tp_pixad.y;
        cnt++;
      }

      switch(cnt)
      {
        case 1:
          lcd_clear(WHITE);

          while(!PEN_CHECK)
          {
          }

          touch_adjust_point_draw(LCD_WIDTH - TOUCH_OFFSET - 1, TOUCH_OFFSET, RED);
          break;

        case 2:
          lcd_clear(WHITE);

          while(!PEN_CHECK)
          {
          }

          touch_adjust_point_draw(TOUCH_OFFSET, LCD_HEIGHT - TOUCH_OFFSET - 1, RED);
          break;

        case 3:
          lcd_clear(WHITE);

          while(!PEN_CHECK)
          {
          }

          touch_adjust_point_draw(LCD_WIDTH - TOUCH_OFFSET - 1, LCD_HEIGHT - TOUCH_OFFSET - 1, RED);
          break;

        case 4:
          lcd_clear(WHITE);

          while(!PEN_CHECK)
          {
          }

          vx1 = p[1].x > p[0].x ? (p[1].x - p[0].x + 1) * 1000 / (LCD_WIDTH - TOUCH_OFFSET - TOUCH_OFFSET) : (p[0].x - p[1].x - 1) * 1000 / (LCD_WIDTH - TOUCH_OFFSET - TOUCH_OFFSET);
          chx1 = p[1].x > p[0].x ? p[0].x - (vx1 * TOUCH_OFFSET) / 1000 : p[0].x + (vx1 * TOUCH_OFFSET) / 1000;
          vy1 = p[2].y > p[0].y ? (p[2].y - p[0].y - 1) * 1000 / (LCD_HEIGHT - TOUCH_OFFSET - TOUCH_OFFSET) : (p[0].y - p[2].y - 1) * 1000 / (LCD_HEIGHT - TOUCH_OFFSET - TOUCH_OFFSET);
          chy1 = p[2].y > p[0].y ? p[0].y - (vy1 * TOUCH_OFFSET) / 1000 : p[0].y + (vy1 * TOUCH_OFFSET) / 1000;

          vx2 = p[3].x > p[2].x ? (p[3].x - p[2].x + 1) * 1000 / (LCD_WIDTH - TOUCH_OFFSET - TOUCH_OFFSET) : (p[2].x - p[3].x - 1) * 1000 / (LCD_WIDTH - TOUCH_OFFSET - TOUCH_OFFSET);
          chx2 = p[3].x > p[2].x ? p[2].x - (vx2 * TOUCH_OFFSET) / 1000 : p[2].x + (vx2 * TOUCH_OFFSET) / 1000;
          vy2 = p[3].y > p[1].y ? (p[3].y - p[1].y - 1) * 1000 / (LCD_HEIGHT - TOUCH_OFFSET - TOUCH_OFFSET) : (p[1].y - p[3].y - 1) * 1000 / (LCD_HEIGHT - TOUCH_OFFSET - TOUCH_OFFSET);
          chy2 = p[3].y > p[1].y ? p[1].y - (vy2 * TOUCH_OFFSET) / 1000 : p[1].y + (vy2 * TOUCH_OFFSET) / 1000;


          if((vx1 > vx2 && vx1 > vx2 + TOUCH_ADJUST) || (vx1 < vx2 && vx1 < vx2 - TOUCH_ADJUST) || (vy1 > vy2 && vy1 > vy2 + TOUCH_ADJUST) || (vy1 < vy2 && vy1 < vy2 - TOUCH_ADJUST))
          {
            cnt = 0;
            lcd_clear(WHITE);
            touch_adjust_point_draw(TOUCH_OFFSET, TOUCH_OFFSET, RED);
            continue;
          }

          vx = (vx1 + vx2) / 2;
          vy = (vy1 + vy2) / 2;
          chx = (chx1 + chx2) / 2;
          chy = (chy1 + chy2) / 2;

          lcd_clear(WHITE);

          lcd_show_string(10, 30, 240, 20, 16, "Adjust Information As Follow:");
          
          lx = 10;
          ly = 50;
          lcd_show_string(lx, ly, 200, 16, 16, "VX1:");
          lx += 40;
          lcd_show_num(lx, ly, vx1, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "VY1:");
          lx += 40;
          lcd_show_num(lx, ly, vy1, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHX1:");
          lx += 40;
          lcd_show_num(lx, ly, chx1, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHY1:");
          lx += 40;
          lcd_show_num(lx, ly, chy1, 5, 16);

          lx = 110;
          ly = 50;
          lcd_show_string(lx, ly, 200, 16, 16, "VX2:");
          lx += 40;
          lcd_show_num(lx, ly, vx2, 5, 16);
          lx = 110;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "VY2:");
          lx += 40;
          lcd_show_num(lx, ly, vy2, 5, 16);
          lx = 110;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHX2:");
          lx += 40;
          lcd_show_num(lx, ly, chx2, 5, 16);
          lx = 110;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHY2:");
          lx += 40;
          lcd_show_num(lx, ly, chy2, 5, 16);

          lx = 10;
          ly = 150;
          lcd_show_string(lx, ly, 200, 16, 16, "VX:");
          lx += 40;
          lcd_show_num(lx, ly, vx, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "VY:");
          lx += 40;
          lcd_show_num(lx, ly, vy, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHX:");
          lx += 40;
          lcd_show_num(lx, ly, chx, 5, 16);
          lx = 10;
          ly += 20;
          lcd_show_string(lx, ly, 200, 16, 16, "CHY:");
          lx += 40;
          lcd_show_num(lx, ly, chy, 5, 16);

          lx = 10;
          ly += 30;
          lcd_show_string(lx, ly, 200, 16, 16, "Adjust OK!");
          ly += 20;
          lcd_show_string(lx, ly, 240, 16, 16, "Touch Anywhere To Continue!");
          touch_precise_coor_read();

          lcd_clear(WHITE);
 
          return;
      }
    }
  }
}

/**
  * @brief  test touch screen
  * @param  none
  * @retval none
  */
void touch_test(void)
{
  if(PEN_CHECK == 0)
  {
    if(coor_convert())
    {
      if(tp_pixlcd.x > 200 && tp_pixlcd.y > 300)
      {
        lcd_clear(WHITE);
        point_color = WHITE;
        back_color = RED;
        lcd_show_string(200,300,200,16,16,"Clear");
        point_color = RED;
        back_color = WHITE;
        lcd_show_string(10, 300, 200, 16, 16, "X:");
        lcd_show_num(30, 300, 0, 3, 16);
        lcd_show_string(100, 300, 200, 16, 16, "Y:");
        lcd_show_num(120, 300, 0, 3, 16);
      }
      else
      {
        lcd_show_string(10, 300, 200, 16, 16, "X:");
        lcd_show_num(30, 300, (uint32_t)tp_pixlcd.x, 3, 16);
        lcd_show_string(100, 300, 200, 16, 16, "Y:");
        lcd_show_num(120, 300, (uint32_t)tp_pixlcd.y, 3, 16);
        lcd_draw_big_point(tp_pixlcd.x, tp_pixlcd.y, RED);
      }
    }
  }
}
