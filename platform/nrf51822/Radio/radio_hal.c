/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "compiler_defs.h"
#include "si446x_defs.h"
#include "nrf51.h"
#include "bps.h"
#include "nrf_gpio.h"
#include <spi_master.h>
#include "spi_master_config.h"



                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */
static uint32_t *spi_base_address;
                /* ======================================= *
                 *     P U B L I C   F U N C T I O N S     *
                 * ======================================= */
void si4463_spi_init(void)
{
  spi_base_address = spi_master_init(SPI1, SPI_MODE0, 0);
}
void radio_hal_AssertShutdown(void)
{
  nrf_gpio_pin_set(SI4463_CEN);
}

void radio_hal_DeassertShutdown(void)
{
  nrf_gpio_pin_clear(SI4463_CEN);
}

void radio_hal_ClearNsel(void)
{
  nrf_gpio_pin_clear(SPI_PSELSS1);
}

void radio_hal_SetNsel(void)
{
  nrf_gpio_pin_set(SPI_PSELSS1);
}

BIT radio_hal_NirqLevel(void)
{
  return nrf_gpio_pin_read(SI4463_INT);
}

U8 SPI_RW(U8 byte)
{
  uint8_t temp;
  spi_master_tx_rx(spi_base_address, 1, &byte, &temp);
  return temp;
}
void radio_hal_SpiWriteByte(U8 byteToWrite)
{
  SPI_RW(byteToWrite);
}

U8 radio_hal_SpiReadByte(void)
{
  return SPI_RW(0xff);
}
void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
  U8 u8Loop = 0;
  for(u8Loop = 0; u8Loop < byteCount; u8Loop ++)
  {
    radio_hal_SpiWriteByte(pData[u8Loop]);
  }
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
  U8 u8Loop = 0;
  for(u8Loop = 0; u8Loop < byteCount; u8Loop ++)
  {
    pData[u8Loop] = radio_hal_SpiReadByte();
  }
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
BIT radio_hal_Gpio0Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FALSE;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = EZRP_RX_DOUT;
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB)
  retVal = READ_RF_GPIO0_PIN();
#endif
#if (defined SILABS_PLATFORM_WMB930)
  retVal = FALSE;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio1Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FSK_CLK_OUT;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = READ_RF_GPIO1_PIN();
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio2Level(void)
{
  BIT retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = DATA_NFFS;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = READ_RF_GPIO2_PIN();
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

BIT radio_hal_Gpio3Level(void)
{
  BIT retVal = FALSE;

#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = READ_RF_GPIO3_PIN();
#elif defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

#endif
