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
#include "lib/ringbuf.h"

process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

PROCESS(blink_process,"blink");
PROCESS(checkradio_process,"checkradio");
PROCESS(radio_rcv_process,"radioreceive");
PROCESS(uartSend_process,"uartSend");
PROCESS(uartRecv_process,"uartRecv");

AUTOSTART_PROCESSES(&blink_process, &checkradio_process,&radio_rcv_process,&uartSend_process,&uartRecv_process);

PROCESS_THREAD(blink_process, ev, data)
{ 
  static struct etimer et_blink;
  PROCESS_BEGIN();
  vRadio_StartRX();
  while(1) {
    etimer_set(&et_blink, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    /*pstPacket = pktbuf_alloc();
    if(!pstPacket)
    {
      for(unsigned char i = 0;i < 13;i ++)
        memcpy(&pstPacket->data[i * 5],"hello",5);
      pstPacket->len = 5 * 13;
      vRadio_StartTx_Variable_Packet(pstPacket);
    }*/
  }

  PROCESS_END();

} 


PROCESS_THREAD(checkradio_process, ev, data)
{ 
  PROCESS_BEGIN();
  ev_checkradio = process_alloc_event();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_checkradio);

    bRadio_Check_Tx_RX();
  }

  PROCESS_END();

} 

PROCESS_THREAD(radio_rcv_process, ev, data)
{ 
  PROCESS_BEGIN();
  ev_radio_rcv = process_alloc_event();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_radio_rcv);
    while(NULL)
    {
      /*if(memcmp(pstPacket->data,"hello",5))
        ledToggle();*/
    }
  }

  PROCESS_END();

} 


PROCESS_THREAD(uartSend_process, ev, data)
{ 
  PROCESS_BEGIN();
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    
  }
  
  PROCESS_END();
} 


PROCESS_THREAD(uartRecv_process, ev, data)
{
  static unsigned char cmd[50] = {0};
  int loop = 0;
  unsigned char cnt;
  PROCESS_BEGIN();
  uartSendBytes("hello",6);
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
    DISABLE_INTERRUPTS();
    cnt = ringbuf_elements(&uartRcvRingBuf);
    for(loop = 0;loop < cnt;loop ++)
    {
      cmd[loop] = ringbuf_get(&uartRcvRingBuf);
    }
    cmd[loop] = '\0';
    ENABLE_INTERRUPTS();
    if(mode == 1)
    {
      if(strstr((char const *)cmd,"get"))
      {
        uint8_t cfg[sizeof(st_ModuleCfg) + 7];
        eepromReadBytes((unsigned char *)0x4000,cfg,sizeof(st_ModuleCfg) + 7);
        memcpy(cmd,&stModuleCfgInRom,sizeof(st_ModuleCfg));
        if(*(uint32_t*)cfg != 0xffffffff)
        {
          memcpy(cmd,&cfg[7],18 - 6);
        }
        memcpy(cmd + sizeof(st_ModuleCfg),hardwareversion,strlen(hardwareversion));
        memcpy(cmd + sizeof(st_ModuleCfg) + strlen(hardwareversion),softwareversion,strlen(softwareversion));
        memcpy(cmd + sizeof(st_ModuleCfg) + strlen(hardwareversion) + strlen(softwareversion),sn,strlen(sn));
        memcpy(cmd + sizeof(st_ModuleCfg) + strlen(hardwareversion) + strlen(softwareversion) + strlen(sn),'\0',1);
        uartSendBytes(cmd,sizeof(st_ModuleCfg) + strlen(hardwareversion) + strlen(softwareversion) + strlen(sn) + 1);
      }
      else if(strstr((char const *)cmd,"factory reset"))
      {
        //eepromEarase();
        cmd[0] = 0xff;
        for(loop = 0;loop < 100;loop ++)
        eepromWriteBytes((unsigned char*)(0x4000 + loop),cmd,1);
      }
      else if(strstr((char const *)cmd,"cfg set"))
      {
        //eepromEarase();
        eepromWriteBytes((unsigned char*)0x4000,cmd,cnt);
      }
    }
    else
    {
      
    }
  }
  
  PROCESS_END();
} 