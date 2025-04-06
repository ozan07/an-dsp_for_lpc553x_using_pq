/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_common.h"
#include "lcd.h"



#define LCD_LINE_WAVEFORM_COUNT  128u
#define LCD_LINE_WAVEFORM_START  8u
#define LCD_LINE_WAVEFORM_END    (LCD_LINE_WAVEFORM_START + LCD_LINE_WAVEFORM_COUNT)

#define LCD_LINE_FREQSPEC_COUNT  128u
#define LCD_LINE_FREQSPEC_START  144u
#define LCD_LINE_FREQSPEC_END    (LCD_LINE_FREQSPEC_START + LCD_LINE_FREQSPEC_COUNT)

#define LCD_LINE_TEXT_COUNT      32u
#define LCD_LINE_TEXT_START      288u
#define LCD_LINE_TEXT_END        (LCD_LINE_TEXT_START+LCD_LINE_TEXT_COUNT-1)


uint16_t gLcdWaveformDispBuf[LCD_WIDTH];    //for time domain waveform
uint16_t gLcdFreqSpecDispBuf[LCD_WIDTH];    //for frequency domain
uint8_t  gLcdTextDispBuf[LCD_WIDTH/8u];     //display characters

void lcd_draw_line(		uint16_t hwXpos0, //specify x0 position.
                      uint16_t hwYpos0, //specify y0 position.
                      uint16_t hwXpos1, //specify x1 position.
                      uint16_t hwYpos1, //specify y1 position.
                      uint16_t hwColor) //specify the color of the line
{
		int x = hwXpos1 - hwXpos0;
    int y = hwYpos1 - hwYpos0;
    int dx = abs(x), sx = hwXpos0 < hwXpos1 ? 1 : -1;
    int dy = -abs(y), sy = hwYpos0 < hwYpos1 ? 1 : -1;
    int err = dx + dy, e2;
    
    for (;;){
        LCD_DrawPoint(hwXpos0, hwYpos0 , hwColor);
        e2 = 2 * err;
        if (e2 >= dy) {     
            if (hwXpos0 == hwXpos1) break;
            err += dy; hwXpos0 += sx;
        }
        if (e2 <= dx) {
            if (hwYpos0 == hwYpos1) break;
            err += dx; hwYpos0 += sy;
        }
    }
}

void LCD_PrintWaveform(uint16_t *pData, uint16_t len, uint16_t color)
{
    uint16_t x, y , yz1;
	
    len = (len > LCD_WIDTH) ? LCD_WIDTH : len;
    for(x = 1; x < len; x++)
    {
        y = (pData[x] > LCD_LINE_WAVEFORM_COUNT)?LCD_LINE_WAVEFORM_COUNT : pData[x];
        y = LCD_LINE_WAVEFORM_END - y;

			  yz1 = (pData[x-1] > LCD_LINE_WAVEFORM_COUNT)?LCD_LINE_WAVEFORM_COUNT : pData[x-1];
        yz1 = LCD_LINE_WAVEFORM_END - yz1;
			
          lcd_draw_line(x-1,yz1,x,y,color);
					//LCD_DrawPoint(x, y, color);  

    }
}


void LCD_PrintWaveformFir(uint16_t *pData, uint16_t len, uint16_t color)
{
    uint16_t x, y , yz1;
    len = (len > LCD_WIDTH)?LCD_WIDTH:len;
  /*  
    for(x = 0; x < len; x++)
    {
        y = (pData[x] > LCD_LINE_FREQSPEC_COUNT)?LCD_LINE_FREQSPEC_COUNT:pData[x];
        y = LCD_LINE_FREQSPEC_END - y;
        
        for(uint16_t temp_y = y; temp_y <= LCD_LINE_FREQSPEC_END; temp_y++)
        {
             LCD_DrawPoint(x, temp_y, color);         
        }
    }

*/
		
		
		    for(x = 1; x < len; x++)
    {
        y = (pData[x] > LCD_LINE_FREQSPEC_COUNT)?LCD_LINE_FREQSPEC_COUNT : pData[x];
        y = LCD_LINE_FREQSPEC_END - y;

			  yz1 = (pData[x-1] > LCD_LINE_FREQSPEC_COUNT)?LCD_LINE_FREQSPEC_COUNT : pData[x-1];
        yz1 = LCD_LINE_FREQSPEC_END - yz1;
			
          lcd_draw_line(x-1,yz1,x,y,color);
					//LCD_DrawPoint(x, y, color);  

    }



		
}

void LCD_PrintFreqSpec(uint16_t *pData, uint16_t len, uint16_t color)
{
    uint16_t x;
    uint16_t y, temp_y;
    
    len = (len > LCD_WIDTH)?LCD_WIDTH:len;
    for(x = 0; x < len; x++)
    {
        y = (pData[x] > LCD_LINE_FREQSPEC_COUNT) ? LCD_LINE_FREQSPEC_COUNT : pData[x];
        y = LCD_LINE_FREQSPEC_END - y;

        /* ӡһ */
        for (temp_y = y; temp_y <= LCD_LINE_FREQSPEC_END; temp_y++)
        {
            LCD_DrawPoint(x, temp_y, color);
        }
    }
}

void LCD_PrintText(uint8_t line_idx, uint8_t *pBuf, uint16_t color)
{
    uint16_t y = LCD_LINE_TEXT_START;
    uint16_t x = 0;
    
    line_idx = (line_idx > 1) ? 1 : line_idx;
    y += line_idx * 16;
    
    while(x < (LCD_WIDTH - 8))
    {
        LCD_WrChar(x, y, *pBuf++, LCD_FONT_1608, color);
        if(*pBuf == '\0')
        {
            break;
        }
        else if((*pBuf == '\r') || (*pBuf == '\n'))
        {
            break;
        }
        else
        {
            x += 8;
        }
    }
}


void LCD_PrintWaveformText(uint8_t *pChar, uint16_t color)
{
    uint16_t y = LCD_LINE_WAVEFORM_START;
    uint16_t x = 0u;
    
    while (x < (LCD_WIDTH-8u) )
    {
        LCD_WrChar(x, y, *pChar++, LCD_FONT_1206, color);
        if (*pChar == '\0')
        {
            break;
        }
        else if ( (*pChar == '\r') || (*pChar == '\n') )
        {
            break;
        }
        else
        {
            x += 8u;
        }
    }
    
}


void LCD_PrintFreqSpecText(uint8_t *pString, uint16_t color)
{
    uint16_t x = 0;
    uint16_t y = LCD_LINE_FREQSPEC_START;
    
    while (x < (LCD_WIDTH-8u) )
    {
        LCD_WrChar(x, y, *pString++, LCD_FONT_1206, color);
        if (*pString == '\0')
        {
            break;
        }
        else if ( (*pString == '\r') || (*pString == '\n') )
        {
            break;
        }
        else
        {
            x += 8u;
        }
    }
}


