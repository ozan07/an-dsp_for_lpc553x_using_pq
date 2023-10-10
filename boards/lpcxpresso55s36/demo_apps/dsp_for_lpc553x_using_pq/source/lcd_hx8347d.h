/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _LCD_HX8347D_H_
#define _LCD_HX8347D_H_

/* 120ms is necessary after reset, for loading ID bytes, VCOM setting,
 * and other settings from NV memory to registers.
 */
#define HX8347_RESET_CANCEL_MS 120U

typedef void (*hx8347_send_byte_t)(uint8_t dataToSend);
void LCD_Hx8347Init(hx8347_send_byte_t _writeData, hx8347_send_byte_t _writeCommand);
#endif
