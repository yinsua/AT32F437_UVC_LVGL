/**
  **************************************************************************
  * @file     usbh_video_class.h
  * @version  v2.0.9
  * @date     2022-06-28
  * @brief    usb host video class header file
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
#ifndef __USBH_VIDEO_CLASS_H
#define __USBH_VIDEO_CLASS_H
#include "usb_std.h"
#include "string.h"
#include "usbh_core.h"

#define USBH_MAX_VIDEO_STREAM_IN          5
#define UVC_RX_FIFO_SIZE                  1023

//#define UVC_TARGET_WIDTH                640
//#define UVC_TARGET_HEIGHT               480

#define UVC_TARGET_WIDTH                128
#define UVC_TARGET_HEIGHT               160
   
#define UVC_CAPTURE_MODE                UVC_FORMAT_MJPEG
//#define UVC_CAPTURE_MODE                UVC_FORMAT_YUY2
   
// Uncompressed image frame size in byte
#define UVC_UNCOMP_FRAME_SIZE           (UVC_TARGET_WIDTH * UVC_TARGET_HEIGHT)
//#define UVC_UNCOMP_FRAME_SIZE           (160 * 120)
   
#define UVC_MAX_FRAME_SIZE              UVC_UNCOMP_FRAME_SIZE

#define USB_SUBCLASS_VIDEO_CONTROL	                      0x01
#define USB_SUBCLASS_VIDEO_STREAMING	                    0x02
#define USB_SUBCLASS_VIDEO_INTERFACE_COLLECTION           0x03

#define USB_DESC_TYPE_CS_INTERFACE      0x24
#define USB_DESC_TYPE_CS_ENDPOINT       0x25

#define UVC_VC_HEADER			              0x01
#define UVC_VC_INPUT_TERMINAL		        0x02
#define UVC_VC_OUTPUT_TERMINAL		      0x03
#define UVC_VC_SELECTOR_UNIT		        0x04
#define UVC_VC_PROCESSING_UNIT		      0x05
#define UVC_VC_EXTENSION_UNIT		        0x06

#define UVC_VS_UNDEFINED                0x00
#define UVC_VS_INPUT_HEADER             0x01
#define UVC_VS_OUTPUT_HEADER            0x02
#define UVC_VS_STILL_IMAGE_FRAME        0x03
#define UVC_VS_FORMAT_UNCOMPRESSED      0x04
#define UVC_VS_FRAME_UNCOMPRESSED       0x05
#define UVC_VS_FORMAT_MJPEG             0x06
#define UVC_VS_FRAME_MJPEG              0x07
#define UVC_VS_FORMAT_MPEG2TS           0x0A
#define UVC_VS_FORMAT_DV                0x0C
#define UVC_VS_COLORFORMAT              0x0D
#define UVC_VS_FORMAT_FRAME_BASED       0x10
#define UVC_VS_FRAME_FRAME_BASED        0x11
#define UVC_VS_FORMAT_STREAM_BASED      0x12

#define UVC_AS_GENERAL			0x01
#define UVC_FORMAT_TYPE			0x02
#define UVC_FORMAT_SPECIFIC	0x03
#define UVC_EP_GENERAL			0x01
#define UVC_SET_CUR			    0x01
#define UVC_GET_CUR			    0x81
#define UVC_SET_MIN			    0x02
#define UVC_GET_MIN			    0x82
#define UVC_SET_MAX			    0x03
#define UVC_GET_MAX			    0x83
#define UVC_SET_RES			    0x04
#define UVC_GET_RES			    0x84
#define UVC_SET_MEM			    0x05
#define UVC_GET_MEM			    0x85

#define UVC_GET_STAT			    0xff

#define UVC_TERMINAL_UNDEFINED		0x100
#define UVC_TERMINAL_STREAMING		0x101
#define UVC_TERMINAL_VENDOR_SPEC	0x1FF

#define UVC_MAX_NUM_IN_TERMINAL          10
#define UVC_MAX_NUM_OUT_TERMINAL         4
#define UVC_MAX_NUM_FEATURE_UNIT         2
#define UVC_MAX_NUM_SELECTOR_UNIT        2
#define UVC_MAX_NUM_IN_HEADER            3

#define UVC_MAX_MJPEG_FORMAT             3
#define UVC_MAX_MJPEG_FRAME_D            10

#define UVC_MAX_UNCOMP_FORMAT            3
#define UVC_MAX_UNCOMP_FRAME_D           10

#define UVC_MAX_SAMFREQ_NBR              5
#define UVC_MAX_INTERFACE_NBR            5
#define UVC_MAX_CONTROLS_NBR             5

#define VS_PROBE_CONTROL                 0x01
#define VS_COMMIT_CONTROL                0x02

/**
  * @brief uvc support format
  */
typedef enum
{
  UVC_FORMAT_MJPEG = 0,
  UVC_FORMAT_YUY2,
} uvc_format_type;

/**
  * @brief states of the uvc state
  */
//typedef enum
//{
//  UVC_INIT = 0, 
//  UVC_IDLE,   
//  UVC_CS_REQUEST,
//  UVC_SET_FEATURE_UNIT,
//  UVC_SET_INTERFACE,
//  UVC_SET_STREAMING_INTERFACE,
////  UVC_SET_CUR,
////  UVC_GET_RES,
//  UVC_GET_CUR1,
//  UVC_SET_CUR2,
//  UVC_GET_CUR2,
//  UVC_SET_CUR3,
//  UVC_SET_INTERFACE0,
//  UVC_SET_INTERFACE1,
//  UVC_SET_INTERFACE2,
//  UVC_ISOC_OUT,        
//  UVC_ISOC_IN,  
//  UVC_ISOC_POLL,
//  UVC_ERROR,  
//} uvc_state_type;

/**
  * @brief states of the uvc request
  */
typedef enum
{
  UVC_REQ_INIT = 1,
  UVC_REQ_IDLE, 
  UVC_REQ_SET_DEFAULT_IN_INTERFACE,
  UVC_REQ_SET_IN_INTERFACE,
  UVC_REQ_CS_REQUEST,
  UVC_REQ_GET_CUR,
  UVC_REQ_SET_CUR,
  UVC_REQ_SET_CUR_COM,
}uvc_req_state_type;

/**
  * @brief states of the uvc control
  */
typedef enum
{
 UVC_CONTROL_INIT = 1,
 UVC_CONTROL_CHANGE,
 UVC_CONTROL_IDLE, 
}uvc_ctrl_state_type;

/**
  * @brief states of the uvc streaming
  */
typedef enum
{
  UVC_STATE_IDLE = 1,
  UVC_STATE_START_IN,
  UVC_STATE_DATA_IN,
}uvc_stream_state_type;


/**
  * @brief class-specific vc header descriptor
  */
typedef struct
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bcdUVC[2];
  uint8_t  wTotalLength[2];
  uint8_t  dwClockFrequency[4];
  uint8_t  bInCollection;
  uint8_t  baInterfaceNr[UVC_MAX_INTERFACE_NBR];
} uvc_header_desc_type;

/**
  * @brief vc input terminal descriptor
  */
typedef struct 
{
  uint8_t  bLength;           
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bTerminalID;
  uint8_t  wTerminalType[2];
  uint8_t  bAssocTerminal;
  uint8_t  iTerminal;
  uint8_t  wObjectiveFocalLengthMin[2];
  uint8_t  wObjectiveFocalLengthMax[2];
  uint8_t  wOcularFocalLength[2];
  uint8_t  bControlSize;
  uint8_t  bmControls[3];
} uvc_input_desc_type;

/**
  * @brief vc output terminal descriptor
  */
typedef struct 
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;     
  uint8_t  bTerminalID;
  uint8_t  wTerminalType[2];
  uint8_t  bAssocTerminal;
  uint8_t  bSourceID;
  uint8_t  iTerminal;
} uvc_output_desc_type;

/**
  * @brief feature descriptor
  */
typedef struct
{                                                             
  uint8_t  bLength;                                        
  uint8_t  bDescriptorType;                                
  uint8_t  bDescriptorSubtype;                             
  uint8_t  bUnitID;                                        
  uint8_t  bSourceID;                                      
  uint8_t  bControlSize;                                   
  uint8_t  bmaControls[UVC_MAX_CONTROLS_NBR][2];                                                                 
} uvc_feature_desc_type;

/**
  * @brief selector descriptor
  */
typedef struct
{                                                             
  uint8_t  bLength;                                        
  uint8_t  bDescriptorType;                                
  uint8_t  bDescriptorSubtype;                             
  uint8_t  bUnitID;                                        
  uint8_t  bNrInPins; 
  uint8_t  bSourceID0;  
  uint8_t  iSelector;                                                                   
} uvc_selector_desc_type;

/**
  * @brief vs input header descriptor
  */
typedef struct 
{
  uint8_t  bLength;           
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bNumFormats;
  uint8_t  wTotalLength[2];
  uint8_t  bEndPointAddress;
  uint8_t  bmInfo;
  uint8_t  bTerminalLink;
  uint8_t  bStillCaptureMethod;
  uint8_t  bTriggerSupport;
  uint8_t  bTriggerUsage;
  uint8_t  bControlSize;
  uint8_t  bmaControls;
} uvc_in_header_desc_type;

/**
  * @brief vs mjpeg format descriptor
  */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatIndex;
  uint8_t bNumFrameDescriptors;
  uint8_t bmFlags;
  uint8_t bDefaultFrameIndex;
  uint8_t bAspectRatioX;
  uint8_t bAspectRatioY;
  uint8_t bmInterlaceFlags;
  uint8_t bCopyProtect;
} uvc_mjpeg_format_desc_type;

/**
  * @brief vs mjpeg frame descriptor
  */
typedef struct 
{
  uint8_t  bLength;           
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
  uint8_t  bFrameIndex;
  uint8_t  bmCapabilities;
  uint8_t  wWidth[2];
  uint8_t  wHeight[2];
  uint8_t  dwMinBitRate[4];
  uint8_t  dwMaxBitRate[4];
  uint8_t  dwDefaultFrameInterval[4];
  uint8_t  bFrameIntervalType;
  /* dwFrameInterval*N is here */
} uvc_mjpeg_frame_desc_type;

/**
  * @brief vs uncompressed format descriptor
  */
typedef struct 
{
  uint8_t  bLength;           
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;//must be UVC_VS_INPUT_HEADER
  uint8_t  bFormatIndex;
  uint8_t  bNumFrameDescriptors;
  uint8_t  guidFormat[16];
  uint8_t  bBitsPerPixel;
  uint8_t  bDefaultFrameIndex;
  uint8_t  bAspectRatioX;
  uint8_t  bAspectRatioY;
  uint8_t  bmInterfaceFlags;
  uint8_t  bCopyProtect;
} uvc_uncomp_format_desc_type;

/**
  * @brief vs uncompressed frame descriptor
  */
typedef struct 
{
  uint8_t  bLength;           
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;//must be UVC_VS_INPUT_HEADER
  uint8_t  bFrameIndex;
  uint8_t  bmCapabilities;
  uint8_t  wWidth[2];
  uint8_t  wHeight[2];
  uint8_t  dwMinBitRate[4];
  uint8_t  dwMaxBitRate[4];
  uint8_t  dwDefaultFrameInterval[4];
  uint8_t  bFrameIntervalType;
  /* dwFrameInterval*N is here */
} uvc_uncomp_frame_desc_type;


/**
  * @brief uvc streaming struct type
  */
typedef struct
{
  uint8_t              endp;
  uint16_t             max_size;
  uint8_t              alts;
  uint8_t              interface;
  uint8_t              valid; 
  uint16_t             poll;
}uvc_streaming_in_type;

/**
  * @brief uvc interface streaming struct type
  */
typedef struct
{
  uint8_t              endp;
  uint16_t             max_size; 
  uint8_t              interface; 
  uint8_t              alts;
  uint8_t              supported;    

  uint8_t              channel;  
  uint8_t              poll; 
  uint32_t             timer ; 
  
  uint8_t              asociated_as;
  
  uint8_t              *buf;
  uint8_t              *cbuf; 
  uint32_t             partial_ptr; 

  uint32_t             global_ptr;  
  uint16_t             frame_length;  
  uint32_t             total_length;  
}uvc_intf_stream_type;


/**
  * @brief class-specific video control interface descriptor
  */
typedef struct
{
  uvc_header_desc_type       *header;
  uvc_input_desc_type        *input[UVC_MAX_NUM_IN_TERMINAL];
  uvc_output_desc_type       *output[UVC_MAX_NUM_OUT_TERMINAL];
  uvc_feature_desc_type      *feature_unit[UVC_MAX_NUM_FEATURE_UNIT];
  uvc_selector_desc_type     *selector_unit[UVC_MAX_NUM_SELECTOR_UNIT];
}uvc_vc_desc_type;

/**
  * @brief class-specific video streaming interface descriptor
  */
typedef struct
{
  uvc_in_header_desc_type     *in_header[UVC_MAX_NUM_IN_HEADER];
  
  uvc_mjpeg_format_desc_type  *mjpeg_format[UVC_MAX_MJPEG_FORMAT];
  uvc_mjpeg_frame_desc_type   *mjpeg_frame[UVC_MAX_MJPEG_FRAME_D];
  
  uvc_uncomp_format_desc_type *uncomp_format[UVC_MAX_UNCOMP_FORMAT];
  uvc_uncomp_frame_desc_type  *uncomp_frame[UVC_MAX_UNCOMP_FRAME_D];
  
} uvc_vs_desc_type;


/**
  * @brief class-specific video descriptor
  */
typedef struct
{
  uvc_vc_desc_type   cs_desc;
  uvc_vs_desc_type   vs_desc;

  uint16_t alts_num;
  uint16_t input_terminal;
  uint16_t out_terminal;
  uint16_t selector_unit;
  
  uint8_t input_header;
  
  uint8_t mjpeg_format;
  uint8_t mjpeg_frame;
  
  uint8_t uncomp_format;
  uint8_t uncomp_frame;
}uvc_class_spec_desc_type;

typedef __packed struct 
{
  uint16_t    bmHint;                      
  uint8_t     bFormatIndex;                
  uint8_t     bFrameIndex;                
  uint32_t    dwFrameInterval;             
  uint16_t    wKeyFrameRate;               
  uint16_t    wPFrameRate;                 
  uint16_t    wCompQuality;                
  uint16_t    wCompWindowSize;             
  uint16_t    wDelay;                      
  uint32_t    dwMaxVideoFrameSize; 
  uint32_t    dwMaxPayloadTransferSize;
  uint32_t    dwClockFrequency;
  uint8_t     bmFramingInfo;
  uint8_t     bPreferedVersion;
  uint8_t     bMinVersion;
  uint8_t     bMaxVersion;
} uvc_video_info_type;


typedef struct
{
  uvc_req_state_type                 req_state;  
  uvc_ctrl_state_type                control_state; 
  uvc_stream_state_type              steam_in_state;
  
  uvc_streaming_in_type              stream_in[USBH_MAX_VIDEO_STREAM_IN];
  uvc_class_spec_desc_type           class_desc;
  
  uvc_intf_stream_type               intf_stream;//camera;
  uint16_t                           mem[8];  
  uint8_t                            temp_feature;
   __IO uint16_t                     timer; 
  
}usbh_uvc_type;

//typedef struct _format_payload_header
//{
//  uint8_t bHeaderLength;
//  uint8_t bmHeaderInfo;
//  
//}payload_header;

#define  LE16(addr)             (((uint16_t)(*((uint8_t *)(addr))))\
                                + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))



extern usbh_class_handler_type uhost_video_class_handler;
#endif

