/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nmh1000_clock.h"
#include "fsl_clock.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

void NMH1000_ClockInit(void)
{
    /* Attach the configured functional clock source to the LPI2C instance. */
    CLOCK_SetClockDiv(kCLOCK_DivLPI2C0, BOARD_NMH1000_LPI2C_CLOCK_DIV);
    CLOCK_AttachClk(BOARD_NMH1000_LPI2C_CLOCK_ATTACH);
}

uint32_t NMH1000_GetLpi2cClockFreq(void)
{
    return BOARD_NMH1000_LPI2C_CLOCK_FREQ;
}
