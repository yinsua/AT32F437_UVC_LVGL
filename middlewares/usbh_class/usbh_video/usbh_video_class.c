/**
  **************************************************************************
  * @file     usbh_video_class.c
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video class type
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
#include "usbh_video_class.h"
#include "usbh_video_desc_parsing.h"
#include "usbh_video_stream_parsing.h"

static usb_sts_type uhost_init_handler(void *uhost);
static usb_sts_type uhost_reset_handler(void *uhost);
static usb_sts_type uhost_request_handler(void *uhost);
static usb_sts_type uhost_process_handler(void *uhost);

static usb_sts_type uhost_find_video_stream_in(usbh_core_type *puhost);
static usb_sts_type uvc_cs_request_handle(usbh_core_type *phost);
static usb_sts_type uvc_cs_request(usbh_core_type *puhost , uint8_t feature, uint8_t channel);

usb_sts_type uvc_vs_set_cur(usbh_core_type *puhost, uint16_t request_type);
usb_sts_type uvc_vs_get_cur(usbh_core_type *puhost, uint16_t request_type);

extern uint8_t usbh_cfg_buffer[USB_MAX_DATA_LENGTH]; /*!< usb host cfg buffer */
__IO uint8_t tmp_frame_buffer[UVC_RX_FIFO_SIZE];
uvc_video_info_type video_params;


usbh_uvc_type usbh_uvc;
usbh_class_handler_type uhost_video_class_handler =
{
 uhost_init_handler,
 uhost_reset_handler,
 uhost_request_handler,
 uhost_process_handler,
 &usbh_uvc
};

static usb_sts_type uhost_init_handler(void *uhost)
{
  usbh_core_type *puhost = (usbh_core_type *)uhost;
  usb_sts_type status = USB_OK;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;;
  
  status = uhost_find_video_stream_in(puhost);
  if(status != USB_OK)
  {
    USBH_DEBUG("Unsupport Device!");
    return USB_NOT_SUPPORT;
  }
  
  uvc_parse_descriptor(puhost);
  uvc_parse_format_descriptor(&puvc->class_desc);
  
  if (uvc_format_index == -1)
  {
    return USB_FAIL;
  }
  
  uvc_parse_frame_descriptor(&puvc->class_desc);
  if (uvc_frame_index == -1)
  {
    return USB_FAIL;
  }
  
  if(puvc->intf_stream.supported == 1)
  {       
    puvc->intf_stream.channel = usbh_alloc_channel(puhost, puvc->intf_stream.endp);
    
    
    /* enable channel */
    usbh_hc_open( puhost, 
                  puvc->intf_stream.channel,
                  puvc->intf_stream.endp,
                  puhost->dev.address, 
                  EPT_ISO_TYPE,
                  puvc->intf_stream.max_size,
                  puhost->dev.speed);
    usbh_set_toggle(puhost, puvc->intf_stream.channel, 0);
  }
    
  puvc->req_state     = UVC_REQ_INIT;
  puvc->control_state = UVC_CONTROL_INIT;
  
  status = USB_OK;
  return status;
}


static usb_sts_type uhost_reset_handler(void *uhost)
{
  usb_sts_type status = USB_OK;
  usbh_core_type *puhost = (usbh_core_type *)uhost;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;;
  if(puvc->intf_stream.channel != 0x00)
  {
    usbh_free_channel(puhost, puvc->intf_stream.channel);
    usbh_ch_disable(puhost, puvc->intf_stream.channel);
    puvc->intf_stream.channel = 0;
  }

  return status;
}


static usb_sts_type uhost_request_handler(void *uhost)
{
  usb_sts_type status = USB_WAIT;
  usb_sts_type req_status = USB_OK;
  usbh_core_type *puhost = (usbh_core_type *)uhost;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;;

  switch (puvc->req_state)
  {
  case UVC_REQ_INIT:
  case UVC_REQ_SET_DEFAULT_IN_INTERFACE:
    if(puvc->intf_stream.supported == 1)
    {
      if(puhost->ctrl.state == CONTROL_IDLE)
      {
        usbh_set_interface(puhost, puvc->intf_stream.interface, 0);
      }
      else
      {
        if(usbh_ctrl_result_check(puhost, CONTROL_IDLE, ENUM_IDLE) == USB_OK)
        {
          puvc->req_state = UVC_REQ_SET_CUR;
        }
      }
    }
    else
    {
      puvc->req_state = UVC_REQ_SET_DEFAULT_IN_INTERFACE;   
    }
    break;
    
  case UVC_REQ_CS_REQUEST:
    if(uvc_cs_request_handle(puhost) == USB_OK)
    {
      puvc->req_state = UVC_REQ_SET_IN_INTERFACE;
    }
    break;
   
  case UVC_REQ_SET_IN_INTERFACE:
    if(puvc->intf_stream.supported == 1)
    {
      if(puhost->ctrl.state == CONTROL_IDLE)
      {
        usbh_set_interface(puhost, puvc->intf_stream.interface, puvc->intf_stream.alts);
      }
      else
      {
        if(usbh_ctrl_result_check(puhost, CONTROL_IDLE, ENUM_IDLE) == USB_OK)
        {
          puvc->req_state = UVC_REQ_IDLE;
          puvc->steam_in_state = UVC_STATE_START_IN;
        }
      }
    }
    else
    {
      puvc->req_state = UVC_REQ_SET_IN_INTERFACE;     
    }
    break;
  case UVC_REQ_GET_CUR:
    req_status = uvc_vs_get_cur(puhost, VS_PROBE_CONTROL << 8);
    if(req_status == USB_OK)
    {
        puvc->req_state = UVC_REQ_SET_CUR_COM;
    }
    break;
    case UVC_REQ_SET_CUR:
    req_status = uvc_vs_set_cur(puhost, VS_PROBE_CONTROL << 8);
    if(req_status == USB_OK)
    {

       puvc->req_state = UVC_REQ_GET_CUR;
    }
    break;
    case UVC_REQ_SET_CUR_COM:
      req_status = uvc_vs_set_cur(puhost, VS_COMMIT_CONTROL << 8);
      if(req_status == USB_OK)
      {

         puvc->req_state = UVC_REQ_SET_IN_INTERFACE;
      }
      break;
  case UVC_REQ_IDLE:
    status = USB_OK;    
  default:
    break;
  }
  return status;
}

static usb_sts_type uhost_process_handler(void *uhost)
{
  usb_sts_type status = USB_OK;
  usbh_core_type *puhost = (usbh_core_type *)uhost;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
  volatile uint32_t rxlen = 0;
  urb_sts_type urb_status;
  if(puvc->intf_stream.supported == 1)
  {
    switch(puvc->steam_in_state)
    {
      case UVC_STATE_START_IN:
        usbh_isoc_recv(puhost, puvc->intf_stream.channel,
                            (uint8_t*)tmp_frame_buffer, 
                             puvc->intf_stream.max_size);
        puvc->steam_in_state = UVC_STATE_DATA_IN;
        puvc->intf_stream.timer =  puhost->timer;
        break;
    
    
      case UVC_STATE_DATA_IN:
        urb_status = usbh_get_urb_status(puhost, puvc->intf_stream.channel);
        if(urb_status == URB_DONE && (puhost->timer - puvc->intf_stream.timer >= puvc->intf_stream.poll))
        {
          puvc->intf_stream.timer =  puhost->timer;
          rxlen = puhost->hch[puvc->intf_stream.channel].trans_count;
          uvc_stream_data_process((uint16_t)rxlen);
//          puvc->steam_in_state = UVC_STATE_START_IN;
          usbh_isoc_recv(puhost, puvc->intf_stream.channel,
                            (uint8_t*)tmp_frame_buffer, 
                             puvc->intf_stream.max_size);
        }
//        else if(puhost->timer - puvc->intf_stream.timer >= puvc->intf_stream.poll)
//        {
//          puvc->steam_in_state = UVC_STATE_START_IN;
//        }
      break;
      
      default:
        break;
    }
  }
  return status;
}
 
static usb_sts_type uhost_find_video_stream_in(usbh_core_type *puhost)
{
  uint8_t intfx = 0, eptidx = 0, idx = 0, alt_settings = 0;
  uint16_t size1 = 0, size2 = 0;
  usb_itf_desc_type *usbitf;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
  
  for(intfx = 0; intfx < USBH_MAX_VIDEO_STREAM_IN; intfx ++)
  {
    usbitf = &puhost->dev.cfg_desc.interface[intfx];
    if((usbitf->interface.bInterfaceClass == USB_CLASS_CODE_VIDEO) &&
      (usbitf->interface.bInterfaceSubClass == 0x02) )
    {
      if((usbitf->endpoint[0].bEndpointAddress & 0x80) &&
         (usbitf->endpoint[0].wMaxPacketSize > 0))
      {
        puvc->stream_in[alt_settings].endp = usbitf->endpoint[0].bEndpointAddress;
        puvc->stream_in[alt_settings].max_size = usbitf->endpoint[0].wMaxPacketSize;
        puvc->stream_in[alt_settings].interface = usbitf->interface.bInterfaceNumber;
        puvc->stream_in[alt_settings].alts = usbitf->interface.bAlternateSetting;
        puvc->stream_in[alt_settings].poll = usbitf->endpoint[0].bInterval;
        puvc->stream_in[alt_settings].valid = 1;
        alt_settings ++;
      }
    }
  }
  
  if(alt_settings == 0)
    return USB_NOT_SUPPORT;
  
  for(idx = 0; idx < USBH_MAX_VIDEO_STREAM_IN; idx ++)
  {
    size1 = puvc->stream_in[idx].max_size;
    if(size1 > size2 && size1 < UVC_RX_FIFO_SIZE)
    {
      size2 = size1;
      puvc->intf_stream.endp = puvc->stream_in[idx].endp;
      puvc->intf_stream.max_size = puvc->stream_in[idx].max_size;
      puvc->intf_stream.alts = puvc->stream_in[idx].alts;
      puvc->intf_stream.interface = puvc->stream_in[idx].interface;
      puvc->intf_stream.poll = puvc->stream_in[idx].poll;
      puvc->intf_stream.supported = 1;
      eptidx ++;
    }
  }
  USBH_DEBUG("select iso size %d\r\n", size2);
  if(eptidx == 0)
  {
    return USB_NOT_SUPPORT;
  }
  
  return USB_OK;
}

/**
  * @brief  uvc_cs_request 
  *         The function is responsible for handling AC Specific requests for a specific feature and channel
  *         for Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static usb_sts_type uvc_cs_request(usbh_core_type *puhost , uint8_t feature, uint8_t channel)
{   
  usb_sts_type status = USB_WAIT;

  return status; 
}

/**
  * @brief  uvc_cs_request_handle 
  *         The function is responsible for handling VC Specific requests for a all features 
  *         and associated channels for Video class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static usb_sts_type uvc_cs_request_handle(usbh_core_type *puhost)
{ 

  usb_sts_type status = USB_WAIT;
  usb_sts_type cs_status = USB_WAIT; 
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
        
  cs_status = uvc_cs_request(puhost, 
                                   puvc->temp_feature, 
                                   0);
  
  if(cs_status != USB_WAIT)
  {        
  }
  
  return status;
}



usb_sts_type uvc_vs_set_cur(usbh_core_type *puhost, uint16_t request_type)
{
  uint16_t len = 26;
	usb_sts_type status = USB_WAIT;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
  
  if (request_type == (VS_PROBE_CONTROL << 8))
  {
    memset(&video_params, 0, sizeof(video_params));
    video_params.bmHint = 1;
    video_params.bFormatIndex = uvc_format_index;             
    video_params.bFrameIndex = uvc_frame_index;
    
    video_params.dwFrameInterval = 333333;//30 FPS
    /* ProbeParams.dwFrameInterval = 2000000;//5 FPS */
  }
  
  puhost->ctrl.setup.bmRequestType = USB_DIR_H2D | 
                                     USB_REQ_RECIPIENT_INTERFACE |
                                     USB_REQ_TYPE_CLASS;
  puhost->ctrl.setup.bRequest = UVC_SET_CUR;
  puhost->ctrl.setup.wValue = (uint16_t)request_type;
  puhost->ctrl.setup.wLength = len;
  puhost->ctrl.setup.wIndex = puvc->intf_stream.interface;
  status = usbh_ctrl_request(puhost, (uint8_t *)&video_params, len);
  
  do
  {
    status = usbh_ctrl_result_check(puhost, CONTROL_IDLE, ENUM_IDLE);
  }while(status == USB_WAIT);
  
  return USB_OK;
}

usb_sts_type uvc_vs_get_cur(usbh_core_type *puhost, uint16_t request_type)
{
	uint16_t len = 26;
	usb_sts_type status = USB_WAIT;
  usbh_uvc_type *puvc = (usbh_uvc_type *)puhost->class_handler->pdata;
  memset(&video_params, 0, sizeof(video_params));

  puhost->ctrl.setup.bmRequestType = USB_DIR_D2H | 
                                     USB_REQ_RECIPIENT_INTERFACE |
                                      USB_REQ_TYPE_CLASS;
  puhost->ctrl.setup.bRequest = UVC_GET_CUR;
  puhost->ctrl.setup.wValue = (uint16_t)request_type;
  puhost->ctrl.setup.wLength = len;
  puhost->ctrl.setup.wIndex = puvc->intf_stream.interface;
  status = usbh_ctrl_request(puhost, (uint8_t *)&video_params, len);
  
  do
  {
    status = usbh_ctrl_result_check(puhost, CONTROL_IDLE, ENUM_IDLE);
  }while(status == USB_WAIT);
  
//  if (status == USB_OK)
//  {
//    if (video_params.dwMaxVideoFrameSize > 0)
//    {
//      return USB_OK;
//    }
//    else
//      return USB_FAIL;
//  }
  
  return status;
}
