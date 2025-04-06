/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_common.h"
#include "fsl_powerquad.h"
#include "lcd.h"
#include "task_lcd_display.h"
#include "app.h"
#include "LPC55S36.h"

#define PQ_MAT_INV_ROW_COL_COUNT_MAX 9u


extern uint8_t  gLcdTextDispBuf[LCD_WIDTH/8u];  /* ʾַ */
extern volatile uint32_t  gPQProcCycles[APP_USER_TASK_COUNT];   /* PowerQuad timing cycles. */


/* A x B = E. */
void task_pq_mat_inv(void)
{
    PRINTF("%s()\r\n", __func__);

    arm_matrix_instance_f32 matrixA;
    arm_matrix_instance_f32 matrixB;
    float32_t mDataA[PQ_MAT_INV_ROW_COL_COUNT_MAX][PQ_MAT_INV_ROW_COL_COUNT_MAX] = {0.0f};
    float32_t mDataB[PQ_MAT_INV_ROW_COL_COUNT_MAX][PQ_MAT_INV_ROW_COL_COUNT_MAX] = {0.0f};
    uint32_t i;
    uint32_t  calcTime;

    //PRINTF("%s()\r\n", __func__);

    /* Initialize the matrix. */
    for (i = 0u; i < PQ_MAT_INV_ROW_COL_COUNT_MAX; i++)
    {
        mDataA[i][i] = 1.0f;
    }
    matrixA.numRows = PQ_MAT_INV_ROW_COL_COUNT_MAX;
    matrixA.numCols = PQ_MAT_INV_ROW_COL_COUNT_MAX;
    matrixA.pData   = (float32_t *)mDataA;
    matrixB.numRows = PQ_MAT_INV_ROW_COL_COUNT_MAX;
    matrixB.numCols = PQ_MAT_INV_ROW_COL_COUNT_MAX;
    matrixB.pData   = (float32_t *)mDataB;

    TimerCount_Start();
    arm_mat_inverse_f32(&matrixA, &matrixB);

    TimerCount_Stop(calcTime);

#if 0
    PRINTF("matrixB.numRows = %8d\r\n", matrixB.numRows);
    PRINTF("matrixB.numCols = %8d\r\n", matrixB.numCols);
    PRINTF("matrixB.pData  ->\r\n");
    for (i = 0u; i < PQ_MAT_INV_ROW_COL_COUNT_MAX; i++)
    {
        for (j = 0u; j < PQ_MAT_INV_ROW_COL_COUNT_MAX; j++)
        {
            PRINTF("%8.2f ", mDataB[i][j]);
        }
        PRINTF("\r\n");
    }
#endif

    /* ˢLCD */
    //lcd_clear_screen(LCD_COLOR_BLACK);
    //uint16_t height, width;

    //memset(gAppBmpDisplayData, 0xFF, sizeof(gAppBmpDisplayData)/sizeof(gAppBmpDisplayData[0]));
    //memset(gAppBmpDisplayData, 0xFF, 2 * LCD_HEIGHT * LCD_WIDTH);
    //App_BmpDumpFromFile("2:\\MInv24b.bmp", &height, &width, gAppBmpDisplayData);
    //lcd_clear_image(gAppBmpDisplayData, height*width*2);
    //LCD_DisplayImage(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)IMAGE_MATRIX_INV_ADDRESS);
    
		LCD_ClearScreen(LCD_COLOR_WHITE);
		
    /* ʾ */
    sprintf((char *)gLcdTextDispBuf, "PowerQuad Matrix Inversion x%d", PQ_MAT_INV_ROW_COL_COUNT_MAX);
    LCD_PrintText(0, gLcdTextDispBuf, LCD_COLOR_BLUE);
#if APP_CFG_USING_TIMING_CYCLES
    sprintf((char *)gLcdTextDispBuf, "Time: %4d cycles.", calcTime);
#else
    sprintf((char *)gLcdTextDispBuf, "Time: %3d us @96MHz.", (calcTime + (APP_TIMER_CYCLE_PER_US-1)) / APP_TIMER_CYCLE_PER_US);
#endif /* APP_CFG_USING_TIMING_CYCLES */
    LCD_PrintText(1, gLcdTextDispBuf, LCD_COLOR_RED);

    PRINTF("%s : %d cycles.\r\n", __func__, calcTime);
    /* Record the cycles into the table. */
    gPQProcCycles[APP_USER_TASK_MAT_INV_IDX] = calcTime;

}
/* EOF. */
