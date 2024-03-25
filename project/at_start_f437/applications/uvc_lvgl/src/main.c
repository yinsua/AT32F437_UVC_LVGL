/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.0
  * @date     
  * @brief    main program
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

#include "at32f435_437_clock.h"
#include "at32f435_437_board.h"


void crm_configuration(void);
void nvic_configuration(void);

/* ------------------------ UVC Camera ----------------------*/
#include "usb_conf.h"
#include "usb_core.h"
#include "usbh_int.h"
#include "usbh_user.h"
#include "usbh_video_class.h"
#include "usbh_video_stream_parsing.h"

/* usb global struct define */
otg_core_type otg_core_struct;
void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);
void button_exint_init(void);
void button_isr(void);
uint8_t buffer0[UVC_MAX_FRAME_SIZE];
uint8_t buffer1[UVC_MAX_FRAME_SIZE];

/**
  * @brief  configure button exint
  * @param  none
  * @retval none
  */
void button_exint_init(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
  * @brief  exint0 interrupt handler
  * @param  none
  * @retval none
  */
void EXINT0_IRQHandler(void)
{
  delay_ms(50);
  exint_flag_clear(EXINT_LINE_0);
  if(USBH_CLASS == otg_core_struct.host.global_state)
  {
    otg_core_struct.host.global_state = USBH_SUSPEND;
    USBH_DEBUG("Host Start Enter Suspend Mode");
  }
  else if(USBH_SUSPENDED == otg_core_struct.host.global_state)
  {
    otg_core_struct.host.global_state = USBH_WAKEUP;
    USBH_DEBUG("Host Start Wakeup");
  }
}

/* ----------------------------------------------------------*/


/* ------------------------ LVGL ----------------------------*/
#include "lv_tick_custom.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl.h"

void lv_demo_benchmark(void);
void lv_example_style_10(void);
/* ----------------------------------------------------------*/


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */

int main(void)
{
  system_clock_config();
  nvic_configuration();
  delay_init();
  
  uart_print_init(115200);
  printf("usart init ok\r\n");
  
  /* for littlevgl gui tick increase */  
  tmr7_int_init(191, 999);
 
  crm_configuration();
  lcd_init();
  //lcd_clear(RED);

#if 1
  /* for benchmark test */
  lv_init();
	printf("lv_init finished\r\n");
  lv_port_disp_init();
	printf("lv_port_disp_init finished\r\n");
  lv_demo_benchmark();
	printf("lv_demo_benchmark finished\r\n");
#else  

  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_example_style_10(); 
#endif  
	printf("init finished\r\n");
	
  while(1)
  {
    lv_task_handler();
  }
}


/**
  * @brief  crm configuration
  * @param  none
  * @retval none
  */
void crm_configuration(void)
{
  crm_periph_clock_enable(LCD_SPI_MASTER_DMA_CLK, TRUE);
  crm_periph_clock_enable(LCD_SPI_SELECTED_CLK, TRUE);
  //crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(LCD_PEN_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_RST_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_BLK_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_DC_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_CS1_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_CS2_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_SPI_SCK_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_SPI_MOSI_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_SPI_MISO_GPIO_CLK, TRUE);  
}


/**
  * @brief  nvic configuration
  * @param  none
  * @retval none
  */
void nvic_configuration(void)
{
  /* 2 bit for pre-emption priority,2 bits for subpriority */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* configure dma1 channel3 for the spi transmit */
  nvic_irq_enable(LCD_SPI_MASTER_Tx_DMA_IRQn, 0, 2);
}


/******************************** USB Function ***********************************************/
/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);

    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      /* 216MHz */
      case 216000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4_5);
        break;

      /* 240MHz */
      case 240000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5);
        break;

      /* 264MHz */
      case 264000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5_5);
        break;

      /* 288MHz */
      case 288000000:
        crm_usb_clock_div_set(CRM_USB_DIV_6);
        break;

      default:
        break;

    }
  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  /* dp and dm */
  gpio_init_struct.gpio_pins = OTG_PIN_DP | OTG_PIN_DM;
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DP_SOURCE, OTG_PIN_MUX);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DM_SOURCE, OTG_PIN_MUX);

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif

#ifdef USBH_5V_POWER_SWITCH
  crm_periph_clock_enable(OTG_PIN_POWER_SWITCH_CLOCK, TRUE);
  gpio_bits_set(OTG_PIN_POWER_SWITCH_GPIO, OTG_PIN_POWER_SWITCH);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_pins = OTG_PIN_POWER_SWITCH;
  gpio_init(OTG_PIN_POWER_SWITCH_GPIO, &gpio_init_struct);
#endif


}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

#endif

/**
  * @brief  this function handles otgfs interrupt.
  * @param  none
  * @retval none
  */
void OTG_IRQ_HANDLER(void)
{
  usbh_irq_handler(&otg_core_struct);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  delay_us(us);
}