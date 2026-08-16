/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NMH1000_CLOCK_H_
#define _NMH1000_CLOCK_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * LPI2C instance the NMH1000 sensor is connected to.
 *
 * TODO: Verify this matches the LPI2C peripheral wired to the FRDM/Arduino
 *       header used by the NMH1000 on your board. Change if needed.
 */
#ifndef BOARD_NMH1000_LPI2C_BASEADDR
#define BOARD_NMH1000_LPI2C_BASEADDR LPI2C0
#endif

/*
 * Functional clock source and divider for the LPI2C instance.
 *
 * TODO: Verify the clock attach/div selectors below against your board's
 *       clock_config.c. These defaults target the FRO12M source with no
 *       division, which is a safe/common choice for I2C on MCXA parts.
 */
#ifndef BOARD_NMH1000_LPI2C_CLOCK_ATTACH
#define BOARD_NMH1000_LPI2C_CLOCK_ATTACH kFRO12M_to_LPI2C0
#endif

#ifndef BOARD_NMH1000_LPI2C_CLOCK_DIV
#define BOARD_NMH1000_LPI2C_CLOCK_DIV 1U
#endif

/* Frequency (Hz) of the LPI2C functional clock after the attach/div above.
 * Used by callers to configure the LPI2C baud rate. */
#ifndef BOARD_NMH1000_LPI2C_CLOCK_FREQ
#define BOARD_NMH1000_LPI2C_CLOCK_FREQ 12000000U
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Configure and enable the LPI2C functional clock used by the NMH1000.
 *
 * Attaches the configured clock source to the LPI2C instance and applies the
 * configured divider. Call this once, before initializing the LPI2C driver.
 */
void NMH1000_ClockInit(void);

/*!
 * @brief Return the LPI2C functional clock frequency in Hz.
 *
 * @return The configured LPI2C clock frequency (BOARD_NMH1000_LPI2C_CLOCK_FREQ).
 */
uint32_t NMH1000_GetLpi2cClockFreq(void);

#if defined(__cplusplus)
}
#endif

#endif /* _NMH1000_CLOCK_H_ */
