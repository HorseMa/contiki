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

unsigned char phone_num[11];
unsigned char alarm_enable = TRUE;

PROCESS(led_process,"led state");
PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&led_process,&uartRecv_process);

PROCESS_THREAD(led_process, ev, data)
{ 
  static struct etimer et_blink;
  static unsigned char revert = 0;
  PROCESS_BEGIN();
  while(1)
  {
    (revert)?GPIO_WriteHigh(GPIOD, GPIO_PIN_7):GPIO_WriteLow(GPIOD, GPIO_PIN_7);
    revert ^= 1;
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)
    {
      continue;
    }
    else
    {
      break;
    }
  }
  while(1)
  {
    (revert)?GPIO_WriteHigh(GPIOD, GPIO_PIN_7):GPIO_WriteLow(GPIOD, GPIO_PIN_7);
    etimer_set(&et_blink, CLOCK_SECOND / 2);
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
    etimer_set(&et_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT();
    if((ev == PROCESS_EVENT_TIMER) && (alarm_enable == TRUE))
    {
      if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1)!=0)
      {
        // alarm report
        UART2_SendString("ATD");
        UART2_SendBytes(phone_num,11);
        UART2_SendString(";\r\n");
        GPIO_WriteLow(GPIOC, GPIO_PIN_1);
        etimer_set(&et_blink, CLOCK_SECOND * 40);
        PROCESS_WAIT_EVENT();
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
  static pst_Packet pstPacket;
  static char *p = NULL;
  //static unsigned char buf[BUF_SIZE] = {0};
  PROCESS_BEGIN();
  eepromReadBytes(0x4010,&alarm_enable,1);
  eepromReadBytes(0x4011,phone_num,11);
  GPIO_WriteLow(GPIOB, GPIO_PIN_0);
  etimer_set(&et_blink, CLOCK_SECOND / 10);
  PROCESS_WAIT_EVENT();
  GPIO_WriteHigh(GPIOB, GPIO_PIN_0);
  etimer_set(&et_blink, CLOCK_SECOND * 6);
  PROCESS_WAIT_EVENT();
  while(1)//匹配波特率
  {
    if(ev != PROCESS_EVENT_MSG)
    {
      UART2_SendString("AT");
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    else
    {
      pstPacket = data;
      if(strstr((char const *)(((pst_Packet)data)->data),"AT"))
      {
        // 波特率匹配成功
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
        break;
      }
      else
      {
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
      }
    }
  }
  /*while(1)//关闭回显
  {
    if(ev != PROCESS_EVENT_MSG)
    {
      UART2_SendString("ATEO\r\n");
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    else
    {
      pstPacket = data;
      if(strstr((char const *)(((pst_Packet)data)->data),"OK"))
      {
        // 波特率匹配成功
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
        break;
      }
      else
      {
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
      }
    }
  }*/
  etimer_set(&et_blink, CLOCK_SECOND / 2);
  PROCESS_WAIT_EVENT();
  while(1)//设置短信为文本模式
  {
    if(ev != PROCESS_EVENT_MSG)
    {
      UART2_SendString("AT+CMGF=1\r\n");
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    else
    {
      pstPacket = data;
      if(strstr((char const *)(((pst_Packet)data)->data),"OK"))
      {
        // 波特率匹配成功
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
        break;
      }
      else
      {
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
      }
    }
  }
  while(1)//设置短信URC自动上报
  {
    if(ev != PROCESS_EVENT_MSG)
    {
      UART2_SendString("AT+CNMI=2,2,0,1,1\r\n");
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    else
    {
      pstPacket = data;
      if(strstr((char const *)(((pst_Packet)data)->data),"OK"))
      {
        // 波特率匹配成功
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
        break;
      }
      else
      {
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
      }
    }
  }
  /*while(1)//拨打电话
  {
    if(ev != PROCESS_EVENT_MSG)
    {
      UART2_SendString("ATD18701872013;\r\n");
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    else
    {
      pstPacket = data;
      if(strstr((char const *)(((pst_Packet)data)->data),"OK"))
      {
        // 拨打成功成功
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
        break;
      }
      else
      {
        pktbuf_free((pst_Packet)data);
        ev = PROCESS_EVENT_TIMER;
      }
    }
  }*/
  process_start(&read_gpio_process,NULL);
  process_post(&led_process,PROCESS_EVENT_MSG,NULL);
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_MSG)
    {
      pstPacket = data;
      if(p = strstr((const char *)(pstPacket->data),"369237shouji"))
      {
        memcpy(phone_num,p + 12,11);
        eepromWriteBytes(0x4011,phone_num,11);
      }
      if(p = strstr((const char *)(pstPacket->data),"369237kaien"))
      {
        if(*(p + 11) == 'Y')
        {
          alarm_enable = TRUE;
          eepromWriteBytes(0x4010,&alarm_enable,1);
        }
        else
        {
          alarm_enable = FALSE;
          eepromWriteBytes(0x4010,&alarm_enable,1);
        }
      }
      if(p = strstr((const char *)(pstPacket->data),"boda"))
      {
        if(alarm_enable == TRUE)
        {
          UART2_SendString("ATD");
          UART2_SendBytes(phone_num,11);
          UART2_SendString(";\r\n");
        }
      }
      else
      {
        
      }
      pktbuf_free((pst_Packet)data);
    }
  }
  
  PROCESS_END();
}