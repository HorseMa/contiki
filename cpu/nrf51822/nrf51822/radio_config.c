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

#define PACKET0_S1_SIZE                  (0UL)  //!< ���������ǲ����
#define PACKET0_S0_SIZE                  (0UL)  //!< ���������ǲ����
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< ���������ǲ����
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address�����趨
#define PACKET1_STATIC_LENGTH            (5UL)  //!< ÿ�η��͹̶�����Ϊ4���ֽڵ����ݰ�
#define PACKET1_PAYLOAD_SIZE             (5UL)  //!< ����������ݳ��ȸ�STALENһ��

void radio_configure()
{
  // �շ�������
//  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);  //�趨���书�ʣ�������4DBm
//  NRF_RADIO->FREQUENCY = 8UL;                                                         //�趨�շ����������ŵ�������Ϊ2.4008GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //����ģʽ
	  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);  //�趨���书�ʣ�������0DBm
  NRF_RADIO->FREQUENCY =100UL;                                                         //�趨�շ����������ŵ�������Ϊ2.440GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //����ģʽ
NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
  // �趨��ַ
//  NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // �߼���ַ      51822[31:24] = 2401[24:31]
//  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // �߼���ַ�趨
	NRF_RADIO->PREFIX0 = 0x23C343E7UL;  // �߼���ַ      51822[31:24] = 2401[24:31]
  NRF_RADIO->PREFIX1 = 0x13E363A3UL;  // �߼���ַ�趨
  NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // �߼���ַ
  NRF_RADIO->BASE1   = 0x43434343UL;  // �߼���ַ�趨
  NRF_RADIO->TXADDRESS = 0x01UL;      // ���͵�ַ�趨   ͨ��0
//  NRF_RADIO->RXADDRESSES = 0x01UL;    // ���յ�ַ�趨  ͨ��0
 NRF_RADIO->RXADDRESSES = (1<<0);     // ���յ�ַ�趨  ͨ��0  1111 1111
	
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

  // CRC�趨
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // 2�ֽ�CRC
  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // CRC�����г�ʼֵ     
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFUL;        // CRC�����г�ʼֵ     
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }
  
  nrf_delay_ms(3);
}


