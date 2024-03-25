/**
  **************************************************************************
  * @file     usbh_video_desc_parsing.h
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video desc header file
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
#ifndef __USBH_VIDEO_DESC_PARSING_H
#define __USBH_VIDEO_DESC_PARSING_H

#include "usbh_video_class.h"

usb_sts_type uvc_parse_descriptor(usbh_core_type *puhost);
usb_sts_type uvc_parse_cs_descriptors(uvc_class_spec_desc_type *class_desc, 
                                      uint8_t ac_subclass, 
                                      uint8_t *pdesc);

void uvc_parse_format_descriptor(uvc_class_spec_desc_type *class_desc);
void uvc_parse_frame_descriptor(uvc_class_spec_desc_type *class_desc);

extern uvc_format_type g_uvc_format;
extern int uvc_format_index;
extern int uvc_frame_index;

#endif

