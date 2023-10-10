/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "fsl_common.h"
#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "lcd_hx8347d.h"
#include "lcd_fonts.h"
#include "lcd.h"


/*******************************************************************************
 * LCD & SPI description
 ******************************************************************************/
/*
 * LCD pixel data format is : color_h[7:0], color_l[7:0]
 * High 8-bit of one pixel MUST be sent frist.
 * 
 * SPI, when passing a start address of memory to SPI, it will send the data stored
 * at low address first.
 *
 * When pass a 16-bit pixel data to SPI, SPI will send low 8-bit first, then high
 * 8-bit.
 */



/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LCD_SPI               Driver_SPI8
#define BOARD_LCD_SPI_BAUDRATE      50000000U /*! Transfer baudrate */


#define BOARD_LCD_DC_GPIO           GPIO /*! LCD data/command base */
#define BOARD_LCD_DC_GPIO_PORT      1U   /*! LCD data/command port */
#define BOARD_LCD_DC_GPIO_PIN       28U   /*! LCD data/command pin */
#define BOARD_LCD_BKL_GPIO_PORT     0U   /*! LCD back light port, active high */
#define BOARD_LCD_BKL_GPIO_PIN      12U   /*! LCD back light pin */
#define BOARD_LCD_CS_GPIO_PORT      1U   /*! LCD cs port, active high */
#define BOARD_LCD_CS_GPIO_PIN       26U   /*! LCD cs pin */



#define LCD_SPI_MASTER              SPI8
#define LCD_SPI_MASTER_IRQ          FLEXCOMM8_IRQn
#define LCD_SPI_MASTER_CLK_SRC      kCLOCK_Hs_Lspi
#define LCD_SPI_MASTER_CLK_FREQ     CLOCK_GetHsLspiClkFreq()
#define LCD_SPI_SSEL                3
#define LCD_SPI_SPOL                kSPI_SpolActiveAllLow



/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/

void LCD_InitPins(void)
{
    /* Define the init structure for the data/command output pin */
    gpio_pin_config_t pin_config = {
        kGPIO_DigitalOutput,
        1,
    };
    
    /* Enables the clock for the GPIO1 module */
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    
    
    /* Init data/command GPIO output . */
    GPIO_PortInit(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT);
    GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, BOARD_LCD_DC_GPIO_PIN, &pin_config);
    
    /* BKL init */
    //GPIO_PortInit(BOARD_LCD_DC_GPIO, BOARD_LCD_BKL_GPIO_PORT);
    GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_BKL_GPIO_PORT, BOARD_LCD_BKL_GPIO_PIN, &pin_config);

    

    
    
    /* CS pin init */
    //GPIO_PortInit(BOARD_LCD_DC_GPIO, BOARD_LCD_CS_GPIO_PORT);
    //GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_CS_GPIO_PORT, BOARD_LCD_CS_GPIO_PIN, &pin_config);
}


void  LCD_WrCmd(uint8_t cmd)
{
    spi_transfer_t xfer            = {0};
    GPIO_PortClear(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, 1u << BOARD_LCD_DC_GPIO_PIN);
    /*Start Transfer*/
    xfer.txData      = &cmd;
    xfer.rxData      = destBuff;
    xfer.dataSize    = 1;
    xfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferBlocking(LCD_SPI_MASTER, &xfer);
}

void LCD_WrData(uint8_t data)
{
    spi_transfer_t xfer            = {0};
    GPIO_PortSet(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, 1u << BOARD_LCD_DC_GPIO_PIN);
    /*Start Transfer*/
    xfer.txData      = &data;
    xfer.rxData      = destBuff;
    xfer.dataSize    = 1;
    xfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferBlocking(LCD_SPI_MASTER, &xfer);
}

void LCD_WrNByteData(uint8_t *pData, uint32_t num)
{
    assert(num <= BUFFER_SIZE);  //data number bigger than buffer size
    
    spi_transfer_t xfer            = {0};
    GPIO_PortSet(BOARD_LCD_DC_GPIO, BOARD_LCD_DC_GPIO_PORT, 1u << BOARD_LCD_DC_GPIO_PIN);
    /*Start Transfer*/
    xfer.txData      = pData;
    xfer.rxData      = destBuff;
    xfer.dataSize    = num;
    xfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferBlocking(LCD_SPI_MASTER, &xfer); 
}




void LCD_Init(void)
{
    spi_master_config_t userConfig = {0};
    uint32_t srcFreq               = 0;
    
    /* Attach main clock to High speed SPI */
    CLOCK_AttachClk(kMAIN_CLK_to_HSLSPI);
    SPI8_InitPins();
    
    /*
     * userConfig.enableLoopback = false;
     * userConfig.enableMaster = true;
     * userConfig.polarity = kSPI_ClockPolarityActiveHigh;
     * userConfig.phase = kSPI_ClockPhaseFirstEdge;
     * userConfig.direction = kSPI_MsbFirst;
     * userConfig.baudRate_Bps = 500000U;
     */
    SPI_MasterGetDefaultConfig(&userConfig);
    srcFreq            = LCD_SPI_MASTER_CLK_FREQ;
    userConfig.sselNum = (spi_ssel_t)LCD_SPI_SSEL;
    userConfig.sselPol = (spi_spol_t)LCD_SPI_SPOL;
    userConfig.baudRate_Bps = 10000000;
    SPI_MasterInit(LCD_SPI_MASTER, &userConfig, srcFreq);
    
    LCD_InitPins();
    LCD_Hx8347Init(LCD_WrData, LCD_WrCmd);
    
    
    /* lcd display test */
    for (uint32_t i = 0; i < BUFFER_SIZE; i=i+2)
    {
        srcBuff[i] = 0x07;
        srcBuff[i+1] = 0xE0;
    }
    
    for(uint32_t i = 0; i < 2400; i++)
    {
        LCD_WrNByteData(srcBuff, 64);
    }
}


void LCD_SetCursor(uint16_t x, uint16_t y)
{
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    
    LCD_WrCmd(0x02);    //Column address upper byte
    LCD_WrData(x>>8);
    LCD_WrCmd(0x03);    //Column address low byte
    LCD_WrData(x&0xFF);
    
    LCD_WrCmd(0x06);    //Row address upper byte
    LCD_WrData(y>>8);
    LCD_WrCmd(0x07);    //Row address low byte
    LCD_WrData(y&0xFF);
    
    LCD_WrCmd(0x22);    //memory write
}

/* Set the start address and end address for an area */
void LCD_SetAreaAddr(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    assert(x_start < LCD_WIDTH);
    assert(y_start < LCD_HEIGHT);
    assert(x_end < LCD_WIDTH);
    assert(y_end < LCD_HEIGHT);
    /* Whether need to check *_start must less than *_end ?? */
    
    LCD_WrCmd(0x02);    //Column Start 
    LCD_WrData(x_start>>8);
    LCD_WrCmd(0x03);    
    LCD_WrData(x_start&0xFF);
    LCD_WrCmd(0x04);    //Column End
    LCD_WrData(x_end>>8);
    LCD_WrCmd(0x05);    
    LCD_WrData(x_end&0xFF);
    LCD_WrCmd(0x06);    //Row Start
    LCD_WrData(y_start>>8);
    LCD_WrCmd(0x07);    
    LCD_WrData(y_start&0xFF);
    LCD_WrCmd(0x08);    //Row End
    LCD_WrData(y_end>>8);
    LCD_WrCmd(0x09);    
    LCD_WrData(y_end&0xFF);
    
    LCD_WrCmd(0x22);    //memory write
}

//void LCD_WrPixel(uint16_t *pPixelVal, uint32_t num)
//{
//    uint16_t pixelVal, tempPixel;
//    
//    for(uint32_t i = 0; i < num; i++)
//    {
//        pixelVal = *pPixelVal;
//        tempPixel = (pixelVal >> 8)|(pixelVal << 8);
//        
//    }
//    
//    
//}

/* color format: RGB565, [15:0]=[G2G1G0B4B3B2B1B0,R4R3R2R1R0G5G4G3] 
 * SPI bit shift direction is: MSB first,
 * SPI will send low 8-bit of one pixel value firstly, and then send hihg 8-bit of pixel value
 * Bit stream on SPI MOSI is: |R4->R3->R2->R1->R0->G5->G4->G3|-->|G2->G1->G0->B4->B3->B2->B1->B0| 
 */
void LCD_ClearScreen(uint16_t color)
{
    //LCD_SetAreaAddr(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
    //uint16_t tempColor = (color >> 8)|(color << 8);
    LCD_SetCursor(0, 0);
    
    
    for(uint32_t i = 0; i < LCD_WIDTH*LCD_HEIGHT; i++)
    {
        LCD_WrNByteData((uint8_t*)&color, 2);
    } 
}


void LCD_WrPixelValue(uint16_t pixelVal)
{
    uint16_t temp = (pixelVal >> 8) | (pixelVal << 8);
    LCD_WrNByteData((uint8_t *)&temp, 2); 
}

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    //uint16_t tempColor = (color >> 8)|(color << 8);
    LCD_SetCursor(x, y);
    LCD_WrNByteData((uint8_t *)&color, 2);  
}



void LCD_WrChar(uint16_t x, uint16_t y, uint8_t character, uint8_t chSize, uint16_t color)
{
    uint8_t tempData = 0;
    uint16_t pixelXAddr = x, pixelYAddr = y;
    
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    
    for(uint32_t i = 0; i < chSize; i++)
    {
         if(chSize == LCD_FONT_1206)
         {
             tempData = c_chFont1206[character-0x20][i];
         }
         else if(chSize ==  LCD_FONT_1608)
         {
             tempData = c_chFont1608[character-0x20][i];
         }
         else
         {
             PRINTF("Not support font\r\n");
         }
         
         for(uint32_t j = 0; j < 8; j++)
         {
             if(tempData & 0x80)
             {
                    LCD_DrawPoint(pixelXAddr, pixelYAddr, color);
             }
             tempData <<= 1;
             pixelYAddr++;
             if((pixelYAddr - y) == chSize)
             {
                 pixelYAddr = y;
                 pixelXAddr++;
                 break;
             }  
         }
    }
}

void LCD_WrUnsignedInteger(uint16_t x, uint16_t y, uint32_t integer, uint8_t chSize, uint16_t color)
{
    uint32_t factor = 1000000000;
    
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    
    for(uint32_t i = 0; i < 9; i++)
    {
        uint32_t res = integer/factor;
        integer = integer % factor;
        factor = factor / 10;
        if(res != 0)
        {
            LCD_WrChar(x+(chSize/2)*i, y, res+'0', chSize, color);    
        }
    } 
}

void LCD_WrString(uint16_t x, uint16_t y, char *pString, uint8_t chSize, uint16_t color)
{
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    
    while(*pString != '\0')
    {
        if(x > (LCD_WIDTH - chSize/2))
        {
            x = 0;
            y += chSize;    //return
        }
        if(y > (LCD_HEIGHT - chSize))
        {
            x = y = 0;
            LCD_ClearScreen(0x00);
        }
        
        LCD_WrChar(x, y, *pString++, chSize, color);
        x += chSize/2;         
    }  
}

void LCD_DisplayImage(uint16_t x, uint16_t y, uint16_t imageWidth, uint16_t imageHeight, uint16_t *pImage)
{
    
    LCD_SetAreaAddr(x, y, x+imageWidth-1, y+imageHeight-1);
    for(uint32_t i = 0; i < imageWidth*imageHeight; i++)
    {
        LCD_WrPixelValue(*pImage++);
    }
        
}
