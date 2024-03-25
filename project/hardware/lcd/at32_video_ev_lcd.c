/**
  **************************************************************************
  * @file     at32_video_ev_lcd.c
  * @brief    at32_video_ev_lcd program
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
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"

/******************************************************************************/
/* point color and background color */
uint16_t point_color = 0x0000;
uint16_t back_color = 0xFFFF;

lcd_dev_struct lcddev;

/**
  * @brief  lcd write register
  * @param  regval: register value
  * @retval none
  */
void lcd_wr_reg(uint8_t regval)
{
  LCD_DC_CLR;//cmd
  lcd_spi1_write(regval);
  LCD_DC_SET;//data
}

/**
  * @brief  lcd write 8bit data
  * @param  data: data value
  * @retval none
  */
void lcd_wr_data(uint8_t data)
{
  lcd_spi1_write(data);
}

/**
  * @brief  lcd write 16bit data
  * @param  data: data value
  * @retval none
  */
void lcd_wr_data16(uint16_t data)
{
  lcd_spi1_write(data >> 8);
  lcd_spi1_write(data & 0xFF);
}

/**
  * @brief  lcd draw a point fast
  * @param  x: x coordinate value
  * @param  y: y coordinate value
  * @param  color: draw color
  * @retval none
  */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
  lcd_set_cursor(x, y);
  lcd_wr_data16(color);
}

/**
  * @brief  lcd draw a big point
  * @param  x: x coordinate value
  * @param  y: y coordinate value
  * @param  color: big point color
  * @retval none
  */
void lcd_draw_big_point(uint16_t x, uint16_t y, uint16_t color)
{
  uint8_t i;
  if((x>0)&&(y>0)&&(x<LCD_WIDTH)&&(y<LCD_HEIGHT))
  {
    for(i = 0; i < TOUCH_POINT_SIZE; i++)
    {
      lcd_set_cursor(x - 1, y - 1 + i);
      lcd_wr_data16(color);
      lcd_wr_data16(color);
      lcd_wr_data16(color);
    }
  }
}

/**
  * @brief  lcd draw a line
  * @param  x1: start x coordinate value
  * @param  y1: start y coordinate value
  * @param  x2: end x coordinate value
  * @param  y2: end y coordinate value
  * @param  color: line color
  * @retval none
  */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, urow, ucol;
  delta_x = x2 - x1;
  delta_y = y2 - y1;
  urow = x1;
  ucol = y1;

  if(delta_x > 0)
  {
    incx = 1;
  }
  else if(delta_x == 0)
  {
    incx = 0;  //perpendicular line
  }
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }

  if(delta_y > 0)
  {
    incy = 1;
  }
  else if(delta_y == 0)
  {
    incy = 0;  //horizontal line
  }
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }

  if( delta_x > delta_y)
  {
    distance = delta_x;
  }
  else
  {
    distance = delta_y;
  }

  for(t = 0; t <= distance + 1; t++ )
  {
    lcd_draw_point(urow, ucol, color);
    xerr += delta_x ;
    yerr += delta_y ;

    if(xerr > distance)
    {
      xerr -= distance;
      urow += incx;
    }

    if(yerr > distance)
    {
      yerr -= distance;
      ucol += incy;
    }
  }
}

/**
  * @brief  lcd draw a rectangle
  * @param  x1: start x coordinate value
  * @param  y1: start y coordinate value
  * @param  x2: end x coordinate value
  * @param  y2: end y coordinate value
  * @param  color: rectangle color
  * @retval none
  */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  lcd_draw_line(x1, y1, x2, y1, color);
  lcd_draw_line(x1, y1, x1, y2, color);
  lcd_draw_line(x1, y2, x2, y2, color);
  lcd_draw_line(x2, y1, x2, y2, color);
}

/**
  * @brief  lcd draw a circle
  * @param  x0: center of circle x coordinate value
  * @param  y0: center of circle y coordinate value
  * @param  r: radius value
  * @param  color: circle color
  * @retval none
  */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
  int a, b;
  int di;
  a = 0;
  b = r;
  di = 3 - (r << 1);

  while(a <= b)
  {
    lcd_draw_point(x0 + a, y0 - b, color);        //5
    lcd_draw_point(x0 + b, y0 - a, color);        //0
    lcd_draw_point(x0 + b, y0 + a, color);        //4
    lcd_draw_point(x0 + a, y0 + b, color);        //6
    lcd_draw_point(x0 - a, y0 + b, color);        //1
    lcd_draw_point(x0 - b, y0 + a, color);
    lcd_draw_point(x0 - a, y0 - b, color);        //2
    lcd_draw_point(x0 - b, y0 - a, color);        //7
    a++;

    /* draw a circle with bresenham algorithm */
    if(di < 0)
    {
      di += 4 * a + 6;
    }
    else
    {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
}

/**
  * @brief  lcd show string
  * @param  x: start x coordinate value
  * @param  y: start y coordinate value
  * @param  width: string width
  * @param  height: string height
  * @param  size: string size
  * @param  p: string point
  * @retval none
  */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p)
{
  uint8_t x0 = x;
  width += x;
  height += y;

  while((*p <= '~') && (*p >= ' '))
  {
    if(x >= width)
    {
      x = x0;
      y += size;
    }

    if(y >= height)
    {
      break;
    }

    lcd_show_char(x, y, *p, size, 0);
    x += size / 2;
    p++;
  }
}

/**
  * @brief  lcd show a character
  * @param  x: start x coordinate value
  * @param  y: start y coordinate value
  * @param  num: character number
  * @param  size: character size
  * @param  mode: superposition mode
  * @retval none
  */
void lcd_show_char(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
  uint8_t temp, t1, t;
  uint16_t y0 = y;
  uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
  num = num - ' ';

  for(t = 0; t < csize; t++)
  {
    if(size == 12)
    {
      temp = asc2_1206[num][t];  //1206 typeface
    }
    else if(size == 16)
    {
      temp = asc2_1608[num][t];  //1608 typeface
    }
    else if(size == 24)
    {
      temp = asc2_2412[num][t];  //2412 typeface
    }
    else
    {
      return;
    }

    for(t1 = 0; t1 < 8; t1++)
    {
      if(temp & 0x80)
      {
        lcd_draw_point(x, y, point_color);
      }
      else if(mode == 0)
      {
        lcd_draw_point(x, y, back_color);
      }

      temp <<= 1;
      y++;

      if(y >= lcddev.height)
      {
        return;
      }

      if((y - y0) == size)
      {
        y = y0;
        x++;

        if(x >= lcddev.width)
        {
          return;
        }

        break;
      }
    }
  }
}

/**
  * @brief  write lcd block
  * @param  xstart: x direction start
  * @param  ystart: y direction start
  * @param  xend: x direction end
  * @param  yend: y direction end
  * @retval none
  */
void lcd_set_window(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend)
{
  lcd_wr_reg(lcddev.setxcmd);
  lcd_wr_data(Xstart >> 8);
  lcd_wr_data(Xstart & 0xff);
  lcd_wr_data(Xend >> 8);
  lcd_wr_data(Xend & 0xff);

  lcd_wr_reg(lcddev.setycmd);
  lcd_wr_data(Ystart >> 8);
  lcd_wr_data(Ystart & 0xff);
  lcd_wr_data(Yend >> 8);
  lcd_wr_data(Yend & 0xff);

  lcd_wr_reg(lcddev.wramcmd);
}

/**
  * @brief  set lcd cursor position
  * @param  sx: x position
  * @param  sy: y position
  * @retval none
  */
void lcd_set_cursor(uint16_t Xpos, uint16_t Ypos)
{
  lcd_wr_reg(lcddev.setxcmd);
  lcd_wr_data(Xpos >> 8);
  lcd_wr_data(Xpos & 0xff);

  lcd_wr_reg(lcddev.setycmd);
  lcd_wr_data(Ypos >> 8);
  lcd_wr_data(Ypos & 0xff);

  lcd_wr_reg(lcddev.wramcmd);
}

/**
  * @brief  set lcd scan direction
  * @param  dir: direction
  *         0: 0, 1: 90, 2: 180, 3: 270
  * @retval none
  */
void lcd_scan_dir(uint8_t dir)
{
  switch(dir)
  {
    case 0:
      lcddev.width = 240;
      lcddev.height = 320;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0x08);
      break;
    case 1:
      lcddev.width = 320;
      lcddev.height = 240;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0xA8);
      break;
    case 2:
      lcddev.width = 240;
      lcddev.height = 320;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0xC8);
      break;
    case 3:
      lcddev.width = 320;
      lcddev.height = 240;
      lcddev.setxcmd = 0x2A;
      lcddev.setycmd = 0x2B;
      lcddev.wramcmd = 0x2C;
      lcd_wr_reg(0x36);
      lcd_wr_data(0x68);
      break;
    default:
      break;
  }
}

/**
  * @brief  initialize lcd ili9341
  * @param  none
  * @retval none
  */
void ili9341_ivo24_initial(void)
{
  lcd_wr_reg(0x11);  //sleep out 
  delay_ms(120);
  lcd_wr_reg(0xCF);
  lcd_wr_data(0x00);
  lcd_wr_data(0xC1);
  lcd_wr_data(0x30);
  lcd_wr_reg(0xED);
  lcd_wr_data(0x64);
  lcd_wr_data(0x03);
  lcd_wr_data(0x12);
  lcd_wr_data(0x81);
  lcd_wr_reg(0xE8);
  lcd_wr_data(0x85);
  lcd_wr_data(0x00);
  lcd_wr_data(0x79);
  lcd_wr_reg(0xCB);
  lcd_wr_data(0x39);
  lcd_wr_data(0x2C);
  lcd_wr_data(0x00);
  lcd_wr_data(0x34);
  lcd_wr_data(0x02);
  lcd_wr_reg(0xF7);
  lcd_wr_data(0x20);
  lcd_wr_reg(0xEA);
  lcd_wr_data(0x00);
  lcd_wr_data(0x00);
  lcd_wr_reg(0xC0);  //power control
  lcd_wr_data(0x1D); //vrh[5:0]
  lcd_wr_reg(0xC1);  //power control
  lcd_wr_data(0x12); //sap[2:0];bt[3:0]
  lcd_wr_reg(0xC5);  //vcm control
  lcd_wr_data(0x33);
  lcd_wr_data(0x3F);
  lcd_wr_reg(0xC7);  //vcm control
  lcd_wr_data(0x92);
  lcd_wr_reg(0x3A);  //memory access control
  lcd_wr_data(0x55);
  lcd_wr_reg(0xB1);
  lcd_wr_data(0x00);
  lcd_wr_data(0x12);
  lcd_wr_reg(0xB6);  //display function control
  lcd_wr_data(0x0A);
  lcd_wr_data(0xA2);

  lcd_wr_reg(0x44);
  lcd_wr_data(0x02);

  lcd_wr_reg(0xF2);  //3gamma function disable
  lcd_wr_data(0x00);
  lcd_wr_reg(0x26);  //gamma curve selected
  lcd_wr_data(0x01);
  lcd_wr_reg(0xE0);  //set gamma
  lcd_wr_data(0x0F);
  lcd_wr_data(0x22);
  lcd_wr_data(0x1C);
  lcd_wr_data(0x1B);
  lcd_wr_data(0x08);
  lcd_wr_data(0x0F);
  lcd_wr_data(0x48);
  lcd_wr_data(0xB8);
  lcd_wr_data(0x34);
  lcd_wr_data(0x05);
  lcd_wr_data(0x0C);
  lcd_wr_data(0x09);
  lcd_wr_data(0x0F);
  lcd_wr_data(0x07);
  lcd_wr_data(0x00);
  lcd_wr_reg(0xE1);  //set gamma
  lcd_wr_data(0x00);
  lcd_wr_data(0x23);
  lcd_wr_data(0x24);
  lcd_wr_data(0x07);
  lcd_wr_data(0x10);
  lcd_wr_data(0x07);
  lcd_wr_data(0x38);
  lcd_wr_data(0x47);
  lcd_wr_data(0x4B);
  lcd_wr_data(0x0A);
  lcd_wr_data(0x13);
  lcd_wr_data(0x06);
  lcd_wr_data(0x30);
  lcd_wr_data(0x38);
  lcd_wr_data(0x0F);
  lcd_wr_reg(0x29);  //display on
}

/**
  * @brief  initialize lcd
  * @param  none
  * @retval none
  */
void lcd_init(void)
{
  lcd_hw_init(); 

  /* lcd reset */
  LCD_RST_CLR;
  delay_ms(100);
  LCD_RST_SET;
  delay_ms(100);
  
  /* lcd backlight on */
  LCD_BLK_SET;
  delay_ms(100);
 
  LCD_CS1_CLR;

  ili9341_ivo24_initial();
  lcd_scan_dir(DFT_SCAN_DIR);
}

/**
  * @brief  clear lcd
  * @param  color: clear lcd with color
  * @retval none
  */
void lcd_clear(uint16_t color)
{
#if 0   /* not use dma */
  uint16_t i, j;
  for(i = 0; i < LCD_HEIGHT; i++)
  {
    lcd_set_cursor(0, i);

    for(j = 0; j < LCD_WIDTH; j++)
    {
      lcd_wr_data16(color);
    }
  }
#else   /* use dma */
 	uint32_t num,num1;
  lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
  LCD_DC_SET;
  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_16BIT);
  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = FALSE;
  num = LCD_WIDTH * LCD_HEIGHT;
  while(num)
  {
    if(num >= 0xFFFE)
    {
      num -= 0xFFFE;
      num1 = 0xFFFE;
    }
    else
    {
      num1 = num;
      num = 0;
    }
    LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = FALSE;
    LCD_SPI_MASTER_Tx_DMA_Channel->dtcnt_bit.cnt = num1;
    LCD_SPI_MASTER_Tx_DMA_Channel->maddr  = (uint32_t)&color;
    LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = TRUE;
    while(dma_flag_get(LCD_SPI_MASTER_Tx_DMA_FLAG) != SET);
    dma_flag_clear(LCD_SPI_MASTER_Tx_DMA_FLAG);
  }
  LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.mincm = TRUE;
  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_8BIT);
#endif  
}

/**
  * @brief  fill lcd in designated area with different color
  * @param  sx: x direction start
  * @param  sy: y direction start
  * @param  ex: x direction end
  * @param  ey: y direction end
  * @param  color: fill color point
  * @retval none
  */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
#if 0   /* not use dma */
  uint16_t i, j;
  for(i = sy; i <= ey; i++)
  {
    lcd_set_cursor(sx, i);

    for(j = sx; j < ex; j++)
    {
      lcd_wr_data16(*color++);
    }
  } 
#else   /* use dma */
	 uint32_t num,num1;
  lcd_set_window(sx, sy, ex - 1, ey - 1);
  LCD_DC_SET;
  num = (ex - sx) * (ey - sy);
  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_16BIT);
  while(num)
  {
    if(num >= 0xFFFE)
    {
      num -= 0xFFFE;
      num1 = 0xFFFE;
    }
    else
    {
      num1 = num;
      num = 0;
    }
    LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = FALSE;
    LCD_SPI_MASTER_Tx_DMA_Channel->dtcnt_bit.cnt = num1;
    LCD_SPI_MASTER_Tx_DMA_Channel->maddr  = (uint32_t)color;
    LCD_SPI_MASTER_Tx_DMA_Channel->ctrl_bit.chen = TRUE;
    while(dma_flag_get(LCD_SPI_MASTER_Tx_DMA_FLAG) != SET);
    dma_flag_clear(LCD_SPI_MASTER_Tx_DMA_FLAG);
    color += num1;
  }
  spi_frame_bit_num_set(LCD_SPI_SELECTED, SPI_FRAME_8BIT);
#endif
}

/**
  * @brief  lcd power
  * @param  m: the base of the power
  * @param  n: the exponential of the power
  * @retval result
  */
uint32_t lcd_pow(uint8_t m, uint8_t n)
{
  uint32_t result = 1;

  while(n--)
  {
    result *= m;
  }

  return result;
}

/**
  * @brief  lcd show a number
  * @param  x: start x coordinate value
  * @param  y: start y coordinate value
  * @param  num: number value
  * @param  len: number length
  * @param  size: number size
  * @retval none
  */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
  uint8_t t, temp;
  uint8_t enshow = 0;

  for(t = 0; t < len; t++)
  {
    temp = (num / lcd_pow(10, len - t - 1)) % 10;

    if(enshow == 0 && t < (len - 1))
    {
      if(temp == 0)
      {
        lcd_show_char(x + (size / 2)*t, y, ' ', size, 0);
        continue;
      }
      else
      {
        enshow = 1;
      }

    }

    lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0);
  }
}

/**
  * @brief  lcd show numbers
  * @param  x: start x coordinate value
  * @param  y: start y coordinate value
  * @param  num: number value
  * @param  len: number length
  * @param  size: number size
  * @param  mode: superposition mode
  * @retval none
  */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
  uint8_t t, temp;
  uint8_t enshow = 0;

  for(t = 0; t < len; t++)
  {
    temp = (num / lcd_pow(10, len - t - 1)) % 10;

    if(enshow == 0 && t < (len - 1))
    {
      if(temp == 0)
      {
        if(mode & 0x80)
        {
          lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0x01);
        }
        else
        {
          lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0x01);
        }

        continue;
      }
      else
      {
        enshow = 1;
      }

    }

    lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0x01);
  }
}

/**
  * @brief  convert bgr to rgb
  * @param  c: convert bgr value
  * @retval rgb value
  */
uint16_t lcd_bgr2rgb(uint16_t c)
{
  uint16_t  r, g, b, rgb;
  b = (c >> 0) & 0x1f;
  g = (c >> 5) & 0x3f;
  r = (c >> 11) & 0x1f;
  rgb = (b << 11) + (g << 5) + (r << 0);
  return(rgb);
}

