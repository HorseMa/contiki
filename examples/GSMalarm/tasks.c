#include "contiki.h"
#include "tasks.h"
#include "bsp.h"
#include "list.h"
#include "radio.h"
#include "si446x_defs.h"
#include "packetsbuf.h"
#include <string.h>
#include "global.h"
#include "bsp.h"
process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&read_gpio_process,&uartRecv_process);

PROCESS_THREAD(read_gpio_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
  while(1)
  {
    etimer_set(&et_blink, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT();
    if((ev == PROCESS_EVENT_TIMER) && (1))
    {
      if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1)!=0)
      {
        // alarm report
      }
      else
      {
      }
    }
  }
  PROCESS_END();
}

PROCESS_THREAD(uartRecv_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
baudrate_matching:
  UART2_SendString("AT");
  etimer_set(&et_blink, CLOCK_SECOND / 2);
  PROCESS_WAIT_EVENT();
  if(ev == PROCESS_EVENT_TIMER)
  {
    goto baudrate_matching;
  }
  else if(ev == PROCESS_EVENT_MSG)
  {
    if(0 == memcmp(((pst_UartBuf)data)->buf,"AT",2))
    {
      // ������ƥ��ɹ�
    }
    else
    {
      goto baudrate_matching;
    }
  }
  else
  {
    goto baudrate_matching;
  }
close_echo:
  UART2_SendString("ATEO");//�رջ���
  etimer_set(&et_blink, CLOCK_SECOND / 2);
  PROCESS_WAIT_EVENT();
  if(ev == PROCESS_EVENT_TIMER)
  {
    goto close_echo;
  }
  else if(ev == PROCESS_EVENT_MSG)
  {
    if(0 == memcmp(((pst_UartBuf)data)->buf,"AT",2))
    {
      // �رջ��Գɹ�
    }
    else
    {
      goto close_echo;
    }
  }
  else
  {
    goto close_echo;
  }
set_msg_report_mode:
  UART2_SendString("AT+CMGF=1\r\n");//���ö���ϢΪ�ı�ģʽ
  etimer_set(&et_blink, CLOCK_SECOND / 2);
  PROCESS_WAIT_EVENT();
  if(ev == PROCESS_EVENT_TIMER)
  {
    goto set_msg_report_mode;
  }
  else if(ev == PROCESS_EVENT_MSG)
  {
    if(0 == memcmp(((pst_UartBuf)data)->buf,"AT",2))
    {
      // ���óɹ�
    }
    else
    {
      goto set_msg_report_mode;
    }
  }
  else
  {
    goto set_msg_report_mode;
  }
set_msg_report_auto:
  UART2_SendString("AT+CNMI=2,2,0,1,1\r\n");//�����¶���ϢURC�ϱ�ģʽ
  etimer_set(&et_blink, CLOCK_SECOND / 2);
  PROCESS_WAIT_EVENT();
  if(ev == PROCESS_EVENT_TIMER)
  {
    goto set_msg_report_auto;
  }
  else if(ev == PROCESS_EVENT_MSG)
  {
    if(0 == memcmp(((pst_UartBuf)data)->buf,"OK",2))
    {
      // ���óɹ�
    }
    else
    {
      goto set_msg_report_auto;
    }
  }
  else
  {
    goto set_msg_report_auto;
  }

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_MSG)
    {
      if(0 == memcmp(((pst_UartBuf)data)->buf,"369237shouji",12))
      {
      }
      else if(0 == memcmp(((pst_UartBuf)data)->buf,"369237kaien",11))
      {
        
      }
      else
      {
        
      }
    }
  }
  
  PROCESS_END();
}