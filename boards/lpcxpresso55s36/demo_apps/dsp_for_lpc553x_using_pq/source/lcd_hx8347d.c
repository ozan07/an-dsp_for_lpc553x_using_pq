/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_common.h"
#include "lcd_hx8347d.h"





void LCD_Hx8347Init(hx8347_send_byte_t _writeData, hx8347_send_byte_t _writeCommand)
{
    SDK_DelayAtLeastUs(HX8347_RESET_CANCEL_MS * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    
    _writeCommand(0xEA);    //PTBA[15:8]
    _writeData(0x00);
    _writeCommand(0xEB);    //PTBA[7:0]
    _writeData(0x20);
    _writeCommand(0xEC);    //STBA[15:8]
    _writeData(0x0C);
    _writeCommand(0xED);    //STBA[7:0]
    _writeData(0xC4);
    _writeCommand(0xE8);    //OPON[7:0]
    _writeData(0x38);
    _writeCommand(0xE9);    //OPON1[7:0]
    _writeData(0x10);
    _writeCommand(0xF1);    //OTPS1B
    _writeData(0x01);
    _writeCommand(0xF2);    //GEN
    _writeData(0x10);
    
    //Gamma 2.2 Setting
     _writeCommand(0x40);
    _writeData(0x01);
    _writeCommand(0x41);
    _writeData(0x00);
    _writeCommand(0x42);
    _writeData(0x00);
    _writeCommand(0x43);
    _writeData(0x10);
    _writeCommand(0x44);
    _writeData(0x0E);
    _writeCommand(0x45);
    _writeData(0x24);
    _writeCommand(0x46);
    _writeData(0x04);
    _writeCommand(0x47);
    _writeData(0x50);
    _writeCommand(0x48);
    _writeData(0x02);
    _writeCommand(0x49);
    _writeData(0x13);
    _writeCommand(0x4A);
    _writeData(0x19);
    _writeCommand(0x4B);
    _writeData(0x19);
    _writeCommand(0x4C);
    _writeData(0x16);
    
    _writeCommand(0x50);
    _writeData(0x1B);
    _writeCommand(0x51);
    _writeData(0x31);
    _writeCommand(0x52);
    _writeData(0x2F);
    _writeCommand(0x53);
    _writeData(0x3F);
    _writeCommand(0x54);
    _writeData(0x3F);
    _writeCommand(0x55);
    _writeData(0x3E);
    _writeCommand(0x56);
    _writeData(0x2F);
    _writeCommand(0x57);
    _writeData(0x7B);
    _writeCommand(0x58);
    _writeData(0x09);
    _writeCommand(0x59);
    _writeData(0x06);
    _writeCommand(0x5A);
    _writeData(0x06);
    _writeCommand(0x5B);
    _writeData(0x0C);
    _writeCommand(0x5C);
    _writeData(0x1D);
    _writeCommand(0x5D);
    _writeData(0xCC);
    
    //Power Voltage Setting
    _writeCommand(0x1B);    //VRH=4.65V
    _writeData(0x1B);
    _writeCommand(0x1A);    //BT (VGH~15V,VGL~-10V,DDVDH~5V)
    _writeData(0x01);
    _writeCommand(0x24);    //VMH(VCOM High voltage ~3.2V)
    _writeData(0x2F);
    _writeCommand(0x25);    //VML(VCOM Low voltage -1.2V)
    _writeData(0x57);
    
    //****VCOM offset**
    _writeCommand(0x23);    //for Flicker adjust //can reload from OTP
    _writeData(0x88);
    
    //Power on Setting
    _writeCommand(0x18);
    _writeData(0x34);
    _writeCommand(0x19);
    _writeData(0x01);
    _writeCommand(0x01);
    _writeData(0x00);
    _writeCommand(0x1F);
    _writeData(0x88);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    _writeCommand(0x1F);
    _writeData(0x80);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    _writeCommand(0x1F);
    _writeData(0x90);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    _writeCommand(0x1F);
    _writeData(0xD0);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    
    //262k/65k color selection
    _writeCommand(0x17);
    _writeData(0x05);
    
    //SET PANEL
    _writeCommand(0x36);
    _writeData(0x00);
    //Display ON Setting
    _writeCommand(0x28);
    _writeData(0x38);
    SDK_DelayAtLeastUs(40000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    _writeCommand(0x28);
    _writeData(0x3F);
    
    _writeCommand(0x16);
    _writeData(0x18);
    
    //Set GRAM AREA
    _writeCommand(0x02);    //Column Start
    _writeData(0x00);
    _writeCommand(0x03);    
    _writeData(0x00);
    _writeCommand(0x04);    //Column End
    _writeData(0x00);
    _writeCommand(0x05);    
    _writeData(0xEF);
    _writeCommand(0x06);    //Row Start
    _writeData(0x00);
    _writeCommand(0x07);    
    _writeData(0x00);
    _writeCommand(0x08);    //Row End
    _writeData(0x01);
    _writeCommand(0x09);    
    _writeData(0x3F);
    
    _writeCommand(0x22);    //memory write
    
}



