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
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;  					// ??3yía2????ùê±?ó×?±?o?±ê?? 
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;     			// ???ˉía2????ùê±?ó
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)  	// ía2????ùê±?ó×?±?o?
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
  static uint8 ip[4];
  static uint8 pc_ip[4]={10,51,11,177};/*??????IP??*/
  uint16_t anyport=0x8888;/*????????????*/
  uint16_t len=0;
  static unsigned char buffer[100];
  uint8 ret;
  uint8_t dhcpret=0;

  PROCESS_BEGIN();
  while(1)
  {
    gipo_init();
    spi_w5500_init();
    Reset_W5500();
    set_default(); 
    //set_network();
	//os_dly_wait(10);
    //GetNetConfig();
    init_dhcp_client();
    
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
    //PROCESS_WAIT_EVENT();
#if 1
    set_network();
    setRTR(2000);/*设置溢出时间值*/
    setRCR(3);/*设置最大重新发送次数*/
    getSIPR (ip);
    getSUBR(ip);
    getGAR(ip);
    while(1){
      ret = getSn_SR(SOCK_SERVER);
      //switch(getSn_SR(0))/*??socket0???*/
      {
        if(ret == SOCK_INIT)/*socket?????*/
        {
          connect(SOCK_SERVER, pc_ip ,0x8888);/*?TCP?????????????*/ 
        }
        if(ret == SOCK_ESTABLISHED)/*socket????*/
        {
          if(getSn_IR(SOCK_SERVER) & Sn_IR_CON)
          {
            setSn_IR(SOCK_SERVER, Sn_IR_CON);/*Sn_IR??0??1*/
          }
        
          len=getSn_RX_RSR(SOCK_SERVER);/*len?????????*/
          if(len>0)
          {
            recv(SOCK_SERVER,buffer,len);/*W5500????Sever???*/
            send(SOCK_SERVER,buffer,len);/*W5500?Server????*/
          }
        }
        if(ret == SOCK_CLOSE_WAIT)/*socket??????*/
        {
        }
        if(ret == SOCK_CLOSED)/*socket??*/
        {
          while(!socket(SOCK_SERVER,Sn_MR_TCP,anyport++,Sn_MR_ND));/*??socket0?????*/
        }
      }
  }
#endif

    //etimer_set(&et_blink, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT();

  }
  PROCESS_END();
}

PROCESS_THREAD(uartRecv_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();

  while(1)//匹配波特率
  {
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
  }
  
  PROCESS_END();
}