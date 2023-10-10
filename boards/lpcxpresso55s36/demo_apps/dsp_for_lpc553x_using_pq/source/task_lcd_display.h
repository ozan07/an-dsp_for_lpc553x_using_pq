/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef _TASK_LCD_DISPLAY_H_
#define _TASK_LCD_DISPLAY_H_


void LCD_PrintWaveform(uint16_t *pData, uint16_t len, uint16_t color);
void LCD_PrintWaveformFir(uint16_t *pData, uint16_t len, uint16_t color);
void LCD_PrintText(uint8_t line_idx, uint8_t *pBuf, uint16_t color);
void LCD_PrintFreqSpec(uint16_t *pData, uint16_t len, uint16_t color);
void LCD_PrintWaveformText(uint8_t *pChar, uint16_t color);
void LCD_PrintFreqSpecText(uint8_t *pString, uint16_t color);

#endif /* _TASK_LCD_DISPLAY_H_ */