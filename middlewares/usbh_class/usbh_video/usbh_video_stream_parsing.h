/**
  **************************************************************************
  * @file     usbh_video_stream_parsing.h
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video stream header file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_VIDEO_STREAM_PARSING_H
#define __USBH_VIDEO_STREAM_PARSING_H

#include "usbh_video_class.h"

void uvc_stream_data_process(uint16_t size);
void uvc_stream_init(uint8_t* buffer0, uint8_t* buffer1);
void uvc_stream_buffer_update(void);


#endif
