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
#include "nrf_gpio.h"
#include "util.h"
#include "app_uart.h"
#include <stdio.h>
#include <string.h>


process_event_t ev_checkradio;
process_event_t ev_radio_rcv;

unsigned char phone_num[11];
uint8 pc_ip[4]={10,51,11,177};/*??????IP??*/
static char str[100];
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
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )  
{  
    int  i;  
    char szTmp[3];  
  
    for( i = 0; i < nSrcLen; i++ )  
    {  
        sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );  
        memcpy( &sDest[i * 3], szTmp, 2 );  
        sDest[i * 3 + 2] = ' ';
    }  
    return ;  
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
    NRF_RADIO->EVENTS_READY = 0U; 				 // ê??t×?±? ê?·￠?￡ê?×a??íê3é  ±ê????    
    NRF_RADIO->TASKS_RXEN   = 1U;          // ê1?ü?óê?
    while(NRF_RADIO->EVENTS_READY == 0U)   // μè′y?óê?×?±?o?
    {
    }
				
    NRF_RADIO->EVENTS_END = 0U;  					 // ?áê?ê??t			
    NRF_RADIO->TASKS_START = 1U;           // ?aê?
    while(NRF_RADIO->EVENTS_END == 0U)     // ?óê?íê±?
    {
      etimer_set(&et_blink, CLOCK_SECOND / 100);
      PROCESS_WAIT_EVENT();
    }

				
    if (NRF_RADIO->CRCSTATUS == 1U)        // CRCD￡?éí¨1y  2??CRCD￡?é
    {
      //if((packet[0]==100)&&(packet[1]==0x23)&&(packet[2]==0x4e)&&(packet[3]==0x0)&&(packet[4]==0x0)) 
      {
        //nrf_gpio_pin_clear(LED3);         // oìμ?áá
        //etimer_set(&et_blink, CLOCK_SECOND / 5);
        //PROCESS_WAIT_EVENT();             
        //nrf_gpio_pin_set(LED3);           // oìμ??e
						 
        //simple_uart_putstring((const uint8_t *)&packet);
					}
      }
      memset(str,0,100);
      Hex2Str(packet,str,5);
      strcat(str,"\r\n");
      uart_put_string(str);
      NRF_RADIO->EVENTS_DISABLED = 0U;       // 1?±?ê??t  
      NRF_RADIO->TASKS_DISABLE   = 1U;       // 1?±?ê?·￠?÷
      while (NRF_RADIO->EVENTS_DISABLED == 0U)
      {
      }
  }
  
  PROCESS_END();
}


PROCESS_THREAD(read_gpio_process, ev, data)
{ 
  static struct etimer et_blink;
  static uint8 ip[4];
  static uint8 ipstr[50];
  uint16_t anyport=0x8888;/*????????????*/
  uint16_t len=0;
  static unsigned char buffer[100];
  uint8 ret;
  uint8_t dhcpret=0;

  PROCESS_BEGIN();
  inet_addr_("10.51.11.177\0\n",pc_ip);
  while(1)
  {
    gipo_init();
    spi_w5500_init();
    nrf_gpio_pin_clear(W5500_RST);
    etimer_set(&et_blink, CLOCK_SECOND / 100);
    PROCESS_WAIT_EVENT();
    nrf_gpio_pin_set(W5500_RST);
    etimer_set(&et_blink, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT();

    set_default(); 

    init_dhcp_client();
    
    do{
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
      uart_put_string("get local addr\r\n");
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
#if 1
    uart_put_string("get local addr ok\r\n");
    set_network();
    setRTR(2000);/*设置溢出时间值*/
    setRCR(3);/*设置最大重新发送次数*/
    getSIPR (ip);
    getSUBR(ip);
    getGAR(ip);
    while(1){
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();

      ret = getSn_SR(SOCK_SERVER);
      //switch(getSn_SR(0))/*??socket0???*/
      {
        if(ret == SOCK_INIT)/*socket?????*/
        {
          //uint8 szTmp
          //sprintf( szTmp, "%02X", (unsigned char) sSrc[i] ); 
          //uart_put_string();
          connect(SOCK_SERVER, pc_ip ,0x8888);/*?TCP?????????????*/ 
        }
        if(ret == SOCK_ESTABLISHED)/*socket????*/
        {
          if(getSn_IR(SOCK_SERVER) & Sn_IR_CON)
          {
            setSn_IR(SOCK_SERVER, Sn_IR_CON);/*Sn_IR??0??1*/
          }
          send(SOCK_SERVER,str,strlen(str));
          len=getSn_RX_RSR(SOCK_SERVER);/*len?????????*/
          if(len>0)
          {
            recv(SOCK_SERVER,buffer,len);/*W5500????Sever???*/
            //send(SOCK_SERVER,str,strlen(str));/*W5500?Server????*/
          }
        }
        if(ret == SOCK_CLOSE_WAIT)
        {
          while(!socket(SOCK_SERVER,Sn_MR_TCP,anyport++,Sn_MR_ND));
        }
        if(ret == SOCK_CLOSED)// || (ret == SOCK_CLOSE_WAIT))/*socket??*/
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

void uart_event_handle(app_uart_evt_t * p_event)
{
  
}

PROCESS_THREAD(uartRecv_process, ev, data)
{ 
  static struct etimer et_blink;
  static uint8 rxbuf[50],txbuf[50];
  app_uart_comm_params_t app_uart_comm_params;
  static app_uart_buffers_t app_uart_buffers;
  PROCESS_BEGIN();
  app_uart_comm_params.rx_pin_no = RX_PIN_NUMBER;
  app_uart_comm_params.tx_pin_no = TX_PIN_NUMBER;
  app_uart_comm_params.flow_control = APP_UART_FLOW_CONTROL_DISABLED;
  app_uart_comm_params.use_parity = false;
  app_uart_comm_params.baud_rate = UART_BAUDRATE_BAUDRATE_Baud38400;
  app_uart_buffers.rx_buf = rxbuf;
  app_uart_buffers.tx_buf = txbuf;
  app_uart_buffers.rx_buf_size = 50;
  app_uart_buffers.tx_buf_size = 50;
  app_uart_init(&app_uart_comm_params,&app_uart_buffers,uart_event_handle,APP_IRQ_PRIORITY_LOW,NULL);
  uart_put_string("NRF TEST!");
  while(1)//匹配波特率
  {
    nrf_gpio_pin_clear(LED3);         // oìμ?áá
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();             
    nrf_gpio_pin_set(LED3);
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();
  }
  
  PROCESS_END();
}

