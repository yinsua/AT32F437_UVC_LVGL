/**
  **************************************************************************
  * @file     usbh_video_stream_parsing.c
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video stream parsing
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
 
#include "at32f435_437.h"
#include "usbh_video_stream_parsing.h"
#include "usbh_video_desc_parsing.h"
#include "usbh_video_class.h"


#define UVC_HEADER_SIZE_POS             0
#define UVC_HEADER_BIT_FIELD_POS        1
#define UVC_HEADER_FID_BIT              (1 << 0)
#define UVC_HEADER_EOF_BIT              (1 << 1)
#define UVC_HEADER_SIZE                 12

extern __IO uint8_t tmp_frame_buffer[UVC_RX_FIFO_SIZE];
extern uvc_format_type g_uvc_format;

typedef struct _uvc_data_struct
{
  uint32_t packet_cnt;
  uint32_t data_cnt;
  uint32_t header_cnt;
  uint32_t c_frame_len;
  uint32_t r_frame_len;
  uint8_t prev_fid;
  uint8_t initialized;
  uint8_t new_frame;
  uint8_t switch_ready;
  uint8_t is_enabled;
  uint8_t is_sof;
  uint8_t is_eof;
  uint8_t *buffer0;
  uint8_t *buffer1;
  uint8_t *use_buffer;
  uint8_t *filled_buffer;
}uvc_data_struct;

uvc_data_struct uvc_data;

void video_stream_add_packet_data(uint8_t* buf, uint16_t data_size);
uint8_t uvc_stream_switch_buffers(void);

void uvc_stream_data_process(uint16_t size)
{
	uint8_t m_fid;
	uint16_t data_size;
  
  /* add packet counter */
  uvc_data.packet_cnt++;
  
  if ((size < 2) && (size > UVC_RX_FIFO_SIZE))
    return;
  
  if ((uvc_data.is_enabled == 0) || (uvc_data.initialized == 0))
  {
    uvc_stream_switch_buffers();
    uvc_data.is_eof = 0;
    return;
  }
  
  if (size <= UVC_HEADER_SIZE)
  {
    uvc_data.header_cnt++;
  }
  else if (size > UVC_HEADER_SIZE)
  {
    uvc_data.data_cnt ++;
    
    m_fid = (tmp_frame_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_FID_BIT);
    if ((m_fid != uvc_data.prev_fid) && (uvc_data.is_eof == 1))
    {
      uvc_data.c_frame_len= 0;
      uvc_data.is_sof = 1;
    }
    uvc_data.prev_fid = m_fid;
    
    data_size = size - UVC_HEADER_SIZE;
    video_stream_add_packet_data((uint8_t*)&tmp_frame_buffer[UVC_HEADER_SIZE], data_size);
    
    if (tmp_frame_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT)
    {
      uvc_data.is_eof = 1;
      
      if (uvc_data.is_sof == 0)
      {
        uvc_data.c_frame_len = 0;
        return; 
      }
      
      if (g_uvc_format == UVC_FORMAT_MJPEG)
      {
        uvc_data.is_enabled = 0;
        uvc_stream_switch_buffers();
      }      
    }
    else
    {
      uvc_data.is_eof = 0;
    }
    
    if ((g_uvc_format == UVC_FORMAT_YUY2) && 
        (uvc_data.c_frame_len >= UVC_UNCOMP_FRAME_SIZE))
    {
      if (uvc_data.is_sof == 0)
        return;
      
      uvc_stream_switch_buffers();
    }
  }
}

uint8_t uvc_stream_switch_buffers(void)
{
  if (uvc_data.switch_ready == 1)
  {
    uvc_data.filled_buffer = uvc_data.use_buffer;
    if (uvc_data.use_buffer == uvc_data.buffer0)
      uvc_data.use_buffer = uvc_data.buffer1;
    else
      uvc_data.use_buffer = uvc_data.buffer0;
    
     uvc_data.new_frame = 1;
     uvc_data.switch_ready = 0;
     uvc_data.is_enabled  = 1;
     uvc_data.is_sof = 0;
     uvc_data.r_frame_len = uvc_data.c_frame_len;
     uvc_data.c_frame_len = 0;
     return 1;
  }
  else
  {
    uvc_data.is_enabled  = 0;
  }
  return 0;
}

void video_stream_add_packet_data(uint8_t* buf, uint16_t size)
{
  if ((uvc_data.c_frame_len + size) > UVC_UNCOMP_FRAME_SIZE)
  {
    uvc_data.c_frame_len = UVC_UNCOMP_FRAME_SIZE;
    return;
  }
  memcpy((void*)&uvc_data.use_buffer[uvc_data.c_frame_len], buf, size);
  uvc_data.c_frame_len+= size;
}

void uvc_stream_buffer_update(void)
{
  uvc_data.switch_ready  = 1;
}

void uvc_stream_init(uint8_t* buffer0, uint8_t* buffer1)
{
  if ((buffer0 == NULL) || (buffer1 == NULL))
    return;
  
  uvc_data.buffer0 = buffer0;
  uvc_data.buffer1 = buffer1;
  uvc_data.use_buffer = uvc_data.buffer0;
  uvc_data.filled_buffer = uvc_data.buffer1;
  uvc_data.initialized = 1;
  uvc_data.is_enabled  = 1;
  uvc_data.switch_ready  = 1;
  uvc_data.is_eof = 1;
}



