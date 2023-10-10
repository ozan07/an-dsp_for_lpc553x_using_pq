/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "app.h"
#include "lcd.h"
#include "fsl_inputmux.h"
#include "fsl_pint.h"
#include "fsl_inputmux_connections.h"


/* Enable SW3 (USER) button to switch different PowerQuad computation */


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USER_BUTTON_PIN_INT0_SRC    kINPUTMUX_GpioPort0Pin17ToPintsel

#define USER_MENU_MAX_INDEX         8


/*******************************************************************************
 * Variable
 ******************************************************************************/
volatile int32_t menu_index = -1;           //menu index
volatile uint32_t menu_update_flag = 0;     //indicates menu changed

/*******************************************************************************
 * Code
 ******************************************************************************/
void user_button_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
    
    if(menu_index < USER_MENU_MAX_INDEX)
    {
        menu_index++;
    }
    else
    {
        menu_index = 0;
    }
    menu_update_flag = 1;
    
    PRINTF("\f\r\nPINT Pin Interrupt %d event detected.", menu_index);
}




void APP_InitUserKey(void)
{
    /* Connect trigger sources to PINT */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, USER_BUTTON_PIN_INT0_SRC);
    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);
    
    /* Initialize PINT */
    PINT_Init(PINT);
    
    /* Setup Pin Interrupt 0 for rising edge */
    PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, user_button_callback);
    /* Enable callbacks for PINT0 by Index */
    PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);
}

