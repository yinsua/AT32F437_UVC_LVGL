/**
  **************************************************************************
  * @file     at32_video_ev_touch.h
  * @brief    at32_video_ev_touch header file
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

#ifndef __AT32_VIDEO_EV_TOUCH_H
#define __AT32_VIDEO_EV_TOUCH_H
#include "at32f435_437_board.h"

/* touch panel interface define */
#define  CMD_RDX                         0xD0	  //touch read x coordinate register
#define  CMD_RDY                         0x90   //touch read y coordinate register

#define READ_TIMES                       5
#define LOST_VAL                         1
#define ERR_RANGE                        10
#define TOUCH_OFFSET                     50
#define TOUCH_ADJUST                     1000

struct tp_pix_
{
  uint16_t x;
  uint16_t y;
};
struct tp_pixu32_
{
  uint32_t x;
  uint32_t y;
};

uint8_t tpstate(void);
void touch_adjust(void);
uint8_t touch_precise_coor_read(void);
uint8_t coor_convert(void);
void touch_test(void);
uint16_t touch_read_data(void);

#endif





