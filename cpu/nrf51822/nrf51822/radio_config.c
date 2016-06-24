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

#define PACKET0_S1_SIZE                  (0UL)  //!< 此例程我们不理会
#define PACKET0_S0_SIZE                  (0UL)  //!< 此例程我们不理会
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< 此例程我们不理会
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address长度设定
#define PACKET1_STATIC_LENGTH            (5UL)  //!< 每次发送固定长度为4个字节的数据包
#define PACKET1_PAYLOAD_SIZE             (5UL)  //!< 保持最大数据长度跟STALEN一致

void radio_configure()
{
  // 收发器配置
//  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);  //设定发射功率，这里是4DBm
//  NRF_RADIO->FREQUENCY = 8UL;                                                         //设定收发器工作的信道，这里为2.4008GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //工作模式
	  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);  //设定发射功率，这里是0DBm
  NRF_RADIO->FREQUENCY =100UL;                                                         //设定收发器工作的信道，这里为2.440GHZ
//  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);               //工作模式
NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
  // 设定地址
//  NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // 逻辑地址      51822[31:24] = 2401[24:31]
//  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // 逻辑地址设定
	NRF_RADIO->PREFIX0 = 0x23C343E7UL;  // 逻辑地址      51822[31:24] = 2401[24:31]
  NRF_RADIO->PREFIX1 = 0x13E363A3UL;  // 逻辑地址设定
  NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // 逻辑地址
  NRF_RADIO->BASE1   = 0x43434343UL;  // 逻辑地址设定
  NRF_RADIO->TXADDRESS = 0x01UL;      // 发送地址设定   通道0
//  NRF_RADIO->RXADDRESSES = 0x01UL;    // 接收地址设定  通道0
 NRF_RADIO->RXADDRESSES = (1<<0);     // 接收地址设定  通道0  1111 1111
	
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

  // CRC设定
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // 2字节CRC
  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // CRC计算中初始值     
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
  {
    NRF_RADIO->CRCINIT = 0xFFUL;        // CRC计算中初始值     
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }
  
  nrf_delay_ms(3);
}


