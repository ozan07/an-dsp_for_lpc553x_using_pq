/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef _APP_H_
#define _APP_H_

#include <stdio.h>

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_common.h"
#include "fsl_clock.h"
//#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "fsl_powerquad.h"
#include "math.h"
//#include "lcd.h"
//#include "lcd_fonts.h"

#include "arm_math.h"
#include "arm_const_structs.h" /* for fft. */

/* for sdcard. */
//#include "fsl_sd.h"
//#include "ff.h"
//#include "diskio.h"
//#include "fsl_sd_disk.h"

//#include "bmp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* TimerCount
 * TickTimer
 * ,systick.
 */
/* Systick Start */
#define TimerCount_Start()	do {\
							SysTick->LOAD  =  0xFFFFFF  ;	/* set reload register */\
							SysTick->VAL  =  0  ;			/* Clear Counter */		 \
							SysTick->CTRL  =  0x5 ;			/* Enable Counting*/	 \
							} while(0)

/* Systick Stop and retrieve CPU Clocks count */
#define TimerCount_Stop(Value) 	do {\
								SysTick->CTRL  =0;	/* Disable Counting */				 \
								Value = SysTick->VAL;/* Load the SysTick Counter Value */\
								Value = 0xFFFFFF - Value;/* Capture Counts in CPU Cycles*/\
								} while(0)

#define TIMER_TICKERS_PER_US   150

#define APP_FFT_LEN_512  512

/* enable to show the computing result values. */
#define APP_CFG_ENABLE_SHOW_OUTPUT_NUMBERS  0 //1

/* enable hardware prescale.
 * when disabled, the same software way is used.
 */
#define APP_CFG_POWERQUAD_ENABLE_HW_PRESCALER  1
                                    
                                    
#define APP_USER_TASK_FFT_128_IDX    0u
#define APP_USER_TASK_FFT_256_IDX    1u
#define APP_USER_TASK_FFT_512_IDX    2u
#define APP_USER_TASK_MAT_ADD_IDX    3u
#define APP_USER_TASK_MAT_INV_IDX    4u
#define APP_USER_TASK_MAT_MUL_IDX    5u
#define APP_USER_TASK_FIR_HPS_IDX    6u
#define APP_USER_TASK_FIR_LPS_IDX    7u
#define APP_USER_TASK_RECORDS_IDX    8u /* only for displaying records. */
#define APP_USER_TASK_COUNT          9u



/* PQ FFT. */
#define APP_PQ_FFT_SAMPLE_COUNT_128   128u
#define APP_PQ_FFT_SAMPLE_COUNT_256   256u
#define APP_PQ_FFT_SAMPLE_COUNT_512   512u
#define APP_PQ_FFT_SAMPLE_COUNT_MAX   APP_PQ_FFT_SAMPLE_COUNT_512

#define APP_PQ_FIR_SAMPLE_COUNT_240   240u

#define APP_PQ_FFT_PERIOD_BASE        32.5f//64u  /* 64 points for a cycle. */


#define APP_CFG_USING_TIMING_CYCLES         0u /* or show the us. */

#if !APP_CFG_USING_TIMING_CYCLES
    #define APP_TIMER_CYCLE_PER_US   (98)
#endif /* APP_CFG_USING_TIMING_CYCLES */                                       
                                    

                                    
/* image address in flexspi flash */
#define IMAGE_WELCOME_ADDRESS       0x08000000
#define IMAGE_MATRIX_ADD_ADDRESS    0x08030000
#define IMAGE_MATRIX_INV_ADDRESS    0x08060000
#define IMAGE_MATRIX_MUL_ADDRESS    0x08090000
#define IMAGE_RESULT_TAB_ADDRESS    0x080C0000                                    
                                    
                                    
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void BOARD_InitHardware(void);

#endif /* _APP_H_ */
