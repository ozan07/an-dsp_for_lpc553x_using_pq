/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "app.h"
#include "lcd.h"


extern uint8_t  gLcdTextDispBuf[LCD_WIDTH/8u];
extern uint32_t  gPQProcCycles[8];                              //--3
extern uint32_t  gARMProcCycles[8];                             //--5

void task_pq_records(void)
{
    uint16_t height, width;
    uint32_t line;

    /* fir taps run in ram, optimization level is high. */
    gARMProcCycles[APP_USER_TASK_FFT_128_IDX] = 14171;
    gARMProcCycles[APP_USER_TASK_FFT_256_IDX] = 25682;
    gARMProcCycles[APP_USER_TASK_FFT_512_IDX] = 69782;
    gARMProcCycles[APP_USER_TASK_MAT_ADD_IDX] = 2062;
    gARMProcCycles[APP_USER_TASK_MAT_INV_IDX] = 17731;
    gARMProcCycles[APP_USER_TASK_MAT_MUL_IDX] = 28965;
    gARMProcCycles[APP_USER_TASK_FIR_HPS_IDX] = 20651;
    gARMProcCycles[APP_USER_TASK_FIR_LPS_IDX] = 20651;

    //App_BmpDumpFromFile("2:\\Tab24b.bmp", &height, &width, gAppBmpDisplayData);
    //lcd_clear_image(gAppBmpDisplayData, height*width*2);
    LCD_DisplayImage(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)IMAGE_RESULT_TAB_ADDRESS);

    for (line = 0u; line < 8u; line++)
    {
        height = 113 + line * 24;

        /* pq cycles. */
        width  = 82;
        sprintf((char *)gLcdTextDispBuf, "%d", gPQProcCycles[line]);
        LCD_WrString(width, height, (char *)gLcdTextDispBuf, LCD_FONT_1206, LCD_COLOR_RED);

        /* pq us. */
        width += 45;
        sprintf((char *)gLcdTextDispBuf, "%d", (gPQProcCycles[line]+APP_TIMER_CYCLE_PER_US-1)/APP_TIMER_CYCLE_PER_US);
        LCD_WrString(width, height, (char *)gLcdTextDispBuf, LCD_FONT_1206, LCD_COLOR_RED);

        /* arm cycles. */
        width += 35;
        sprintf((char *)gLcdTextDispBuf, "%d", gARMProcCycles[line]);
        LCD_WrString(width, height, (char *)gLcdTextDispBuf, LCD_FONT_1206, LCD_COLOR_BLUE);

        /* arm us. */
        width += 45;
        sprintf((char *)gLcdTextDispBuf, "%d", (gARMProcCycles[line]+APP_TIMER_CYCLE_PER_US-1)/APP_TIMER_CYCLE_PER_US);
        LCD_WrString(width, height, (char *)gLcdTextDispBuf, LCD_FONT_1206, LCD_COLOR_BLUE);

    }


}

/* EOF. */
