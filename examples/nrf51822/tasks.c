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
#include "nrf_drv_wdt.h"
#include "ad_hoc.h"
#include "tags_manage.h"
#include "dev_list.h"

process_event_t ev_433_tx_over;
process_event_t ev_433_rx_over;
process_event_t ev_checkw5500;
process_event_t ev_radio_rcv;
process_event_t ev_2_4g_rcv;
process_event_t ev_data_report_start;
process_event_t ev_data_report_end;

unsigned char phone_num[11];
uint8 pc_ip[4]={10,51,11,177};/*??????IP??*/
static uint8_t socket_buf[2048];
PROCESS(led_process,"led state");
PROCESS(read_gpio_process,"read_gpio");
PROCESS(uartRecv_process,"uartRecv");
PROCESS(si4463_center_process,"si4463");
PROCESS(si4463_enddev_process,"si4463");
PROCESS(ethernet_process,"ethernet");
PROCESS(data_report_process,"data_report");

AUTOSTART_PROCESSES(&uartRecv_process,&led_process,&data_report_process,&si4463_center_process,&si4463_enddev_process);

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
extern nrf_drv_wdt_channel_id m_channel_id;
PROCESS_THREAD(led_process, ev, data)
{ 
  static struct etimer et_blink;
  int i = 0;
  PROCESS_BEGIN();
  ev_2_4g_rcv = process_alloc_event();
  if(stDevCfg.tag_type == 1)
  {
    radio_configure();
  }
  else if(stDevCfg.tag_type == 2)
  {
    radio_configure2();
  }
  else
  {
    
  }

  while(1)
  {
    dev_list_timer_update();
    nrf_drv_wdt_channel_feed(m_channel_id);
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();  
  }  
  PROCESS_END();
}

static uint8_t global_sn = 0;
PROCESS_THREAD(ethernet_process, ev, data)
{
  static struct etimer et_ethernet;
  uint8_t dhcpret = 0;
  //static uint8_t localip[4] = {10,51,11,172};
  static uint8 ret;
  static uint16_t len = 0;
  static uint16_t loop = 0;
  
  static pst_EthPkgFormat pkg;
  uint8_t checksum;
  
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
  #if 0
  while(1)
  {
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
    while(stDefaultCfg.active == 0)
    {
      etimer_set(&et_ethernet, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    etimer_set(&et_ethernet, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT();
    ret = getSn_SR(SOCK_SERVER);
#if 1
    if(ret == SOCK_INIT)
    {
      //DISABLE_INTERRUPTS();
      // set destination IP
      //set_network();
      IINCHIP_WRITE( Sn_DIPR0(SOCK_SERVER), stDevCfg.server_ip[0]);
      IINCHIP_WRITE( Sn_DIPR1(SOCK_SERVER), stDevCfg.server_ip[1]);
      IINCHIP_WRITE( Sn_DIPR2(SOCK_SERVER), stDevCfg.server_ip[2]);
      IINCHIP_WRITE( Sn_DIPR3(SOCK_SERVER), stDevCfg.server_ip[3]);
      IINCHIP_WRITE( Sn_DPORT0(SOCK_SERVER), (uint8)((stDevCfg.server_port & 0xff00) >> 8));
      IINCHIP_WRITE( Sn_DPORT1(SOCK_SERVER), (uint8)(stDevCfg.server_port & 0x00ff));
      IINCHIP_WRITE( Sn_CR(SOCK_SERVER) ,Sn_CR_CONNECT);
      /* wait for completion */
      while ( IINCHIP_READ(Sn_CR(SOCK_SERVER) ) ) 
      {
        //etimer_set(&et_ethernet, CLOCK_SECOND / 100);
        //PROCESS_WAIT_EVENT();
      }
      etimer_set(&et_ethernet, CLOCK_SECOND / 20);
      PROCESS_WAIT_EVENT();
      loop = 0;
      while ( IINCHIP_READ(Sn_SR(SOCK_SERVER)) != SOCK_SYNSENT )
      {
        if(IINCHIP_READ(Sn_SR(SOCK_SERVER)) == SOCK_ESTABLISHED)
        {
          break;
        }
        if (getSn_IR(SOCK_SERVER) & Sn_IR_TIMEOUT)
        {
          IINCHIP_WRITE(Sn_IR(SOCK_SERVER), (Sn_IR_TIMEOUT));  // clear TIMEOUT Interrupt
          break;
        }
        etimer_set(&et_ethernet, CLOCK_SECOND / 50);
        PROCESS_WAIT_EVENT();
        loop ++;
        if(loop > 50)
        {
          //NVIC_SystemReset();
          /*nrf_gpio_pin_clear(W5500_RST);
          etimer_set(&et_ethernet, CLOCK_SECOND / 100);
          PROCESS_WAIT_EVENT();
          nrf_gpio_pin_set(W5500_RST);
          etimer_set(&et_ethernet, CLOCK_SECOND * 2);
          PROCESS_WAIT_EVENT();*/
          break;
        }
      }
      //ENABLE_INTERRUPTS();
    }
    
#else
    if(ret == SOCK_INIT)
    {
      DISABLE_INTERRUPTS();
      connect(SOCK_SERVER, stDevCfg.server_ip ,stDevCfg.server_port);
      ENABLE_INTERRUPTS();
    }
#endif
    if(ret == SOCK_ESTABLISHED)
    {
      adhocSetWorkMode(WORK_MODE_CENTER);
      //NVIC_DisableIRQ(RADIO_IRQn);
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
        if(pkg->head != 0xaa55)
        {
          continue;
        }
        if(pkg->dev_id != stDefaultCfg.dev_id)
        {
            continue;
        }
        checksum = 0;
        for(loop = 0;loop < pkg->len - 1 + 2;loop ++)
        {
          checksum += *((uint8_t*)pkg + loop);
        }
        if(*((uint8_t*)pkg + loop) != checksum)
        {
          continue;
        }
        if(pkg->cmd == 0x03)//配置参数
        {
          pkg->sn = global_sn ++;
          pkg->dev_id = stDefaultCfg.dev_id;
          stDevCfg.rx_gain = pkg->data[0];
          stDevCfg.local_ip[0] = pkg->data[1];
          stDevCfg.local_ip[1] = pkg->data[2];
          stDevCfg.local_ip[2] = pkg->data[3];
          stDevCfg.local_ip[3] = pkg->data[4];
          stDevCfg.local_port = pkg->data[5] + (pkg->data[6] * 256);
          stDevCfg.server_ip[0] = pkg->data[7];
          stDevCfg.server_ip[1] = pkg->data[8];
          stDevCfg.server_ip[2] = pkg->data[9];
          stDevCfg.server_ip[3] = pkg->data[10];
          stDevCfg.server_port = pkg->data[11] + (pkg->data[12] * 256);
          stDevCfg.tag_type = pkg->data[13];
          stDevCfg.reserved1 = pkg->data[14] + pkg->data[15] * 256;

          write_cfg();
          pkg->data[0] = 'o';
          pkg->data[1] = 'k';
          pkg->len = 7 + 2;
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 2;loop++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          *((uint8_t*)pkg + loop) = checksum;
          send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
          etimer_set(&et_ethernet, CLOCK_SECOND / 1);
          PROCESS_WAIT_EVENT();
          NVIC_SystemReset();
        }
        if(pkg->cmd == 0x04)//读取参数
        {
          pkg->sn = global_sn ++;
          pkg->dev_id = stDefaultCfg.dev_id;
          memcpy(pkg->data,(uint8_t*)&stDevCfg,sizeof(st_DevCfg));
          pkg->len = 7 + sizeof(st_DevCfg);
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 2;loop++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          *((uint8_t*)pkg + loop) = checksum;
          send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);

        }
        if(pkg->cmd == 0x05)//控制指令
        {
          pkg->sn = global_sn ++;
          pkg->dev_id = stDefaultCfg.dev_id;
          pkg->data[0] = 'o';
          pkg->data[1] = 'k';
          pkg->len = 7 + 2;
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 2;loop++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          *((uint8_t*)pkg + loop) = checksum;
          send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
        }
        if(pkg->cmd == 0x06)//恢复出厂设置
        {
          pkg->sn = global_sn ++;
          pkg->dev_id = stDefaultCfg.dev_id;
          pkg->data[0] = 'o';
          pkg->data[1] = 'k';
          pkg->len = 7 + 2;
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 2;loop++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          *((uint8_t*)pkg + loop) = checksum;
          send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
          earase_cfg();
          stDevCfg.rx_gain = stDefaultCfg.rx_gain;
          memcpy(stDevCfg.local_ip,stDefaultCfg.local_ip,4);
          stDevCfg.local_port = stDefaultCfg.local_port;
          memcpy(stDevCfg.server_ip,stDefaultCfg.server_ip,4);
          stDevCfg.server_port = stDefaultCfg.server_port;
          stDevCfg.tag_type = stDefaultCfg.tag_type;
          stDevCfg.reserved1 = 0;
          write_cfg();
          etimer_set(&et_ethernet, CLOCK_SECOND / 1);
          PROCESS_WAIT_EVENT();
          NVIC_SystemReset();
        }
      }
    }
    if(ret == SOCK_CLOSE_WAIT)
    {
      adhocSetWorkMode(WORK_MODE_END_DEVICE);
      //NVIC_EnableIRQ(RADIO_IRQn);
      while(!socket(SOCK_SERVER,Sn_MR_TCP,stDefaultCfg.local_port,Sn_MR_ND))
      {
        etimer_set(&et_ethernet, CLOCK_SECOND / 20);
        PROCESS_WAIT_EVENT();
      }
    }
    if(ret == SOCK_CLOSED)
    {
      adhocSetWorkMode(WORK_MODE_END_DEVICE);
      //NVIC_EnableIRQ(RADIO_IRQn);
      while(!socket(SOCK_SERVER,Sn_MR_TCP,stDefaultCfg.local_port,Sn_MR_ND))
      {
        etimer_set(&et_ethernet, CLOCK_SECOND / 20);
        PROCESS_WAIT_EVENT();
      }
    }
  }
  PROCESS_END();
}

PROCESS_THREAD(data_report_process, ev, data)
{
  static struct etimer et_blink;
  static pst_EthPkgFormat pkg;
  uint16 loop;
  uint8 checksum;
  PROCESS_BEGIN();
  ev_data_report_start = process_alloc_event();
  ev_data_report_end = process_alloc_event();
  pkg = (pst_EthPkgFormat)socket_buf;
  while(1)
  {
    while(adhocGetWorkMode() == WORK_MODE_END_DEVICE)
    {
      etimer_set(&et_blink, CLOCK_SECOND / 5);
      PROCESS_WAIT_EVENT();
    }
    etimer_set(&et_blink, CLOCK_SECOND * 5);
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)
    {
      pkg->head = 0xaa55;
      pkg->dev_id = stDefaultCfg.dev_id;
      pkg->sn = global_sn ++;
      pkg->cmd = 0x02;
      pkg->len = 7;
      checksum = 0;
      for(loop = 0;loop < pkg->len - 1 + 2;loop++)
      {
        checksum += *((uint8_t*)pkg + loop);
      }
      *((uint8_t*)pkg + loop) = checksum;
      send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
    }
    else
    {
      etimer_restart(&et_blink);
    }
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

void uart_event_handle(app_uart_evt_t * p_event)
{
  
}

static uint16 center_addr = 0xffff;
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
    if(adhocGetWorkMode() == WORK_MODE_CENTER)
    {
      nrf_gpio_pin_toggle(LED3);         // oìμ?áá
      etimer_set(&et_blink, CLOCK_SECOND / 5);
      PROCESS_WAIT_EVENT();             
    }
    else
    {
      if(center_addr == 0xffff)// offline
      {
        nrf_gpio_pin_toggle(LED3);         // oìμ?áá
        etimer_set(&et_blink, CLOCK_SECOND / 5);
        PROCESS_WAIT_EVENT();  
      }
      else// online
      {
        nrf_gpio_pin_toggle(LED3);         // oìμ?áá
        etimer_set(&et_blink, CLOCK_SECOND / 1);
        PROCESS_WAIT_EVENT();  
      }
    }
  }
  
  PROCESS_END();
}

PROCESS_THREAD(si4463_center_process, ev, data)
{
  static struct etimer et_blink;
  pst_PkgFormart pstPkgFormart;
  uint8 checksum = 0;
  uint16 loop = 0;
  static uint8 buf[64];
  static uint16 dev_id;
  PROCESS_BEGIN();
  ev_433_tx_over = process_alloc_event();
  ev_433_rx_over = process_alloc_event();

  //spi_master_init(SPI1, SPI_MODE0, 0);
  //vRadio_Init();
  si4463_irq_cfg();
  dev_list_init();
  while(1)
  {
    while(stDefaultCfg.active == 0)
    {
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    while(adhocGetWorkMode() == WORK_MODE_END_DEVICE)
    {
      etimer_set(&et_blink, CLOCK_SECOND / 1);
      PROCESS_WAIT_EVENT();
    }
    vRadio_StartRX(10);
    funBeaconSend(buf);
    etimer_set(&et_blink, CLOCK_SECOND / 5);
    PROCESS_WAIT_EVENT();
    while(1)
    {
      if(ev == ev_433_rx_over)
      {
        pstPkgFormart = (pst_PkgFormart)data;
        if(pstPkgFormart->cmd == enJoinReq)
        {
          if(dev_list_add_node(pstPkgFormart->src_addr))
          {
            funJoinRspSend(buf,pstPkgFormart->src_addr);
          }
        }
      }
      if(ev == PROCESS_EVENT_TIMER)
      {
        break;
      }
      PROCESS_WAIT_EVENT();
    }
    vRadio_StartRX(channel_433m);
    while(1)
    {
      
      if(!dev_list_get_node(&dev_id))
      {
        pst_EthPkgFormat pkg = (pst_EthPkgFormat)socket_buf;
        DISABLE_INTERRUPTS();
        pkg->len = get_tags_2_4(&pkg->data[1],&pkg->data[0],MAX_LEN);
        ENABLE_INTERRUPTS();
        if(pkg->len > 0)
        {
          pkg->len += (7 + 1);
          pkg->head = 0xaa55;
          pkg->dev_id = stDefaultCfg.dev_id;
          pkg->sn = global_sn ++;
          pkg->cmd = 0x01;
          checksum = 0;
          for(loop = 0;loop < pkg->len - 1 + 2;loop++)
          {
            checksum += *((uint8_t*)pkg + loop);
          }
          *((uint8_t*)pkg + loop) = checksum;
          send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
          process_post(&data_report_process,ev_data_report_start,NULL);
        }
        break;
      }
      funDataReqSend(buf,dev_id);//请求中继书据
      etimer_set(&et_blink, CLOCK_SECOND / 10);
      while(1)
      {
        //etimer_set(&et_blink, CLOCK_SECOND / 10);
        PROCESS_WAIT_EVENT();
        if(ev == PROCESS_EVENT_TIMER)
        {
          pst_EthPkgFormat pkg = (pst_EthPkgFormat)socket_buf;;
          //DISABLE_INTERRUPTS();
          pkg->len = get_tags_433(&pkg->data[1],&pkg->data[0],MAX_LEN);
          //ENABLE_INTERRUPTS();
          if(pkg->len > 0)
          {
            pkg->len += (7 + 1);
            pkg->head = 0xaa55;
            pkg->dev_id = dev_id;
            pkg->sn = global_sn ++;
            pkg->cmd = 0x01;
            checksum = 0;
            for(loop = 0;loop < pkg->len - 1 + 2;loop++)
            {
              checksum += *((uint8_t*)pkg + loop);
            }
            *((uint8_t*)pkg + loop) = checksum;
            send(SOCK_SERVER,(uint8_t*)pkg,pkg->len + 2);
            process_post(&data_report_process,ev_data_report_start,NULL);
          }
          break;
        }
        if(ev == ev_433_rx_over)
        {
          pstPkgFormart = (pst_PkgFormart)data;
          if((pstPkgFormart->cmd == enDataRsp) && (pstPkgFormart->src_addr == dev_id))
          {
            dev_list_timer_reset(dev_id);
            etimer_restart(&et_blink);
            uint8 *p = &pstPkgFormart->data[1];
            for(uint8 loop = 0;loop < pstPkgFormart->data[0];loop ++)
            {
              p += add_tags_433(p);
            }
          }
        }
      }
    }
  }
  PROCESS_END();
}
extern uint16 dest_dev;
extern uint8 send_flag;
PROCESS_THREAD(si4463_enddev_process, ev, data)
{
  static struct etimer et_blink;
  static uint8 buf[64];
  pst_PkgFormart pstPkgFormart;
  pst_PkgFormart pstPkgFormarttx = (pst_PkgFormart)buf;
  static uint8 loop1,loop2;
  PROCESS_BEGIN();
  vRadio_StartRX(10);
  
  /*buf[1] = '5';
  buf[2] = '8';
  buf[3] = '0';
  buf[4] = '0';
  dest_dev = (buf[1] - 0x30) * 16 + (buf[2] - 0x30);
  dest_dev += ((buf[3] - 0x30) * 16 + (buf[4] - 0x30)) * 256;
*/
  while(1)
  {
    while(1)
    {
      if(send_flag)
      {
        for(loop1 = 0;loop1 < 11;loop1 ++)
        {
          for(loop2 = 0;loop2 < 5;loop2 ++)
          {
            funFactoryResetSend(loop1,dest_dev,buf);
            //funInactiveSend(loop1,dest_dev,buf);
            //funActiveSend(loop1,dest_dev,buf);
            etimer_set(&et_blink, CLOCK_SECOND / 50);
            PROCESS_WAIT_EVENT();
          }
        }
        send_flag  = 0;
      }
      else
      {
        etimer_set(&et_blink, CLOCK_SECOND / 50);
        PROCESS_WAIT_EVENT();
      }
    }
    while(stDefaultCfg.active == 0)
    {
      etimer_set(&et_blink, CLOCK_SECOND / 2);
      PROCESS_WAIT_EVENT();
    }
    while(adhocGetWorkMode() == WORK_MODE_CENTER)
    {
      center_addr = 0xffff;
      etimer_set(&et_blink, CLOCK_SECOND / 5);
      PROCESS_WAIT_EVENT();
    }
    vRadio_StartRX(10);
    PROCESS_WAIT_EVENT();
    if(ev == ev_433_rx_over)
    {
      pstPkgFormart = (pst_PkgFormart)data;
      if(pstPkgFormart->cmd == enBeacon)
      {
        funJoinReqSend(pstPkgFormart->src_addr,buf);//发送入网请求
      }
    }
    else if(ev == ev_433_tx_over)
    {
      
    }
    else
    {
      continue;
    }
    
    while(1)
    {
      etimer_set(&et_blink, CLOCK_SECOND / 5);
      PROCESS_WAIT_EVENT();
      if(ev == PROCESS_EVENT_TIMER)
      {
        break;
      }
      if(ev == ev_433_rx_over)
      {
        pstPkgFormart = (pst_PkgFormart)data;
        if((pstPkgFormart->cmd == enJoinRsp) && (pstPkgFormart->dest_addr == stDefaultCfg.dev_id))
        {
          channel_433m = pstPkgFormart->data[0];
          //write_cfg();
          //vRadio_StartRX(channel_433m);
          center_addr = pstPkgFormart->src_addr;
        }
      }
    }
    vRadio_StartRX(channel_433m);
    etimer_set(&et_blink, CLOCK_SECOND * 30);
    while(center_addr != 0xffff)
    {
      PROCESS_WAIT_EVENT();
      if(ev == PROCESS_EVENT_TIMER)
      {
        center_addr = 0xffff;
        break;
      }
      if(ev == ev_433_rx_over)
      {
        pstPkgFormart = (pst_PkgFormart)data;
        if((pstPkgFormart->cmd == enDataReq) && (pstPkgFormart->dest_addr == stDefaultCfg.dev_id) && (pstPkgFormart->src_addr == center_addr))
        {
          //etimer_set(&et_blink, CLOCK_SECOND * 30);
          etimer_restart(&et_blink);
          pstPkgFormarttx = (pst_PkgFormart)buf;
          pstPkgFormarttx->cmd = enDataRsp;
          pstPkgFormarttx->dest_addr = center_addr;
          pstPkgFormarttx->src_addr = stDefaultCfg.dev_id;
          //DISABLE_INTERRUPTS();
          NVIC_DisableIRQ(RADIO_IRQn);
          get_tags_2_4(&pstPkgFormarttx->data[1],&pstPkgFormarttx->data[0],FIX_LEN);
          //ENABLE_INTERRUPTS();
          if(pstPkgFormarttx->data[0])
          {
            vRadio_StartTx_Variable_Packet(channel_433m,buf,64);
            //PROCESS_WAIT_EVENT();
          }
          else
          {
            NVIC_EnableIRQ(RADIO_IRQn);
          }
          
        }
      }
      if(ev == ev_433_tx_over)
      {
        //etimer_set(&et_blink, CLOCK_SECOND * 30);
        etimer_restart(&et_blink);
        pstPkgFormarttx = (pst_PkgFormart)buf;
        pstPkgFormarttx->cmd = enDataRsp;
        pstPkgFormarttx->dest_addr = center_addr;
        pstPkgFormarttx->src_addr = stDefaultCfg.dev_id;
        //DISABLE_INTERRUPTS();
        get_tags_2_4(&pstPkgFormarttx->data[1],&pstPkgFormarttx->data[0],FIX_LEN);
        //ENABLE_INTERRUPTS();
        if(pstPkgFormarttx->data[0])
        {
          vRadio_StartTx_Variable_Packet(channel_433m,buf,64);
          //PROCESS_WAIT_EVENT();
        }
        else
        {
          NVIC_EnableIRQ(RADIO_IRQn);
        }
      }
    }
    //etimer_stop(&et_blink);
  }
  PROCESS_END();
}
