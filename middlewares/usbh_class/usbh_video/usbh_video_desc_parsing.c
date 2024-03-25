/**
  **************************************************************************
  * @file     usbh_video_desc_parsing.c
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video desc parsing
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjuncstion with Artery microcontrollers. Use of the
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
#include "usb_conf.h"
#include "usbh_core.h"
#include "usbh_ctrl.h"
#include "usbh_video_desc_parsing.h"
#include "usbh_video_class.h"
#include "stdio.h"
#include "string.h"

extern uint8_t usbh_cfg_buffer[USB_MAX_DATA_LENGTH]; /*!< usb host cfg buffer */
int32_t  uvc_format_index = -1;
int32_t  uvc_frame_index = -1;
uvc_format_type g_uvc_format = UVC_FORMAT_MJPEG;
uint32_t g_uvc_width = UVC_TARGET_WIDTH;
uint32_t g_uvc_height = UVC_TARGET_HEIGHT;

uint8_t uvc_find_Interface_index(usbh_core_type *puhost, uint8_t interface_number, uint8_t alt_settings);


uint8_t uvc_find_Interface_index(usbh_core_type *puhost, uint8_t interface_number, uint8_t alt_settings)
{
  usb_interface_desc_type    *pif ;
  uint8_t                     if_ix = 0;
  
  while (if_ix < USBH_MAX_INTERFACE)
  {
    pif = &(puhost->dev.cfg_desc.interface[if_ix].interface);
    if((pif->bInterfaceNumber == interface_number) && (pif->bAlternateSetting == alt_settings))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFF;
}

usb_sts_type uvc_parse_descriptor(usbh_core_type *puhost)
{
  usb_header_desc_type *pdesc;
  uint16_t ptr = 0x9;
  uint8_t itf_index = 0, itf_number = 0, alt_setting;  
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
  
  pdesc   = (usb_header_desc_type *)usbh_cfg_buffer;
  puvc->class_desc.input_terminal = 0;
  puvc->class_desc.out_terminal = 0;  
  puvc->class_desc.alts_num = 0;
  
  while(ptr < puhost->dev.cfg_desc.cfg.wTotalLength)
  {
    pdesc = usbh_get_next_header((uint8_t*) pdesc, &ptr);
    
    switch (pdesc->bDescriptorType)
    {  
      case USB_DESCIPTOR_TYPE_INTERFACE:
        itf_number = *((uint8_t *)pdesc + 2);
        alt_setting = *((uint8_t *)pdesc + 3);
        itf_index = uvc_find_Interface_index(puhost, itf_number, alt_setting);     
      break;

      case USB_DESC_TYPE_CS_INTERFACE:
        if(itf_number <= puhost->dev.cfg_desc.cfg.bNumInterfaces)
        {
        
          uvc_parse_cs_descriptors(&puvc->class_desc,
                           puhost->dev.cfg_desc.interface[itf_index].interface.bInterfaceSubClass, 
                           (uint8_t *)pdesc);
        }
        break;
      
      default:
        break; 
    }
  }
  return USB_OK;
 
}

usb_sts_type uvc_parse_cs_descriptors(uvc_class_spec_desc_type *class_desc, 
                                      uint8_t subclass, 
                                      uint8_t *pdesc)
{
  uint8_t id = pdesc[2];
  uint8_t desc_number = 0;
  if(subclass == USB_SUBCLASS_VIDEO_CONTROL)
  {
    switch(id)
    {
      case UVC_VC_HEADER: 
        class_desc->cs_desc.header = (uvc_header_desc_type *)pdesc;
        break;
        
      case UVC_VC_INPUT_TERMINAL:
        class_desc->cs_desc.input[class_desc->input_terminal++] = (uvc_input_desc_type*) pdesc;    
        break;
        
      case UVC_VC_OUTPUT_TERMINAL:
        class_desc->cs_desc.output[class_desc->out_terminal++] = (uvc_output_desc_type*) pdesc;   
        break;
        
      case UVC_VC_SELECTOR_UNIT:
        class_desc->cs_desc.selector_unit[class_desc->selector_unit++] = (uvc_selector_desc_type*) pdesc; 
        break;    

      default: 
        break;
    }
  }
  else if(subclass == USB_SUBCLASS_VIDEO_STREAMING)
  {
    switch(id)
    {      
      case UVC_VS_INPUT_HEADER:
        if (class_desc->input_header < UVC_MAX_NUM_IN_HEADER)
        {
          class_desc->vs_desc.in_header[class_desc->input_header++] = (uvc_in_header_desc_type*)pdesc;
        }
        break;
        
      case UVC_VS_FORMAT_MJPEG:
        if (class_desc->mjpeg_format < UVC_MAX_MJPEG_FORMAT)
        {
          class_desc->vs_desc.mjpeg_format[class_desc->mjpeg_format++] = (uvc_mjpeg_format_desc_type*)pdesc;
        }
        break;
        
      case UVC_VS_FRAME_MJPEG:
        desc_number = class_desc->mjpeg_frame; 
        
        if (desc_number < UVC_MAX_MJPEG_FRAME_D)
        {
          class_desc->vs_desc.mjpeg_frame[desc_number] = (uvc_mjpeg_frame_desc_type*) pdesc;    
          USBH_DEBUG("mjpeg frame detected: %d x %d", 
                    LE16(class_desc->vs_desc.mjpeg_frame[desc_number]->wWidth), 
                    LE16(class_desc->vs_desc.mjpeg_frame[desc_number]->wHeight));
          class_desc->mjpeg_frame++;
        }
        break;
        
      case UVC_VS_FORMAT_UNCOMPRESSED:
        if (class_desc->uncomp_format < UVC_MAX_UNCOMP_FORMAT)
          class_desc->vs_desc.uncomp_format[class_desc->uncomp_format++] = (uvc_uncomp_format_desc_type*) pdesc; 
        break;
        
      case UVC_VS_FRAME_UNCOMPRESSED:
        desc_number = class_desc->uncomp_frame; 
        
        if (desc_number < UVC_MAX_UNCOMP_FRAME_D)
        {
          class_desc->vs_desc.uncomp_frame[desc_number] = (uvc_uncomp_frame_desc_type*) pdesc;      
          USBH_DEBUG("uncompressed frame detected: %d x %d", 
                    LE16(class_desc->vs_desc.mjpeg_frame[desc_number]->wWidth), 
                    LE16(class_desc->vs_desc.mjpeg_frame[desc_number]->wHeight));
          class_desc->uncomp_frame++;
        }
        break;
        
      default:
        break;
    }
  }
 
  return USB_OK;
}

void uvc_parse_format_descriptor(uvc_class_spec_desc_type *class_desc)
{
  uvc_mjpeg_format_desc_type* mjpeg_format_desc;
  uvc_uncomp_format_desc_type* uncomp_format_desc;
  uvc_format_index = -1;
  
  if (g_uvc_format == UVC_FORMAT_MJPEG)
  {
    if (class_desc->mjpeg_format != 1)
    {
      USBH_DEBUG("not supported mjpeg descriptors number: %d", class_desc->mjpeg_format);
    }
    else
    {
      mjpeg_format_desc = class_desc->vs_desc.mjpeg_format[0];
      uvc_format_index = mjpeg_format_desc->bFormatIndex;
    }
    return;
  }
  else if (g_uvc_format == UVC_FORMAT_YUY2)
  {
    if (class_desc->uncomp_format != 1)
    {
      USBH_DEBUG("not supported uncomp descriptors number: %d", class_desc->uncomp_format);
      return;
    }
    else
    {
      uncomp_format_desc = class_desc->vs_desc.uncomp_format[0];
      if (memcmp(&uncomp_format_desc->guidFormat, "YUY2", 4) != 0)
      {
        USBH_DEBUG("not supported uncomp descriptor type");
        return;
      }
      else
      {
        uvc_format_index = uncomp_format_desc->bFormatIndex;
      }
    }
  }
}

void uvc_parse_frame_descriptor(uvc_class_spec_desc_type *class_desc)
{
  uvc_mjpeg_frame_desc_type* mjpeg_frame_desc;
  uvc_uncomp_frame_desc_type* uncomp_frame_desc;
	uint8_t idx;
  uvc_frame_index = -1;
    
  if (g_uvc_format == UVC_FORMAT_MJPEG)
  {
    for (idx = 0; idx < class_desc->mjpeg_frame; idx++)
    {
      mjpeg_frame_desc = class_desc->vs_desc.mjpeg_frame[idx];
      if ((LE16(mjpeg_frame_desc->wWidth) == g_uvc_width) && \
        (LE16(mjpeg_frame_desc->wHeight) == g_uvc_height))
      {
        uvc_frame_index = mjpeg_frame_desc->bFrameIndex;
      }
    }
  }
  else if (g_uvc_format == UVC_FORMAT_YUY2)
  {
    for (idx = 0; idx < class_desc->uncomp_frame; idx++)
    {
      uncomp_frame_desc = class_desc->vs_desc.uncomp_frame[idx];
      if ((LE16(uncomp_frame_desc->wWidth) == g_uvc_width) && \
        (LE16(uncomp_frame_desc->wHeight) == g_uvc_height))
      {
        uvc_frame_index = uncomp_frame_desc->bFrameIndex;
      }
    }
  }
}
