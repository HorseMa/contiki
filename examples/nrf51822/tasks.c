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
#include "radio.h"
#include "dev_cfg.h"

process_event_t ev_checkradio;
process_event_t ev_checkw5500;
process_event_t ev_radio_rcv;
process_event_t ev_2_4g_rcv;

unsigned char phone_num[11];
uint8 pc_ip[4]={10,51,11,177};/*??????IP??*/
static char str[100];
PROCESS(led_process,"led state");
PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");
PROCESS(si4463_process,"si4463");
PROCESS(ethernet_process,"ethernet");

AUTOSTART_PROCESSES(&uartRecv_process,&led_process,&ethernet_process,&si4463_process);

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
unsigned char tags_local[200][9];
int tags_index = 0;
//unsigned char packet[10];

PROCESS_THREAD(led_process, ev, data)
{ 
  static struct etimer et_blink;
  int i = 0;
  PROCESS_BEGIN();
  ev_2_4g_rcv = process_alloc_event();
  radio_configure();

  while(1)
  {
    PROCESS_WAIT_EVENT();
  }  
  PROCESS_END();
}
uint8 tag_update = 0;
extern int tags_cnt = 0;

PROCESS_THREAD(ethernet_process, ev, data)
{
  static struct etimer et_ethernet;
  uint8_t dhcpret = 0;
  //static uint8_t localip[4] = {10,51,11,172};
  uint8 ret;
  uint16_t len = 0;
  static uint16_t loop = 0;
  static uint8_t socket_buf[2048];
  static pst_EthPkgFormat pkg;
  uint8_t checksum;
  static uint8_t heart_cnt = 0;
  static uint8_t global_sn = 0;
  PROCESS_BEGIN();
  gipo_init();
  //w5500_irq_cfg();
  spi_w5500_init();
  nrf_gpio_pin_clear(W5500_RST);
  etimer_set(&et_ethernet, CLOCK_SECOND / 100);
  PROCESS_WAIT_EVENT();
  nrf_gpio_pin_set(W5500_RST);
  etimer_set(&et_ethernet, CLOCK_SECOND * 2);
  PROCESS_WAIT_EVENT();
  //read_cfg();
  set_default();
  while(1)
  {
#if 0
    init_dhcp_client();
    for(loop = 0;loop < 10;loop ++)
    {
      etimer_set(&et_ethernet, CLOCK_SECOND / 10);
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
    }
    if(loop >= 5)
    {
      continue;
    }
#endif
    //memcpy(ConfigMsg.lip,localip,4);
    //memcpy(ConfigMsg.lip,localip,4);
    set_network();
    //setRTR(2000);/*设置溢出时间值*/
    //setRCR(3);/*设置最大重新发送次数*/
    //getSIPR (ip);
    //getSUBR(ip);
    //getGAR(ip);
    while(1)
    {
      etimer_set(&et_ethernet, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
      heart_cnt ++;
      ret = getSn_SR(SOCK_SERVER);
      if(ret == SOCK_INIT)
      {
        connect(SOCK_SERVER, stDevCfg.server_ip ,stDevCfg.server_port);
      }
      if(ret == SOCK_ESTABLISHED)
      {
        if(getSn_IR(SOCK_SERVER) & Sn_IR_CON)
        {
          setSn_IR(SOCK_SERVER, Sn_IR_CON);
        }
        //send(SOCK_SERVER,(uint8*)tags_local,200 * 5);
        len=getSn_RX_RSR(SOCK_SERVER);/*len?????????*/
        pkg = (pst_EthPkgFormat)socket_buf;
        if(len>0)
        {
          memset(socket_buf,0,2048);
          recv(SOCK_SERVER,socket_buf,len);/*W5500????Sever???*/
          if(pkg->head != 0x05040302)
          {
            continue;
          }
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 4;loop ++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          if(*((uint8_t*)pkg + loop) != checksum)
          {
            continue;
          }
          if(pkg->cmd == 0x43)//配置参数
          {
            pkg->sn = global_sn ++;
            memcpy((uint8_t*)&stDevCfg,pkg->data,sizeof(st_DevCfg) - 2);
            memcpy(stDefaultCfg.server_ip,stDevCfg.server_ip,4);
            stDefaultCfg.server_port = stDevCfg.server_port;
            stDefaultCfg.dev_id = stDevCfg.dev_id;
            write_cfg();
            pkg->data[0] = 'o';
            pkg->data[0] = 'k';
            pkg->len = 9;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);
          }
          if(pkg->cmd == 0x44)//读取参数
          {
            pkg->sn = global_sn ++;
            memcpy(pkg->data,(uint8_t*)&stDevCfg,sizeof(st_DevCfg) - 2);
            pkg->len = 7 + sizeof(st_DevCfg) - 2;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);

          }
          if(pkg->cmd == 0x45)//控制指令
          {
            pkg->sn = global_sn ++;
            pkg->data[0] = 'o';
            pkg->data[0] = 'k';
            pkg->len = 9;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);

          }
          if(pkg->cmd == 0x46)//恢复出厂设置
          {
            pkg->sn = global_sn ++;
            pkg->data[0] = 'o';
            pkg->data[0] = 'k';
            pkg->len = 9;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);
            earase_cfg();
            read_cfg();
          }

        }
        else
        {
          if(tag_update)//给服务器更新标签数据
          {
            pkg->head = 0x05040302;
            pkg->dev_id = stDevCfg.dev_id;
            pkg->sn = global_sn ++;
            pkg->cmd = 0x41;
            DISABLE_INTERRUPTS();
            if(stDevCfg.tag_type)
            {
              pkg->len = 7 + 1 + 5 * tags_cnt;
              pkg->data[0] = tags_cnt;
              for(loop = 0;loop < tags_cnt;loop ++)
              {
                memcpy(pkg->data + 1 + (5 * loop),tags_local[loop],5);
              }
            }
            else
            {
              pkg->len = 7 + 1 + 9 * tags_cnt;
              pkg->data[0] = tags_cnt;
              for(loop = 0;loop < tags_cnt;loop ++)
              {
                memcpy(pkg->data + 1 + (9 * loop),tags_local[loop],9);
              }
            }
            memset(tags_local,0,sizeof(tags_local));
            tags_cnt = 0;
            tags_index = 0;
            tag_update = 0;
            ENABLE_INTERRUPTS();
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);
            heart_cnt = 0;
          }
          if((!tag_update) && (heart_cnt > 10))//在没有标签的情况下，每5秒发送一次心跳
          {
            pkg->head = 0x05040302;
            pkg->dev_id = stDevCfg.dev_id;
            pkg->sn = global_sn ++;
            pkg->cmd = 0x42;
            pkg->len = 7;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 4;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 4);
            heart_cnt = 0;
          }
        }
      }
      if(ret == SOCK_CLOSE_WAIT)
      {
        while(!socket(SOCK_SERVER,Sn_MR_TCP,stDefaultCfg.local_port,Sn_MR_ND));
      }
      if(ret == SOCK_CLOSED)
      {
        while(!socket(SOCK_SERVER,Sn_MR_TCP,stDefaultCfg.local_port,Sn_MR_ND));/*??socket0?????*/
      }
    }
  }
  PROCESS_END();

}

    
extern U8 buffer_433m[64];
uint8 net_flag = 0;
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
  static int i = 0;
  static int cnt = 0;

  PROCESS_BEGIN();
  memcpy(stDevCfg.server_ip,pc_ip,4);
  write_cfg();
  read_cfg();
  ev_checkw5500 = process_alloc_event();
  inet_addr_("10.51.11.177\0\n",pc_ip);
  while(1)
  {
    gipo_init();
    //w5500_irq_cfg();
    spi_w5500_init();
    nrf_gpio_pin_clear(W5500_RST);
    etimer_set(&et_blink, CLOCK_SECOND / 100);
    PROCESS_WAIT_EVENT();
    nrf_gpio_pin_set(W5500_RST);
    etimer_set(&et_blink, CLOCK_SECOND * 2);
    PROCESS_WAIT_EVENT();

    set_default(); 

    init_dhcp_client();
    //PROCESS_WAIT_EVENT_UNTIL(ev == ev_checkw5500);
    do{
      etimer_set(&et_blink, CLOCK_SECOND / 10);
      //for(i = 0;i < 200;i += 12)
      PROCESS_WAIT_EVENT();
      if(cnt > 50)
      {
        vRadio_StartTx_Variable_Packet((uint8*)tags_local[i],10 * 5);
        i += 10;
        if(i == 200)
          i = 0;
      }
      else
      {
        cnt ++;
      }

      //uart_put_string("get local addr\r\n");
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
    //vRadio_StartRX();
    while(1){
      etimer_set(&et_blink, CLOCK_SECOND / 5);
      PROCESS_WAIT_EVENT();
      if(ev == ev_checkw5500)
      {
        send(SOCK_SERVER,(uint8*)buffer_433m,10 * 5);
      }
      else
      {
        //continue;
      }
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
          net_flag  = 1;
          //NVIC_DisableIRQ(RADIO_IRQn);
          NRF_RADIO->INTENSET = RADIO_INTENSET_END_Disabled << RADIO_INTENSET_END_Pos;
          if(getSn_IR(SOCK_SERVER) & Sn_IR_CON)
          {
            setSn_IR(SOCK_SERVER, Sn_IR_CON);/*Sn_IR??0??1*/
          }
          //send(SOCK_SERVER,(uint8*)tags_local,200 * 5);
          len=getSn_RX_RSR(SOCK_SERVER);/*len?????????*/
          if(len>0)
          {
            recv(SOCK_SERVER,buffer,len);/*W5500????Sever???*/
            //send(SOCK_SERVER,str,strlen(str));/*W5500?Server????*/
          }
        }
        if(ret == SOCK_CLOSE_WAIT)
        {
          net_flag = 0;
          //NVIC_EnableIRQ(RADIO_IRQn);
          NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos;
          while(!socket(SOCK_SERVER,Sn_MR_TCP,anyport++,Sn_MR_ND));
        }
        if(ret == SOCK_CLOSED)// || (ret == SOCK_CLOSE_WAIT))/*socket??*/
        {
          net_flag = 0;
          //NVIC_EnableIRQ(RADIO_IRQn);
          NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos;
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

PROCESS_THREAD(si4463_process, ev, data)
{
  static struct etimer et_blink;
  PROCESS_BEGIN();
  ev_checkradio = process_alloc_event();

  spi_master_init(SPI1, SPI_MODE0, 0);
  vRadio_Init();
  si4463_irq_cfg();
  vRadio_StartRX();
  
  while(1)
  {
    /*etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();
    vRadio_StartTx_Variable_Packet("hello",5);*/
    PROCESS_WAIT_EVENT_UNTIL(ev == ev_checkradio);
    bRadio_Check_Tx_RX();
  }
  PROCESS_END();
}