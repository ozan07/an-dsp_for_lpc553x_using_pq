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
#define BOARD_LCD_SPI_BAUDRATE      40000000U /*! Transfer baudrate */


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

#define FONT_1206				12
#define FONT_1608				16
	
#define WHITE						0xFFFF
#define BLACK						0x0000	  
#define BLUE						0x001F  
#define BRED						0XF81F
#define GRED						0XFFE0
#define GBLUE						0X07FF
#define RED							0xF800
#define MAGENTA					0xF81F
#define GREEN						0x07E0
#define CYAN						0x7FFF
#define YELLOW					0xFFE0
#define BROWN						0XBC40 
#define BRRED						0XFC07 
#define GRAY						0X8430

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];

void  LCD_WrCmd(uint8_t cmd);
void lcd_write_byte(uint8_t chByte, uint8_t chCmd);
static void lcd_write_register(uint8_t chRegister, uint8_t chValue);


uint8_t lcd_id;
uint8_t _rotation = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
uint8_t lcd_read_id(void)
{
	uint8_t reg = 0xDC;
	uint8_t tx_val = 0x00;
	uint8_t rx_val;
	//__LCD_CS_CLR();
	//__LCD_DC_CLR();
	LCD_WrCmd(reg);//HAL_SPI_Transmit(&hspi1,&reg,1,0xff);
	LCD_WrCmd(tx_val);//HAL_SPI_TransmitReceive(&hspi1,&tx_val,&rx_val,1,0xff);
	LCD_WrCmd(tx_val);//HAL_SPI_TransmitReceive(&hspi1,&tx_val,&rx_val,1,0xff);
	LCD_WrCmd(tx_val);//HAL_SPI_TransmitReceive(&hspi1,&tx_val,&rx_val,1,0xff);
	//__LCD_CS_SET();
	return destBuff[0];
	
}
void setRotation(uint8_t rotation)
{
		switch(_rotation){

			case 0:
			 /* Memory access control: MY = 0, MX = 0, MV = 0, ML = 0 */
			 /*  */
				lcd_write_register(0X36, 0x00);
				lcd_write_byte(0x2A,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte((LCD_WIDTH-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2B,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(((LCD_HEIGHT-1)>>8)&0xff,LCD_DATA);
				lcd_write_byte((LCD_HEIGHT-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2C,LCD_CMD);
				break;

			case 1:
			 /* Memory access control: MY = 0, MX = 1, MV = 1, ML = 0 */
			 lcd_write_register(0X36, 0x60);
				lcd_write_byte(0x2A,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(((LCD_HEIGHT-1)>>8)&0xff,LCD_DATA);
				lcd_write_byte((LCD_HEIGHT-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2B,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte((LCD_WIDTH-1)&0xff,LCD_DATA);	
				lcd_write_byte(0x2C,LCD_CMD);
				break;

			case 2:
			 /* Memory access control: MY = 1, MX = 1, MV = 0, ML = 0 */
			 lcd_write_register(0X36, 0xC0);
				lcd_write_byte(0x2A,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte((LCD_WIDTH-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2B,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(((LCD_HEIGHT-1)>>8)&0xff,LCD_DATA);
				lcd_write_byte((LCD_HEIGHT-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2C,LCD_CMD);
				break;

			case 3:
			 /* Memory access control: MY = 1, MX = 0, MV = 1, ML = 0 */
			 lcd_write_register(0X36, 0xA0);
			
				lcd_write_byte(0x2A,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(((LCD_HEIGHT-1)>>8)&0xff,LCD_DATA);
				lcd_write_byte((LCD_HEIGHT-1)&0xff,LCD_DATA);

				lcd_write_byte(0x2B,LCD_CMD);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte(0x00,LCD_DATA);
				lcd_write_byte((LCD_WIDTH-1)&0xff,LCD_DATA);
				lcd_write_byte(0x2C,LCD_CMD);
				break;
				
				default:
				break;
		 }

}
void lcd_clear_screen(uint16_t hwColor)  
{
	uint32_t i, wCount = LCD_WIDTH;
	uint8_t hval = hwColor >> 8;
	uint8_t lval = hwColor & 0xFF;
	wCount *= LCD_HEIGHT;
	
	setRotation(_rotation);

		for (i = 0; i < wCount; i ++) {
			lcd_write_byte(hval,1);
			lcd_write_byte(lval,1);
		}
		

}
static void lcd_write_register(uint8_t chRegister, uint8_t chValue)
{
	lcd_write_byte(chRegister, LCD_CMD);
	lcd_write_byte(chValue, LCD_DATA);
}

void lcd_write_byte(uint8_t chByte, uint8_t chCmd)
{
    if (chCmd) {
        __LCD_DC_SET();
    } else {
        __LCD_DC_CLR();
    }
    //__LCD_CS_CLR();
		
		//HAL_SPI_Transmit(&hspi1,&chByte,1,0xff);
		
    spi_transfer_t xfer            = {0};
    
		/*Start Transfer*/
    xfer.txData      = &chByte;
    xfer.rxData      = destBuff;
    xfer.dataSize    = 1;
    xfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferBlocking(LCD_SPI_MASTER, &xfer);
				
    //__LCD_CS_SET();
}
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
    //GPIO_PinInit(BOARD_LCD_DC_GPIO, BOARD_LCD_BKL_GPIO_PORT, BOARD_LCD_BKL_GPIO_PIN, &pin_config);

    

    
    
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
    userConfig.baudRate_Bps = 40000000;
    userConfig.polarity= kSPI_ClockPolarityActiveLow;
		userConfig.phase = kSPI_ClockPhaseFirstEdge;
		
		SPI_MasterInit(LCD_SPI_MASTER, &userConfig, srcFreq);
    
    LCD_InitPins();
		
		
		
		lcd_id = lcd_read_id();


			lcd_write_byte(0x11,LCD_CMD);
			SDK_DelayAtLeastUs(100 * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    
		  lcd_write_register(0x36,0x00);
			lcd_write_register(0x3a,0x55);
			lcd_write_byte(0xb2,LCD_CMD);
			lcd_write_byte(0x0c,LCD_DATA);
			lcd_write_byte(0x0c,LCD_DATA);
			lcd_write_byte(0x00,LCD_DATA);
			lcd_write_byte(0x33,LCD_DATA);
			lcd_write_byte(0x33,LCD_DATA);
			lcd_write_register(0xb7,0x35);
			lcd_write_register(0xbb,0x28);
			lcd_write_register(0xc0,0x3c);
			lcd_write_register(0xc2,0x01);
			lcd_write_register(0xc3,0x0b);
			lcd_write_register(0xc4,0x20);
			lcd_write_register(0xc6,0x0f);
			lcd_write_byte(0xD0,LCD_CMD);
			lcd_write_byte(0xa4,LCD_DATA);
			lcd_write_byte(0xa1,LCD_DATA);
			lcd_write_byte(0xe0,LCD_CMD);
			lcd_write_byte(0xd0,LCD_DATA);
			lcd_write_byte(0x01,LCD_DATA);
			lcd_write_byte(0x08,LCD_DATA);
			lcd_write_byte(0x0f,LCD_DATA);
			lcd_write_byte(0x11,LCD_DATA);
			lcd_write_byte(0x2a,LCD_DATA);
			lcd_write_byte(0x36,LCD_DATA);
			lcd_write_byte(0x55,LCD_DATA);
			lcd_write_byte(0x44,LCD_DATA);
			lcd_write_byte(0x3a,LCD_DATA);
			lcd_write_byte(0x0b,LCD_DATA);
			lcd_write_byte(0x06,LCD_DATA);
			lcd_write_byte(0x11,LCD_DATA);
			lcd_write_byte(0x20,LCD_DATA);
			lcd_write_byte(0xe1,LCD_CMD);
			lcd_write_byte(0xd0,LCD_DATA);
			lcd_write_byte(0x02,LCD_DATA);
			lcd_write_byte(0x07,LCD_DATA);
			lcd_write_byte(0x0a,LCD_DATA);
			lcd_write_byte(0x0b,LCD_DATA);
			lcd_write_byte(0x18,LCD_DATA);
			lcd_write_byte(0x34,LCD_DATA);
			lcd_write_byte(0x43,LCD_DATA);
			lcd_write_byte(0x4a,LCD_DATA);
			lcd_write_byte(0x2b,LCD_DATA);
			lcd_write_byte(0x1b,LCD_DATA);
			lcd_write_byte(0x1c,LCD_DATA);
			lcd_write_byte(0x22,LCD_DATA);
			lcd_write_byte(0x1f,LCD_DATA);
			lcd_write_register(0x55,0xB0);
			lcd_write_byte(0x29,LCD_CMD);
			
			lcd_clear_screen(BLUE);



		
   // LCD_Hx8347Init(LCD_WrData, LCD_WrCmd);
    
    
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


void LCD_SetCursor(uint16_t hwXpos, uint16_t hwYpos)
{
    assert(hwXpos < LCD_WIDTH);
    assert(hwYpos < LCD_HEIGHT);
    
		if(1==_rotation||3==_rotation){
			lcd_write_byte(0x2A,LCD_CMD);
			lcd_write_byte(((hwXpos)>>8)&0xff,LCD_DATA);
			lcd_write_byte((hwXpos)&0xff,LCD_DATA);
			lcd_write_byte(0x2B,LCD_CMD);
			lcd_write_byte(0x00,LCD_DATA);
			lcd_write_byte((hwYpos)&0xff,LCD_DATA);
		}else{
			lcd_write_byte(0x2A,LCD_CMD);
			lcd_write_byte(0x00,LCD_DATA);
			lcd_write_byte(hwXpos&0xff,LCD_DATA);
			lcd_write_byte(0x2B,LCD_CMD);
			lcd_write_byte((hwYpos>>8)&0xff,LCD_DATA);
			lcd_write_byte(hwYpos&0xff,LCD_DATA);
		}
		lcd_write_byte(0x2C, LCD_CMD);

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
void lcd_write_word(uint16_t hwData)
{
		uint8_t hval = hwData >> 8;
		uint8_t lval = hwData & 0xFF;
    __LCD_DC_SET();
    //__LCD_CS_CLR();
		//HAL_SPI_Transmit(&hspi1,&hval,1,0xff);
		//HAL_SPI_Transmit(&hspi1,&lval,1,0xff);
	
	  LCD_WrData(hval);
	  LCD_WrData(lval);
	
    //__LCD_CS_SET();
}
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    assert(x < LCD_WIDTH);
    assert(y < LCD_HEIGHT);
    //uint16_t tempColor = (color >> 8)|(color << 8);
    LCD_SetCursor(x, y);
    lcd_write_byte(0x2C, LCD_CMD);
		lcd_write_word(color);  
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
