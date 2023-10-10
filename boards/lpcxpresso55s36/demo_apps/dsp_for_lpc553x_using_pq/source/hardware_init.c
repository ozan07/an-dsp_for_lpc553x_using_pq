/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/*${header:start}*/
#include "pin_mux.h"
#include "board.h"
#include "clock_config.h"

void BOARD_InitHardware(void)
{
    /* Clocks. */
    //BOARD_BootClockFROHF96M();
    BOARD_BootClockPLL150M();

    //CLOCK_SetFLASHAccessCycles(kCLOCK_Flash7Cycle); /* Setup the flash clock divider. */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH); /* attach main clock divide to FLEXCOMM0 (debug console) */

    /* Pins. */
    BOARD_InitPins();

    /* Peripherals. */
    BOARD_InitDebugConsole();
}

