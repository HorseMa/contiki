#include "contiki.h"
#include "tasks.h"
//#include "bsp.h"
#include "list.h"
#include <string.h>
process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

unsigned char phone_num[11];

PROCESS(led_process,"led state");
PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&led_process,&uartRecv_process);

PROCESS_THREAD(led_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
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

  while(1)//∆•≈‰≤®Ãÿ¬ 
  {
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
  }
  
  PROCESS_END();
}