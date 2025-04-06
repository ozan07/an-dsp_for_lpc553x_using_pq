/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fsl_common.h"
#include "app.h"
#include "lcd.h"
#include "fsl_flexspi_nor_flash.h"
#include "fsl_powerquad.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FlexSpiInstance           0U
#define EXAMPLE_FLEXSPI_AMBA_BASE FlexSPI0_AMBA_BASE
#define FLASH_SIZE                0x02000000UL /* 32MBytes */
#define FLASH_PAGE_SIZE           256UL        /* 256Bytes */
#define FLASH_SECTOR_SIZE         0x1000UL     /* 4KBytes */
#define FLASH_BLOCK_SIZE          0x10000UL    /* 64KBytes */
#define BUFFER_LEN FLASH_PAGE_SIZE

     
     
//#define IMAGE_WELCOME_ADDR        0x08000000    /* address of flexspi flash for welcome image */  


  
     
   
     
     
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief config serial NOR option  */
static serial_nor_config_option_t option = {
    .option0.U = 0xc0000001U,
    .option1.U = 0U,
};

/*! @brief FLEXSPI NOR flash driver Structure */
static flexspi_nor_config_t norConfig;




/* ̬ڴ */
q31_t     gPQFftQ31In[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
q31_t     gPQFftQ31Out[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
q31_t     gPQFftQ31InOut[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
float32_t gPQFftF32In[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];
float32_t gPQFftF32Out[APP_PQ_FFT_SAMPLE_COUNT_MAX*2u];

float32_t gPQFirF32In[APP_PQ_FIR_SAMPLE_COUNT_240*2u];
float32_t gPQFirF32Out[APP_PQ_FIR_SAMPLE_COUNT_240*2u];

volatile uint32_t  gPQProcCycles[APP_USER_TASK_COUNT];   /* PowerQuad timing cycles. */
volatile uint32_t  gARMProcCycles[APP_USER_TASK_COUNT];  /* ARM CMSIS DSP timing cycles. */ 

extern volatile int32_t menu_index;            //menu index
extern volatile uint32_t menu_update_flag;     //indicates menu changed

/*******************************************************************************
 * Prototypes
 ******************************************************************************/



/*******************************************************************************
 * Code
 ******************************************************************************/
/*
void APP_LcdSayHello(void)
{
    uint16_t *pImage = (uint16_t *)IMAGE_WELCOME_ADDR;
    LCD_SetCursor(0, 0);
    for(uint32_t i = 0; i < LCD_WIDTH*LCD_HEIGHT; i++)
    {
        LCD_WrPixelValue(*pImage++);
    }
}
*/
void task_pq_fft_128(void);
void task_pq_fft_256(void);
void task_pq_fft_512(void);
void task_pq_mat_add(void);
void task_pq_mat_inv(void);
void task_pq_mat_mul(void);
void task_pq_fir_lowpass(void);
void task_pq_fir_highpass(void);
void task_pq_records(void);

void APP_InitUserKey(void);

/* Function list on LCD */
void (*cAppLcdDisplayPageFunc[])(void) =
{
    task_pq_fft_128,
    task_pq_fft_256,
    task_pq_fft_512,
    task_pq_mat_add,
    task_pq_mat_inv,
    task_pq_mat_mul,
    task_pq_fir_lowpass,
    task_pq_fir_highpass,
    task_pq_records
};

/*!
 * @brief Main function
 */
uint8_t tempBuffer[64];
int main(void)
{
    status_t status;
    //uint32_t serialNorAddress; /* Address of the serial nor device location */
    //uint32_t AHBNorAddress;    /* Access the serial nor flash via AHB bus */
    
    uint32_t serialNorTotalSize;
    uint32_t serialNorSectorSize;
    uint32_t serialNorPageSize;
    
    
    BOARD_InitHardware();
    APP_InitUserKey();

    PRINTF("\r\nARM CMSIS-DSP vs PowerQuad FFT.\r\n\r\n");

    /* Clean up FLEXSPI NOR flash driver Structure */
    memset(&norConfig, 0U, sizeof(flexspi_nor_config_t));
    /* Setup FLEXSPI NOR Configuration Block */
    status = FLEXSPI_NorFlash_GetConfig(FlexSpiInstance, &norConfig, &option);
    if(status != kStatus_Success)
    {
        PRINTF("Get FLEXSPI NOR configuration block failure\r\n");
    }
    /* Initializes the FLEXSPI module for the other FLEXSPI APIs */
    status = FLEXSPI_NorFlash_Init(FlexSpiInstance, &norConfig);
    if (status == kStatus_Success)
    {
        PRINTF("\r\n Successfully init FLEXSPI serial NOR flash\r\n ");
    }
    else
    {
        PRINTF("\r\n Erase sector failure !\r\n");
    }
    
    serialNorTotalSize  = norConfig.memConfig.sflashA1Size;
    serialNorSectorSize = norConfig.sectorSize;
    serialNorPageSize   = norConfig.pageSize;
    
    /* Print serial NOR flash information */
    PRINTF("\r\n Serial NOR flash Information: ");
    PRINTF("\r\n Serial NOR flash size:\t%d KB, Hex: (0x%x)", (serialNorTotalSize / 1024U), serialNorTotalSize);
    PRINTF("\r\n Serial NOR flash sector size:\t%d KB, Hex: (0x%x) ", (serialNorSectorSize / 1024U),
           serialNorSectorSize);
    PRINTF("\r\n Serial NOR flash page size:\t%d B, Hex: (0x%x)\r\n", serialNorPageSize, serialNorPageSize);
    
    PQ_Init(POWERQUAD);
    
    LCD_Init();
    
    
    //LCD_ClearScreen(0xE007);

    //LCD_WrChar(0, 100, 'Y', LCD_FONT_1608, 0x001F);
    
    //uint16_t *pData = (uint16_t *)0x08000000;



    
    
    //APP_LcdSayHello();
    
   while(1)
   {
       if(menu_update_flag)
       {
           menu_update_flag = 0;
           cAppLcdDisplayPageFunc[menu_index]();
       }
   }
    
}

/* EOF. */

