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

/*  */
extern uint16_t gLcdWaveformDispBuf[LCD_WIDTH]; /* . */
extern uint16_t gLcdFreqSpecDispBuf[LCD_WIDTH]; /* . */
extern uint8_t  gLcdTextDispBuf[LCD_WIDTH/8u];  /*  */

/* FFT */
extern q31_t     gPQFftQ31In[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
extern q31_t     gPQFftQ31Out[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
extern q31_t     gPQFftQ31InOut[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
extern float32_t gPQFftF32In[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
extern float32_t gPQFftF32Out[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];

extern volatile uint32_t  gPQProcCycles[APP_USER_TASK_COUNT];   /* PowerQuad timing cycles. */


void task_pq_fft_512(void)
{
    PRINTF("%s\r\n", __func__);

    arm_cfft_instance_q31 instance;
    uint32_t i;
    uint32_t calcTime;

    /* . */
    for (i = 0; i < APP_PQ_FFT_SAMPLE_COUNT_512; i++)
    {
        /*  */
        gPQFftF32In[i*2] = 1.5f /* , 0 */
                         + 1.0f * arm_cos_f32( (       2.0f * PI / APP_PQ_FFT_PERIOD_BASE) * i  ) /*  */
                         + 0.5f * arm_cos_f32( (4.0f * 2.0f * PI / APP_PQ_FFT_PERIOD_BASE) * i  ) /*  */
                         ;
        gPQFftF32In[i*2] /= 3.0f; /* (0, 1). \. */
        /*  */
        gPQFftF32In[i*2+1] = 0.0f;

    }

    /*  */
    memset(gLcdWaveformDispBuf, 0u, sizeof(gLcdWaveformDispBuf));
    for (i = 0u; (i < APP_PQ_FFT_SAMPLE_COUNT_512) && (i < LCD_WIDTH); i++)
    {
        gLcdWaveformDispBuf[i] = (uint16_t)(gPQFftF32In[i*2] * 128.0f);
    }

    /* PowerquadFFT */
    arm_float_to_q31(gPQFftF32In, gPQFftQ31In, APP_PQ_FFT_SAMPLE_COUNT_512*2u);

    /* cfft., */
    for (i = 0u; i < APP_PQ_FFT_SAMPLE_COUNT_512 * 2u; i++)
    {
        gPQFftQ31InOut[i] = gPQFftQ31In[i] >> 5u; /* powerquadfft27bit,, */
    }

    /*  */
    instance.fftLen = APP_PQ_FFT_SAMPLE_COUNT_512;/* FFT() */
    TimerCount_Start();
    arm_cfft_q31(&instance, gPQFftQ31InOut, 0, 1); /*  */
    //PQ_TransformCFFT(POWERQUAD, APP_PQ_FFT_SAMPLE_COUNT_512, gPQFftQ31InOut, gPQFftQ31InOut);
    
    TimerCount_Stop(calcTime);

    /*  */
    for (i = 0u; i < APP_PQ_FFT_SAMPLE_COUNT_512 * 2u; i++)
    {
        gPQFftQ31Out[i] = gPQFftQ31InOut[i] << 5u;
    }

    /*  */
    arm_q31_to_float(gPQFftQ31Out, gPQFftF32Out, APP_PQ_FFT_SAMPLE_COUNT_512*2u);

    /* , */
    arm_cmplx_mag_f32( gPQFftF32Out, gPQFftF32In, APP_PQ_FFT_SAMPLE_COUNT_512);/* gPQFftF32In */

    /*  */
    memset(gLcdFreqSpecDispBuf, 0u, sizeof(gLcdFreqSpecDispBuf));
    for (i = 0u; i < APP_PQ_FFT_SAMPLE_COUNT_512 && (i < LCD_WIDTH); i++)
    {
        gLcdFreqSpecDispBuf[i] = (int)(gPQFftF32In[i] * 128.0); /* (0,0.5)(0, 128) */
    }

    /* LCD */
    LCD_ClearScreen(LCD_COLOR_WHITE);

    /*  */
    LCD_PrintWaveform(gLcdWaveformDispBuf, APP_PQ_FFT_SAMPLE_COUNT_512, LCD_COLOR_BLUE);
    sprintf((char *)gLcdTextDispBuf, "Input:");
    LCD_PrintWaveformText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    /*  */
    LCD_PrintFreqSpec(gLcdFreqSpecDispBuf, APP_PQ_FFT_SAMPLE_COUNT_512, LCD_COLOR_RED);
    sprintf((char *)gLcdTextDispBuf, "Output:");
    LCD_PrintFreqSpecText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    /*  */
    sprintf((char *)gLcdTextDispBuf, "PowerQuad FFT %d points", APP_PQ_FFT_SAMPLE_COUNT_512);
    LCD_PrintText(0, gLcdTextDispBuf, LCD_COLOR_BLUE);
#if APP_CFG_USING_TIMING_CYCLES
    sprintf((char *)gLcdTextDispBuf, "Time: %4d cycles.", calcTime);
#else
    sprintf((char *)gLcdTextDispBuf, "Time: %3d us @96MHz.", (calcTime + (APP_TIMER_CYCLE_PER_US-1)) / APP_TIMER_CYCLE_PER_US);
#endif /* APP_CFG_USING_TIMING_CYCLES */
    LCD_PrintText(1, gLcdTextDispBuf, LCD_COLOR_RED);

    PRINTF("%s : %d cycles.\r\n", __func__, calcTime);
    /* Record the cycles into the table. */
    gPQProcCycles[APP_USER_TASK_FFT_512_IDX] = calcTime;
}

/* EOF. */





