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
#include "lv_tick_custom.h"
#include "at32_video_ev_lcd.h"
#include "at32_video_ev_spi.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl.h"

void crm_configuration(void);
void nvic_configuration(void);
void lv_demo_benchmark(void);
void lv_example_style_10(void);
/**
  * @brief  initializ lcd display information
  * @param  none
  * @retval none
  */
void lcd_display_initial_info(void)
{  
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

void my_lvgl_demo()
{
	
  uint32_t i;
	//create style
	lv_style_t style_common;
	lv_style_init(&style_common);
	lv_style_reset(&style_common);
	lv_style_set_radius(&style_common, 10);
	lv_style_set_bg_opa(&style_common, LV_OPA_50);

	//create scr
	lv_obj_t * scr = lv_scr_act();
  lv_obj_remove_style_all(scr);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
	
	//create scene_bg
	lv_obj_t * scene_bg = lv_obj_create(scr);
	lv_obj_remove_style_all(scene_bg);
	lv_obj_set_size(scene_bg, lv_obj_get_width(scr), lv_obj_get_height(scr));
	lv_obj_align(scene_bg, LV_ALIGN_BOTTOM_MID, 0, 0);
	
	lv_obj_update_layout(scr);
	
	lv_obj_t * title = lv_label_create(scr);
    lv_obj_set_pos(title, LV_DPI_DEF / 30, LV_DPI_DEF / 30);
	lv_label_set_text(title, "1234567890");
	
	for(i = 0; i < 10; i++) {
			lv_obj_t * obj = lv_obj_create(scene_bg);
			//lv_obj_remove_style_all(obj);
			lv_obj_add_style(obj, &style_common, 0);
			lv_obj_set_style_bg_color(obj, lv_color_hex(0xbd13204f), 0);
			lv_obj_set_style_border_color(obj, lv_color_hex(0xbd13204f), 0);
			lv_obj_set_style_shadow_color(obj, lv_color_hex(0xbd13204f), 0);

			lv_obj_set_size(obj, 30, 40);

			lv_obj_set_x(obj, 50);
			lv_obj_set_y(obj, -50);
	}
}


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

#if 0
  /* for benchmark test */
  lv_init();
	printf("lv_init finished\r\n");
  lv_port_disp_init();
	printf("lv_port_disp_init finished\r\n");
  lv_demo_benchmark();
	printf("lv_demo_benchmark finished\r\n");
#else  
  /* for touch test */
  //touch_adjust();
  lcd_display_initial_info();
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_example_style_10(); 
#endif  
	printf("init finished\r\n");
	
  while(1)
  {
    lv_task_handler();   
		my_lvgl_demo();
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