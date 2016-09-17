/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "compiler_defs.h"
#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "radio_hal.h"
#include "radio.h"
#include "radio_config.h"
//#include "packetsbuf.h"
//#include "bsp.h"
//#include "global.h"
#include "lib/ringbuf.h"
#include "contiki.h"
#include "dev_cfg.h"
#include "tasks.h"
#include "ad_hoc.h"
#include <string.h>
/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const U8 Radio_Configuration_Data_Array[] = 
              RADIO_CONFIGURATION_DATA_ARRAY;

const tRadioConfiguration RadioConfiguration = 
{
  .Radio_ConfigurationArray = (U8*)Radio_Configuration_Data_Array,
  .Radio_ChannelNumber = RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,
  .Radio_PacketLength = RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,
  .Radio_State_After_Power_Up = RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,
  .Radio_Delay_Cnt_After_Reset = RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET,
  //.Radio_CustomPayload = RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD
};

const tRadioConfiguration *pRadioConfiguration = 
                        &RadioConfiguration;

//U8 customRadioPacket[RADIO_MAX_PACKET_LENGTH];
//U8 customRadioPacketLen;


//struct mmem mmem_pkt_rx;

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  U16 wDelay = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  U16 wDelay;
  si4463_spi_init();
  /* Power Up the radio chip */
  vRadio_PowerUp();
  si446x_part_info();
  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
static U8 buffer_433m[64];
U8 bRadio_Check_Tx_RX(void)
{
  //static unsigned char data[64];
  unsigned char loop = 0;
  pst_PkgFormart pstPkgFormart;
  if (radio_hal_NirqLevel() == 0)
  {
      /* Read ITs, clear pending ones */
      si446x_get_int_status(0u, 0u, 0u);
#if 1
      if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_BIT)
      {
      	/* State change to */
      	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
	
	/* Reset FIFO */
      	si446x_fifo_info(0x03);
      
	/* State change to */
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
        return 0;
      }
#endif
      if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_PEND_BIT)
      {
      	/* Reset FIFO */
      	si446x_fifo_info(0x03);
        /* State change to */
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
        return 0;
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_TX_FIFO_ALMOST_EMPTY_PEND_BIT)
      {
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
      {
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
        if(adhocGetWorkMode() == WORK_MODE_CENTER)
        {
          process_post(&si4463_center_process,ev_433_tx_over,NULL);
        }
        else
        {
          process_post(&si4463_enddev_process,ev_433_tx_over,NULL);
        }
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_PREAMBLE_DETECT_PEND_BIT)
      {
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_INVALID_PREAMBLE_PEND_BIT)
      {
      }
      if(Si446xCmd.GET_MODEM_STATUS.MODEM_PEND & SI446X_CMD_GET_MODEM_STATUS_REP_SYNC_DETECT_PEND_BIT)
      {
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_PEND_BIT)
      {
      }
      if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
      {
        /* Get payload length */
        si446x_fifo_info(0x00);
        si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, buffer_433m);
        pstPkgFormart = (pst_PkgFormart)buffer_433m;
        switch(pstPkgFormart->cmd)
        {
          case enFactoryReset:
            if((pstPkgFormart->dest_addr != stDefaultCfg.dev_id) && (pstPkgFormart->dest_addr != 0xffff))
            {
              return 0;
            }
            earase_cfg();
            stDevCfg.rx_gain = stDefaultCfg.rx_gain;
            memcpy(stDevCfg.local_ip,stDefaultCfg.local_ip,4);
            stDevCfg.local_port = stDefaultCfg.local_port;
            memcpy(stDevCfg.server_ip,stDefaultCfg.server_ip,4);
            stDevCfg.server_port = stDefaultCfg.server_port;
            stDevCfg.tag_type = stDefaultCfg.tag_type;
            stDevCfg.reserved1 = 0;
            write_cfg();
            NVIC_SystemReset();
            return 0;
          case enInactive:
            if((pstPkgFormart->dest_addr != stDefaultCfg.dev_id) && (pstPkgFormart->dest_addr != 0xffff))
            {
              return 0;
            }
            if(stDefaultCfg.active == 1)
            {
              stDefaultCfg.active = 0;
              earase_cfg();
              write_cfg();
              NVIC_SystemReset();
            }
            return 0;
          case enActive:
            if((pstPkgFormart->dest_addr != stDefaultCfg.dev_id) && (pstPkgFormart->dest_addr != 0xffff))
            {
              return 0;
            }
            if(stDefaultCfg.active == 0)
            {
              stDefaultCfg.active = 1;
              earase_cfg();
              write_cfg();
              NVIC_SystemReset();
            }
            return 0;
          default:
            break;
        }
        //if(net_flag == 1)
        if(adhocGetWorkMode() == WORK_MODE_CENTER)
        {
          process_post(&si4463_center_process,ev_433_rx_over,buffer_433m);
        }
        else
        {
          process_post(&si4463_enddev_process,ev_433_rx_over,buffer_433m);
        }
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
/*extern unsigned char tags_local[200][9];
extern int tags_index;
extern int tags_cnt;
extern uint8_t tag_update;

        tag_update = 1;
        if(stDevCfg.tag_type)
        {
          for(uint8 loop = 0;loop < 12;loop ++)
          {
            memcpy(tags_local[loop],buffer_433m + 5 * loop,5);
          }
          tags_cnt = 12;
          tags_index = 12;
          tag_update = 1;

        }else
        {
          for(uint8 loop = 0;loop < 7;loop ++)
          {
            memcpy(tags_local[loop],buffer_433m + 9 * loop,9);
          }
          tags_cnt = 7;
          tags_index = 7;
          tag_update = 1;
        }*/
        //for(loop = 0;loop < Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;loop ++)
          //ringbuf_put(&radioRcvRingBuf, data[loop]);
      }
  }

  return 0;
}

/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 ch)
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

   // Reset the Rx Fifo
   si446x_fifo_info(0x03);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  si446x_start_rx(ch, 0u, 0,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );
}

/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(unsigned char channel, unsigned char *bytes, unsigned char len)
{
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset the Tx Fifo */
  si446x_fifo_info(0x03);

  /* Fill the TX fifo with datas */
  //si446x_write_tx_fifo(1, &len);
  si446x_write_tx_fifo(len, bytes);

  /* Start sending packet, channel 0, START immediately */
   si446x_start_tx(channel, 0x80, len);
 
}
