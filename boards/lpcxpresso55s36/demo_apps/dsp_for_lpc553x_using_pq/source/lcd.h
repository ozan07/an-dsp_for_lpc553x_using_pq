/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _LCD_H_
#define _LCD_H_


#define LCD_WIDTH               240
#define LCD_HEIGHT              320

#define LCD_FONT_1206           12  //Font height of character
#define LCD_FONT_1608           16  //Font height of character 
#define LCD_BYTES_PER_PIXEL     2   //RGB565

#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000     
#define LCD_COLOR_BLUE           0x001F  
#define LCD_COLOR_BRED           0XF81F
#define LCD_COLOR_GRED           0XFFE0
#define LCD_COLOR_GBLUE          0X07FF
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0
#define LCD_COLOR_BROWN          0XBC40 
#define LCD_COLOR_BRRED          0XFC07 
#define LCD_COLOR_GRAY           0X8430 

#define __LCD_DC_CLR()      GPIO_PortClear(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, 1u << BOARD_LCD_DC_GPIO_PIN); 
#define __LCD_DC_SET()      GPIO_PortSet(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, 1u << BOARD_LCD_DC_GPIO_PIN);
    
#define LCD_CMD					0
#define LCD_DATA				1    

void LCD_Init(void);

void LCD_ClearScreen(uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_WrChar(uint16_t x, uint16_t y, uint8_t character, uint8_t chSize, uint16_t color);
void LCD_WrNByteData(uint8_t *pData, uint32_t num);
void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_WrPixelValue(uint16_t pixelVal);
void LCD_WrUnsignedInteger(uint16_t x, uint16_t y, uint32_t integer, uint8_t chSize, uint16_t color);
void LCD_WrString(uint16_t x, uint16_t y, char *pString, uint8_t chSize, uint16_t color);
void LCD_DisplayImage(uint16_t x, uint16_t y, uint16_t imageWidth, uint16_t imageHeight, uint16_t *pImage);

#endif /* #ifndef _LCD_H_ */