/* Copyright (c) 2014 Infor Link ELE. All Rights Reserved.

 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "radio_config.h"
#include "nrf_delay.h"
#include "sys/energest.h"
#include "nrf_drv_common.h"
#include "contiki.h"
#include <string.h>


#define PACKET0_S1_SIZE                  (0UL)  //!< ���������ǲ����
#define PACKET0_S0_SIZE                  (0UL)  //!< ���������ǲ����
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< ���������ǲ����
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address�����趨
#define PACKET1_STATIC_LENGTH            (5UL)  //!< ÿ�η��͹̶�����Ϊ4���ֽڵ����ݰ�
#define PACKET1_PAYLOAD_SIZE             (5UL)  //!< ����������ݳ��ȸ�STALENһ��
unsigned char packet[10];
void radio_configure()
{
  // ��?����?��????
//  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);  //����?�����騦?1|?����??a��?��?4DBm
//  NRF_RADIO->FREQUENCY = 8UL;                                                         //����?����?����?��1����¦�?D?�̨���??a��??a2.4008GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //1�����?�꨺?
	  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);  //����?�����騦?1|?����??a��?��?0DBm
  NRF_RADIO->FREQUENCY =100UL;                                                         //����?����?����?��1����¦�?D?�̨���??a��??a2.440GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //1�����?�꨺?
NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
  // ����?����??��
//  NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // ???-��??��      51822[31:24] = 2401[24:31]
//  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // ???-��??������?��
	NRF_RADIO->PREFIX0 = 0x23C343E7UL;  // ???-��??��      51822[31:24] = 2401[24:31]
  NRF_RADIO->PREFIX1 = 0x13E363A3UL;  // ???-��??������?��
  NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // ???-��??��
  NRF_RADIO->BASE1   = 0x43434343UL;  // ???-��??������?��
  NRF_RADIO->TXADDRESS = 0x01UL;      // ����?����??������?��   �����̨�0
//  NRF_RADIO->RXADDRESSES = 0x01UL;    // ?����?��??������?��  �����̨�0
 NRF_RADIO->RXADDRESSES = (1<<0);     // ?����?��??������?��  �����̨�0  1111 1111
	
  // Packet configuration
  NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
                     (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
                     (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos); 

  // Packet configuration
   NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
                      (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
                      (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
                      (PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)           |
                      (PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_MAXLEN_Pos); 

  // CRC����?��
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // 2��??��CRC
  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // CRC?????D3?��??��     
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFUL;        // CRC?????D3?��??��     
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }
  NRF_RADIO->PACKETPTR = (uint32_t)packet; // Set payload pointer

  NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos;
  //nrf_drv_common_irq_enable(RADIO_IRQn,1);
  NVIC_SetPriority(RADIO_IRQn, 1);
  NVIC_ClearPendingIRQ(RADIO_IRQn);
  NVIC_EnableIRQ(RADIO_IRQn);
  nrf_delay_ms(3);
}

extern process_event_t ev_2_4g_rcv;
PROCESS_NAME(led_process);
extern unsigned char tags_local[200][5];
extern int tags_index;

void RADIO_IRQHandler(void)
{
  int i = 0;
  DISABLE_INTERRUPTS();
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  //nrf_delay_ms(3);
  if(NRF_RADIO->EVENTS_END)
  {
    NRF_RADIO->EVENTS_END = 0;
        //*((volatile uint32_t *)((uint8_t *)NRF_TIMERx + (uint32_t)timer_event)) = 0x0UL;
    if (NRF_RADIO->CRCSTATUS == 1U)
    {
      NRF_RADIO->EVENTS_READY = 0U; 				 // ��??t��?��? ��?����?�꨺?��a??����3��  ����????    
      NRF_RADIO->TASKS_RXEN   = 1U;          // ��1?��?����?
      //while(NRF_RADIO->EVENTS_READY == 0U)   // �̨���y?����?��?��?o?
      {
      }
      for(i = 0;i < 200;i++)
      {
        if(!memcmp(packet,tags_local[i],4))
        {
          memcpy(tags_local[i],packet,5);
          break;
        }
      }
      if(i == 200)
      {
        memcpy(tags_local[tags_index++],packet,5);
        if(tags_index >= 200)
        {
            tags_index = 0;
        }
      }
      //process_post(&led_process,ev_2_4g_rcv,NULL);
      NRF_RADIO->EVENTS_END = 0U;  					 // ?����?��??t			
      NRF_RADIO->TASKS_START = 1U;           // ?a��?

    }
  }
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
  ENABLE_INTERRUPTS();
}

