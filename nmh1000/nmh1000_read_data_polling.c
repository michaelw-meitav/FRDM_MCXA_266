/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * NMH1000 read-data-polling example.
 *
 * Adapted from STMicroelectronics/STMems_Standard_C_drivers
 *   nmh1000_STdC/examples/nmh1000_read_data_polling.c
 * (BSD-3-Clause, (c) STMicroelectronics) onto the NXP fsl_nmh1000 driver.
 *
 * API mapping (ST driver -> NXP driver used here):
 *   stmdev_ctx_t / write_reg / read_reg  -> nmh_handle_t + I2C_SendFunc/I2C_ReceiveFunc
 *   nmh1000_device_id_get                -> NMH_ReadReg(NMH1000_WHO_AM_I)
 *   nmh1000_auto_mode_set / user_odr_set -> performed inside NMH_Init
 *   nmh1000_status_get / .b.mdr          -> NMH_ReadReg(NMH1000_CONTROL_REG1) + MDR mask
 *   nmh1000_magnetic_field_get           -> NMH_ReadMagData
 *   tx_com                               -> PRINTF (fsl_debug_console)
 *   platform_write / platform_read       -> NMH1000_I2C_Send / NMH1000_I2C_Receive (LPI2C)
 */

#include "nmh1000_read_data_polling.h"
#include "nmh1000_clock.h"
#include "fsl_nmh1000.h"

#include "fsl_lpi2c.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* LPI2C bus speed used to talk to the NMH1000. */
#ifndef NMH1000_LPI2C_BAUDRATE
#define NMH1000_LPI2C_BAUDRATE 100000U
#endif

/*
 * Expected WHO_AM_I value for the NMH1000.
 *
 * TODO: Verify this value against your NMH1000 variant's datasheet. The device
 *       may ship with WHO_AM_I = 0x00 (unprogrammed) or a custom value, so a
 *       mismatch is reported but not treated as fatal below.
 */
#ifndef NMH1000_WHO_AM_I_EXPECTED
#define NMH1000_WHO_AM_I_EXPECTED 0xC4U
#endif

/*
 * Data-ready (MDR) bit inside CONTROL_REG1.
 *
 * TODO: Verify this mask against the NMH1000 register map. The NXP driver does
 *       not expose a status accessor, so we poll CONTROL_REG1 directly.
 */
#ifndef NMH1000_MDR_MASK
#define NMH1000_MDR_MASK ((uint8_t)0x80U)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static status_t NMH1000_I2C_Send(uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subaddressSize,
                                 uint8_t data);
static status_t NMH1000_I2C_Receive(uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subaddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static nmh_handle_t s_nmhHandle;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Convert the relative magnetic field code returned by NMH_ReadMagData.
 * The NXP driver already right-shifts OUT_M_REG by 2 bits, so magData is the
 * raw 6-bit relative field value.
 *
 * TODO: Replace with the correct Gauss scaling for your sensor variant if an
 *       absolute unit is required (the ST example used nmh1000_convert_to_G). */
static float NMH1000_ConvertToG(uint8_t magData)
{
    return (float)magData;
}

static void NMH1000_Lpi2cInit(void)
{
    lpi2c_master_config_t masterConfig;

    /* Set up the LPI2C functional clock (source/divider) for the sensor bus. */
    NMH1000_ClockInit();

    LPI2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Hz = NMH1000_LPI2C_BAUDRATE;

    LPI2C_MasterInit(BOARD_NMH1000_LPI2C_BASEADDR, &masterConfig, NMH1000_GetLpi2cClockFreq());
}

status_t NMH1000_ReadDataPollingExample(void)
{
    nmh_config_t config;
    uint8_t who = 0U;
    uint8_t count = 0U;

    /* Bring up the LPI2C bus the sensor is connected to. */
    NMH1000_Lpi2cInit();

    /* Read WHO_AM_I before init so the transport is exercised early. */
    s_nmhHandle.I2C_SendFunc    = NMH1000_I2C_Send;
    s_nmhHandle.I2C_ReceiveFunc = NMH1000_I2C_Receive;

    if (NMH_ReadReg(&s_nmhHandle, NMH1000_WHO_AM_I, &who, 1U) != kStatus_Success)
    {
        PRINTF("NMH1000: failed to read WHO_AM_I\r\n");
    }
    else if (who == NMH1000_WHO_AM_I_EXPECTED)
    {
        PRINTF("NMH1000: initialized, WHO_AM_I = 0x%02X\r\n", who);
    }
    else
    {
        PRINTF("NMH1000: unexpected WHO_AM_I = 0x%02X (expected 0x%02X)\r\n",
               who, (uint8_t)NMH1000_WHO_AM_I_EXPECTED);
    }

    /* NMH_Init programs USER_ODR (sample rate) and enables AUTO mode. */
    config.I2C_SendFunc    = NMH1000_I2C_Send;
    config.I2C_ReceiveFunc = NMH1000_I2C_Receive;

    if (NMH_Init(&s_nmhHandle, &config) != kStatus_Success)
    {
        PRINTF("NMH1000: init failed\r\n");
        return kStatus_Fail;
    }

    /* Infinite polling loop (equivalent to nmh1000_read_data_polling). */
    while (1)
    {
        uint8_t ctrl = 0U;
        uint8_t magData = 0U;
        float mag;

        /* Wait for data ready: poll CONTROL_REG1 MDR bit. */
        if (NMH_ReadReg(&s_nmhHandle, NMH1000_CONTROL_REG1, &ctrl, 1U) != kStatus_Success)
        {
            continue;
        }
        if ((ctrl & NMH1000_MDR_MASK) == 0U)
        {
            continue;
        }

        /* Read relative magnetic field strength. */
        if (NMH_ReadMagData(&s_nmhHandle, &magData) != kStatus_Success)
        {
            continue;
        }

        mag = NMH1000_ConvertToG(magData);
        count++;
        PRINTF("Event: %d, Magnetic Field: %0.1f\r\n", count, mag);
    }

    /* Not reached. */
}

/*
 * @brief Write a single byte to a register of the NMH1000 (platform dependent).
 *
 * Equivalent to the ST example's platform_write. Uses a blocking LPI2C write
 * transfer with an 8-bit sub-address (register).
 */
static status_t NMH1000_I2C_Send(uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subaddressSize,
                                 uint8_t data)
{
    lpi2c_master_transfer_t xfer;

    (void)memset(&xfer, 0, sizeof(xfer));
    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subaddressSize;
    xfer.data           = &data;
    xfer.dataSize       = 1U;

    return LPI2C_MasterTransferBlocking(BOARD_NMH1000_LPI2C_BASEADDR, &xfer);
}

/*
 * @brief Read one or more bytes from a register of the NMH1000 (platform dependent).
 *
 * Equivalent to the ST example's platform_read. Uses a blocking LPI2C read
 * transfer with an 8-bit sub-address (register).
 */
static status_t NMH1000_I2C_Receive(uint8_t deviceAddress,
                                    uint32_t subAddress,
                                    uint8_t subaddressSize,
                                    uint8_t *rxBuff,
                                    uint8_t rxBuffSize)
{
    lpi2c_master_transfer_t xfer;

    (void)memset(&xfer, 0, sizeof(xfer));
    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = deviceAddress;
    xfer.direction      = kLPI2C_Read;
    xfer.subaddress     = subAddress;
    xfer.subaddressSize = subaddressSize;
    xfer.data           = rxBuff;
    xfer.dataSize       = rxBuffSize;

    return LPI2C_MasterTransferBlocking(BOARD_NMH1000_LPI2C_BASEADDR, &xfer);
}
