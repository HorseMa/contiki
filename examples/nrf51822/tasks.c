#include "contiki.h"
#include "tasks.h"
//#include "bsp.h"
#include "list.h"
#include <string.h>
#include "radio_config.h"
#include "nrf51.h"
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