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

/* FIR */
extern float32_t gPQFirF32In[APP_PQ_FIR_SAMPLE_COUNT_240];
extern float32_t gPQFirF32Out[APP_PQ_FIR_SAMPLE_COUNT_240];

extern volatile uint32_t  gPQProcCycles[APP_USER_TASK_COUNT];   /* PowerQuad timing cycles. */

#define FIR_INPUT_LEN 240u
#define TEST_LENGTH_SAMPLES  320

/*2832,NUM_TAPS=+1*/
#define NUM_TAPS              32

#define DEMO_POWERQUAD POWERQUAD
#define EXAMPLE_PRIVATE_RAM ((void *)0xE0001000)

//static float32_t firStateF32[FIR_INPUT_LEN + NUM_TAPS - 1];

/*，6000Hz*/
float32_t firCoeffs32_lowpass[] = {
0.0000000000f, -0.0013361934f, -0.0026212054f, -0.0027337662f, 0.0000000000f, 0.0058338130f, 0.0116127053f, 0.0112869710f,
0.0000000000f, -0.0202965560f, -0.0380763812f, -0.0358973375f, 0.0000000000f, 0.0693732235f, 0.1539442321f, 0.2236157294f,
0.2505895307f, 0.2236157294f, 0.1539442321f, 0.0693732235f, 0.0000000000f, -0.0358973375f, -0.0380763812f, -0.0202965560f,
0.0000000000f, 0.0112869710f, 0.0116127053f, 0.0058338130f, 0.0000000000f, -0.0027337662f, -0.0026212054f, -0.0013361934f, 0.0000000000f,

};

/*，6000Hz*/
float32_t firCoeffs32_highpass[]={
0.0000000000f, 0.0013338703f, 0.0026166481f, 0.0027290131f, 0.0000000000f, -0.0058236702f, -0.0115925151f, -0.0112673471f,
0.0000000000f, 0.0202612678f, 0.0380101804f, 0.0358349253f, 0.0000000000f, -0.0692526091f, -0.1536765800f, -0.2232269444f,
0.7504615446f, -0.2232269444f, -0.1536765800f, -0.0692526091f, 0.0000000000f, 0.0358349253f, 0.0380101804f, 0.0202612678f,
0.0000000000f, -0.0112673471f, -0.0115925151f, -0.0058236702f, 0.0000000000f, 0.0027290131f, 0.0026166481f, 0.0013338703f, 0.0000000000f,
};

/************************************************************************************************/


void task_pq_fir_lowpass(void)
{
    uint32_t i;
    uint32_t Fs=48000;

    //arm_fir_instance_f32 S;
    float32_t  *inputF32, *outputF32;
    uint32_t calcTime;

    inputF32 = &gPQFirF32In[0];
    outputF32 = &gPQFirF32Out[0];

    /* Generate the wave. */
    for (i = 0; i < FIR_INPUT_LEN; i++)
    {
        gPQFirF32In[i]    =arm_sin_f32(2*PI*1000*i/Fs)+ 0.5*arm_sin_f32(2*PI*15000*i/Fs)+1.5;
        gPQFirF32In[i] /=3.0f;
    }

    /*  */
    memset(gLcdWaveformDispBuf, 0u, sizeof(gLcdWaveformDispBuf));

    for (i = 0u; i < LCD_WIDTH; i++)
    {
        gLcdWaveformDispBuf[i] = (uint16_t)(gPQFirF32In[i] * 128.0f);
    }

      /* Call FIR init function to initialize the instance structure. */
    //arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32_lowpass[0], &firStateF32[0], FIR_INPUT_LEN);

    PQ_Init(POWERQUAD);
    pq_config_t pqConfig;

    pqConfig.inputAFormat = kPQ_Float;
    pqConfig.inputAPrescale = 0;
    pqConfig.inputBFormat = kPQ_Float;
    pqConfig.inputBPrescale = 0;
    pqConfig.outputFormat = kPQ_Float;
    pqConfig.outputPrescale = 0;
    pqConfig.tmpFormat = kPQ_Float;
    pqConfig.tmpPrescale = 0;
    pqConfig.machineFormat = kPQ_Float;
    pqConfig.tmpBase = (uint32_t *)0xE0000000;

    PQ_SetConfig(POWERQUAD, &pqConfig);
    /*Private ram*/
    PQ_MatrixScale(POWERQUAD, POWERQUAD_MAKE_MATRIX_LEN(16, NUM_TAPS / 16, 0), 1.0, firCoeffs32_lowpass,
                   EXAMPLE_PRIVATE_RAM);

    PQ_WaitDone(POWERQUAD);

    /* In the next calculation, data in private ram is used. */
    pqConfig.inputBFormat = kPQ_Float;
    pqConfig.outputFormat = kPQ_Float;
    PQ_SetConfig(POWERQUAD, &pqConfig);

    /*  */
    TimerCount_Start();

    PQ_FIR(POWERQUAD, inputF32, APP_PQ_FIR_SAMPLE_COUNT_240, EXAMPLE_PRIVATE_RAM, NUM_TAPS, outputF32,PQ_FIR_FIR);
    PQ_WaitDone(POWERQUAD);

    TimerCount_Stop(calcTime);


    /*  */
    memset(gLcdFreqSpecDispBuf, 0u, sizeof(gLcdFreqSpecDispBuf));
    for (i = 0u; i < APP_PQ_FIR_SAMPLE_COUNT_240; i++)
    {
        gLcdFreqSpecDispBuf[i] = (int)(gPQFirF32Out[i] * 128.0); /* (0,0.5)(0, 128) */
    }

    /* LCD */
    LCD_ClearScreen(LCD_COLOR_WHITE);
    /*  */
    LCD_PrintWaveform(gLcdWaveformDispBuf, APP_PQ_FIR_SAMPLE_COUNT_240, LCD_COLOR_BLUE);
    sprintf((char *)gLcdTextDispBuf, "Input:");
    LCD_PrintWaveformText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    LCD_PrintWaveformFir(gLcdFreqSpecDispBuf, APP_PQ_FIR_SAMPLE_COUNT_240, LCD_COLOR_RED);
    sprintf((char *)gLcdTextDispBuf, "Output:");
    LCD_PrintFreqSpecText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    /*  */
    sprintf((char *)gLcdTextDispBuf, "PowerQuad FIR %d points", APP_PQ_FIR_SAMPLE_COUNT_240);
    LCD_PrintText(0, gLcdTextDispBuf, LCD_COLOR_BLUE);
#if APP_CFG_USING_TIMING_CYCLES
    sprintf((char *)gLcdTextDispBuf, "Time: %4d cycles. lowpass", calcTime);
#else
    sprintf((char *)gLcdTextDispBuf, "Time: %3d us @96MHz. LowPass", (calcTime + (APP_TIMER_CYCLE_PER_US-1)) / APP_TIMER_CYCLE_PER_US);
#endif /* APP_CFG_USING_TIMING_CYCLES */
    LCD_PrintText(1, gLcdTextDispBuf, LCD_COLOR_RED);
    /* Record the cycles into the table. */
    gPQProcCycles[APP_USER_TASK_FIR_LPS_IDX] = calcTime;

}

void task_pq_fir_highpass(void)
{
    uint32_t i;
    uint32_t Fs=48000;

    //arm_fir_instance_f32 S;
    float32_t  *inputF32, *outputF32;
    uint32_t calcTime;

    inputF32 = &gPQFirF32In[0];
    outputF32 = &gPQFirF32Out[0];

    /* Generate the wave. */
    for (i = 0; i < FIR_INPUT_LEN; i++)
    {
        gPQFirF32In[i]    =arm_sin_f32(2*PI*1000*i/Fs)+ 0.5*arm_sin_f32(2*PI*15000*i/Fs)+1.5;
        gPQFirF32In[i] /=3.0f;
    }

    /*  */
    memset(gLcdWaveformDispBuf, 0u, sizeof(gLcdWaveformDispBuf));

    for (i = 0u; i < LCD_WIDTH; i++)
    {
        gLcdWaveformDispBuf[i] = (uint16_t)(gPQFirF32In[i] * 128.0f);
    }

      /* Call FIR init function to initialize the instance structure. */
    //arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32_highpass[0], &firStateF32[0], FIR_INPUT_LEN);

    PQ_Init(POWERQUAD);
    pq_config_t pqConfig;

    pqConfig.inputAFormat = kPQ_Float;
    pqConfig.inputAPrescale = 0;
    pqConfig.inputBFormat = kPQ_Float;
    pqConfig.inputBPrescale = 0;
    pqConfig.outputFormat = kPQ_Float;
    pqConfig.outputPrescale = 0;
    pqConfig.tmpFormat = kPQ_Float;
    pqConfig.tmpPrescale = 0;
    pqConfig.machineFormat = kPQ_Float;
    pqConfig.tmpBase = (uint32_t *)0xE0000000;

    PQ_SetConfig(POWERQUAD, &pqConfig);

    /*Private ram*/
    PQ_MatrixScale(POWERQUAD, POWERQUAD_MAKE_MATRIX_LEN(16, NUM_TAPS / 16, 0), 1.0, firCoeffs32_highpass,
                   EXAMPLE_PRIVATE_RAM);

    PQ_WaitDone(POWERQUAD);

    /* In the next calculation, data in private ram is used. */
    pqConfig.inputBFormat = kPQ_Float;
    pqConfig.outputFormat = kPQ_Float;
    PQ_SetConfig(POWERQUAD, &pqConfig);


    /*  */
    TimerCount_Start();
    PQ_FIR(POWERQUAD, inputF32, APP_PQ_FIR_SAMPLE_COUNT_240, EXAMPLE_PRIVATE_RAM, NUM_TAPS, outputF32,PQ_FIR_FIR);
    PQ_WaitDone(POWERQUAD);
    //arm_fir_f32(&S, inputF32, outputF32, FIR_INPUT_LEN);
    TimerCount_Stop(calcTime);

            /*  */
    memset(gLcdFreqSpecDispBuf, 0u, sizeof(gLcdFreqSpecDispBuf));
    for (i = 0u; i < APP_PQ_FIR_SAMPLE_COUNT_240; i++)
    {
        /*，*/
        gLcdFreqSpecDispBuf[i] = (int)((gPQFirF32Out[i]+0.5) * 128.0); /* (0,0.5)(0, 128) */
    }

    /* LCD */
    LCD_ClearScreen(LCD_COLOR_WHITE);
    /*  */
    LCD_PrintWaveform(gLcdWaveformDispBuf, APP_PQ_FIR_SAMPLE_COUNT_240, LCD_COLOR_BLUE);
    sprintf((char *)gLcdTextDispBuf, "Input:");
    LCD_PrintWaveformText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    LCD_PrintWaveformFir(gLcdFreqSpecDispBuf, APP_PQ_FIR_SAMPLE_COUNT_240, LCD_COLOR_RED);
    sprintf((char *)gLcdTextDispBuf, "Output:");
    LCD_PrintFreqSpecText(gLcdTextDispBuf, LCD_COLOR_BLACK);

    sprintf((char *)gLcdTextDispBuf, "PowerQuad FIR %d points", APP_PQ_FIR_SAMPLE_COUNT_240);
    LCD_PrintText(0, gLcdTextDispBuf, LCD_COLOR_BLUE);
#if APP_CFG_USING_TIMING_CYCLES
    sprintf((char *)gLcdTextDispBuf, "Time: %4d cycles. highpass", calcTime);
#else
    sprintf((char *)gLcdTextDispBuf, "Time: %3d us @96MHz. highpass", (calcTime + (APP_TIMER_CYCLE_PER_US-1)) / APP_TIMER_CYCLE_PER_US);
#endif /* APP_CFG_USING_TIMING_CYCLES */
    LCD_PrintText(1, gLcdTextDispBuf, LCD_COLOR_RED);
    /* Record the cycles into the table. */
    gPQProcCycles[APP_USER_TASK_FIR_HPS_IDX] = calcTime;

}

/* EOF. */

