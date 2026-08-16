/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * NMH1000 read-data-polling example.
 *
 * Adapted from STMicroelectronics/STMems_Standard_C_drivers
 *   nmh1000_STdC/examples/nmh1000_read_data_polling.c
 * onto the NXP fsl_nmh1000 driver shipped in this SDK. The ST example targets
 * ST's driver (stmdev_ctx_t / nmh1000_reg.h); this port uses the NXP API
 * (nmh_handle_t / NMH_Init / NMH_ReadMagData / NMH_ReadReg) and LPI2C.
 */

#ifndef _NMH1000_READ_DATA_POLLING_H_
#define _NMH1000_READ_DATA_POLLING_H_

#include "fsl_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Run the NMH1000 read-data-polling example.
 *
 * Initializes the LPI2C clock and peripheral, initializes the NMH1000 sensor
 * (auto mode + ODR via NMH_Init), then continuously polls the data-ready bit,
 * reads the relative magnetic field strength, and prints it over the console.
 *
 * This function does not return under normal operation.
 *
 * @return kStatus_Fail if sensor/peripheral initialization fails.
 */
status_t NMH1000_ReadDataPollingExample(void);

#if defined(__cplusplus)
}
#endif

#endif /* _NMH1000_READ_DATA_POLLING_H_ */
