/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme file
  **************************************************************************
 
  @Description  
  This demo is based on the AT-START-F437 board £¬AT32-Video-Ev V1.x and 2.4'TFT_Lcd 240*320 RGB TOUCH . 
  In this demo, 
  for bench mark test:
  lcd will show the weighted FPS and Opa.speed...
  
  for touch test:
  first you  need to adjust the touch screen, and then enter the touch test phase, where
  the lcd will draw a special transition when going to pressed state.

  ------------------------Lcd 9341----------------------------------------
  ---------------AT32F437----Link to-----Lcd 9341-------------------------
  SPI1: link to 9341 SPI
  PA5  ----------------->Pin3(LCD_SPI_SCK)
  PA6  ----------------->Pin8(LCD_SPI_MISO)
  PA7  ----------------->Pin4(LCD_SPI_MOSI)
  PA15 ----------------->Pin5(RST)
  PC7  ----------------->Pin6(D/CX)
  PA9  ----------------->Pin7(BLK)
  CSX(Chip Select) to GND
  ----------------------------------------------------------------------


1. comment at32_video_ev_spi.c line 40: gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);
2. comment main.c line 134: crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);   








	
/****************** (C) COPYRIGHT 2023 ArteryTek *********END OF FILE*********/