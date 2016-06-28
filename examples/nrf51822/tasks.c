#include "contiki.h"
#include "tasks.h"
#include "bps.h"
#include "list.h"
#include <string.h>
#include "radio_config.h"
#include "nrf51.h"
#include "spi_master.h"
#include "device.h"
#include "dhcp.h"
#include "config.h"
#include "sockutil.h"
#include "radio_config.h"
#include "w5500.h"
#include "socket.h"
process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

unsigned char phone_num[11];

PROCESS(led_process,"led state");
PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&led_process,&uartRecv_process,&read_gpio_process);

void clockInit(void)
{
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;  					// ??3y赤a2????迄那㊣?車℅?㊣?o?㊣那?? 
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;     			// ???‘赤a2????迄那㊣?車
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)  	// 赤a2????迄那㊣?車℅?㊣?o?
  {
  }
}
PROCESS_THREAD(led_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
  static unsigned char packet[10];
  radio_configure();
  NRF_RADIO->PACKETPTR = (uint32_t)packet; // Set payload pointer

  while(1)
  {
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();

  }
  
  PROCESS_END();
}


PROCESS_THREAD(read_gpio_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
  while(1)
  {
    uint8_t pc_ip[4]={192,168,1,101};/*??????IP??*/
    uint16_t anyport=3000;/*????????????*/
    uint16_t len=0;
    unsigned char buffer[100];
    gipo_init();
    spi_w5500_init();
    Reset_W5500();
    set_default(); 
    //set_network();
	//os_dly_wait(10);
    //GetNetConfig();
    init_dhcp_client();
    uint8_t dhcpret=0;
    do{
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
      dhcpret = check_DHCP_state(SOCK_DHCP);
      if(dhcpret == DHCP_RET_NONE)
      {
      }
      if(dhcpret == DHCP_RET_TIMEOUT)
      {
      }
      if(dhcpret == DHCP_RET_UPDATE)
      {
        break;
      }
      if(dhcpret == DHCP_RET_CONFLICT)
      {
      }
    }while(dhcpret != DHCP_RET_UPDATE);
    PROCESS_WAIT_EVENT();
#if 0
	while(1)
  {
		uint8_t dhcpret=0;
		do{
			//os_dly_wait(10);
			dhcpret = check_DHCP_state(SOCK_DHCP);
			switch(dhcpret)
			{
				case DHCP_RET_NONE:
					break;
				case DHCP_RET_TIMEOUT:
					break;
				case DHCP_RET_UPDATE:
					break;
				case DHCP_RET_CONFLICT:
					os_dly_wait(10);
				default:
					break;
			}
		}while(dhcpret != DHCP_RET_UPDATE);
		while(1){
			switch(getSn_SR(0))/*??socket0???*/
			{
				case SOCK_INIT:/*socket?????*/
					connect(0, pc_ip ,6001);/*?TCP?????????????*/ 
					break;
				case SOCK_ESTABLISHED:/*socket????*/
					if(getSn_IR(0) & Sn_IR_CON)
					{
							setSn_IR(0, Sn_IR_CON);/*Sn_IR??0??1*/
          }
        
					len=getSn_RX_RSR(0);/*len?????????*/
					if(len>0)
					{
            recv(0,buffer,len);/*W5500????Sever???*/
            send(0,buffer,len);/*W5500?Server????*/
          }
					break;
				case SOCK_CLOSE_WAIT:/*socket??????*/
					break;
				case SOCK_CLOSED:/*socket??*/
					socket(0,Sn_MR_TCP,anyport++,Sn_MR_ND);/*??socket0?????*/
					break;
			}
		}
  }
#endif

    etimer_set(&et_blink, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT();

  }
  PROCESS_END();
}

PROCESS_THREAD(uartRecv_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();

  while(1)//ぁ饜疏杻薹
  {
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
  }
  
  PROCESS_END();
}