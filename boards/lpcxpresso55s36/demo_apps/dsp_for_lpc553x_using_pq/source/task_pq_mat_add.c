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


#define PQ_MAT_ROW_COUNT_MAX 16u
#define PQ_MAT_COL_COUNT_MAX 16u

extern uint8_t  gLcdTextDispBuf[LCD_WIDTH/8u];  /*  */
extern volatile uint32_t  gPQProcCycles[APP_USER_TASK_COUNT];   /* PowerQuad timing cycles. */

/* A + B = C. */
void task_pq_mat_add(void)
{
    PRINTF("%s()\r\n", __func__);

    arm_matrix_instance_f32 matrixA;
    arm_matrix_instance_f32 matrixB;
    arm_matrix_instance_f32 matrixC;
    float32_t mDataA[PQ_MAT_ROW_COUNT_MAX][PQ_MAT_COL_COUNT_MAX];
    float32_t mDataB[PQ_MAT_ROW_COUNT_MAX][PQ_MAT_COL_COUNT_MAX];
    float32_t mDataC[PQ_MAT_ROW_COUNT_MAX][PQ_MAT_COL_COUNT_MAX];
    uint32_t i, j;
    uint32_t  calcTime;

    /* Initialize the matrix. */
    for (i = 0u; i < PQ_MAT_ROW_COUNT_MAX; i++)
    {
        for (j = 0u; j < PQ_MAT_COL_COUNT_MAX; j++)
        {
            mDataA[i][j] = 1.0f * i * PQ_MAT_ROW_COUNT_MAX + j;
            mDataB[i][j] = 1.0f * i * PQ_MAT_ROW_COUNT_MAX + j;
        }
    }
    matrixA.numRows = PQ_MAT_ROW_COUNT_MAX;
    matrixA.numCols = PQ_MAT_COL_COUNT_MAX;
    matrixA.pData   = (float32_t *)mDataA;
    matrixB.numRows = PQ_MAT_ROW_COUNT_MAX;
    matrixB.numCols = PQ_MAT_COL_COUNT_MAX;
    matrixB.pData   = (float32_t *)mDataB;
    matrixC.numRows = PQ_MAT_ROW_COUNT_MAX;
    matrixC.numCols = PQ_MAT_COL_COUNT_MAX;
    matrixC.pData   = (float32_t *)mDataC;

    /* Calc & Measure. */
    //uint32_t length = POWERQUAD_MAKE_MATRIX_LEN(matrixA.numRows, matrixA.numCols, matrixB.numCols);
    TimerCount_Start();
    arm_mat_add_f32(&matrixA, &matrixB, &matrixC);
    //PQ_MatrixAddition(POWERQUAD, length, &matrixA, &matrixB, &matrixC);
    TimerCount_Stop(calcTime);

    /* LCD */
    //lcd_clear_screen(LCD_COLOR_BLACK);

    //uint16_t height, width;

    //memset(gAppBmpDisplayData, 0xFF, sizeof(gAppBmpDisplayData)/sizeof(gAppBmpDisplayData[0]));
    //App_BmpDumpFromFile("2:\\MAdd24b.bmp", &height, &width, gAppBmpDisplayData);
    //LCD_ClearImage(gAppBmpDisplayData, height*width*2);
    LCD_DisplayImage(0, 0, 240, 320, (uint16_t *)IMAGE_MATRIX_ADD_ADDRESS);


    /*  */
    sprintf((char *)gLcdTextDispBuf, "PowerQuad Matrix Addition x%d", PQ_MAT_ROW_COUNT_MAX);
    LCD_PrintText(0, gLcdTextDispBuf, LCD_COLOR_BLUE);
#if APP_CFG_USING_TIMING_CYCLES
    sprintf((char *)gLcdTextDispBuf, "Time: %4d cycles.", calcTime);
#else
    sprintf((char *)gLcdTextDispBuf, "Time: %3d us @96MHz.", (calcTime + (APP_TIMER_CYCLE_PER_US-1)) / APP_TIMER_CYCLE_PER_US);
#endif /* APP_CFG_USING_TIMING_CYCLES */
    LCD_PrintText(1, gLcdTextDispBuf, LCD_COLOR_RED);

    PRINTF("%s : %d cycles.\r\n", __func__, calcTime);
    /* Record the cycles into the table. */
    gPQProcCycles[APP_USER_TASK_MAT_ADD_IDX] = calcTime;

}
/* EOF. */

