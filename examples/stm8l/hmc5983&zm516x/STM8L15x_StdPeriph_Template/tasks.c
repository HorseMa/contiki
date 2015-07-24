#include "contiki.h"
#include "button-sensor.h"
#include "tasks.h"
#include "stm8l15x_it.h"
#include "uart_recv.h"
#include "hmc5983.h"
#include  <math.h>
#include <stdlib.h>

#define PUTSTRING(x)

#define CFG_CMD_NONVOLATILE     {0XAB,0XBC,0XCD}
#define CFG_CMD_VOLATILE        {0XDE,0XDF,0XEF}

const unsigned char broadcastAddr[2] = {0xff,0xff};

enum nvparamoptcmd{
    enReadLoacalCfg = 0xd1,
    enSetChannelNv = 0xd2,
    enSearchNode = 0xd4,
    enGetRemoteInfo = 0xd5,
    enModifyCfg = 0xd6,
    enResetNode = 0xd9,
    enResetCfg = 0xda
};


enum temporaryparamoptcmd{
    enSetChannel = 0xd1,
    enSetDestAddr = 0xd2,
    enShowSrcAddr = 0xd3,
    enSetIoDirection = 0xd4,
    enReadIoStatus = 0xd5,
    enSetIoStatus = 0xd6,
    enReadAdcValue = 0xd7,
    enEnterSleepMode = 0xd8,
    enSetUnicastOrBroadcast = 0xd9,
    enReadNodeRssi = 0xda
};

struct dev_info{
    unsigned char devName[16];
    unsigned char devPwd[16];
    unsigned char devMode;
    unsigned char devChannel;
    unsigned char devPanid[2];
    unsigned char devLoacalNetAddr[2];
    unsigned char devLoacalIEEEAddr[8];
    unsigned char devDestNetAddr[2];
    unsigned char devDestIEEEAddr[8];
    unsigned char devReserve1;
    unsigned char devPowerLevel;
    unsigned char devRetryNum;
    unsigned char devTranTimeout;       // *10ms
    unsigned char devSerialRate;
    unsigned char devSerialDataB;
    unsigned char devSerialStopB;
    unsigned char devSerialParityB;
    unsigned char devReserve2;
};

static struct dev_info stDevInfo;
static unsigned char usart_buf[128];
static unsigned char const read_local_cfg[5] = {0xAB,0XBC,0XCD,0XD1,0X0A};
//static unsigned char const write_local_cfg[3 + 1 + 2 + 65 + 1] = {0xAB,0XBC,0XCD,0XD6,}; 
//static unsigned char const set_dest_addr[6] = {0xde,0xdf,0xef,0XD2,0X20,0x01};
static int x,y,z;
/*void usart_send_bytes(unsigned char *bytes, int len)
{
  int i;
  for(i = 0;i < len;i ++)
  USART_SendData8(USART1,bytes[i]);
}*/
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(zigbee_comunication, "zigbee communication process");
PROCESS(hmc5983_work, "hmc 5983 work");
AUTOSTART_PROCESSES(&hello_world_process,&zigbee_comunication);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_EXIT();
  PROCESS_END();
}

void uart_send_byte(int len,unsigned char *data)
{
  int i = 0;
  GPIO_WriteBit(GPIOD, GPIO_Pin_0, SET);
  for(i = 0;i < len;i ++)
  {
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) != SET);
    USART_SendData8(USART1,data[i]);
  }
  GPIO_WriteBit(GPIOD, GPIO_Pin_0, RESET);
}
PROCESS_THREAD(zigbee_comunication, ev, data)
{
  static struct etimer et;
  unsigned char sum = 0;
  struct st_UartRcv *pdata = NULL;
  int i = 0;
  PROCESS_BEGIN();
  
  GPIO_DeInit(GPIOB);
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);  //TXD
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);      //RXD
  //GPIO_Init(GPIOB,GPIO_Pin_6,GPIO_Mode_In_FL_IT);       // ACK
  GPIO_Init(GPIOB,GPIO_Pin_5,GPIO_Mode_Out_PP_High_Fast);       // RESET
  //GPIO_Init(GPIOB,GPIO_Pin_4,GPIO_Mode_In_FL_IT);       // STATE
  GPIO_Init(GPIOB,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Fast);       // WAKEUP
  GPIO_Init(GPIOB,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Fast);       // SLEEP
  GPIO_Init(GPIOB,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Fast);       // DEF
  GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);       // DIR
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_1, RESET);
  GPIO_WriteBit(GPIOB, GPIO_Pin_5, RESET);
  
  etimer_set(&et, CLOCK_SECOND / 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_5, SET);
  
  etimer_set(&et, CLOCK_SECOND / 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_1, SET);
  
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC,ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - Receive and transmit enabled
        - USART Clock disabled
  */
  USART_Init(USART1, (uint32_t)115200, USART_WordLength_8b, USART_StopBits_1,
                   USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
  //USART_ITConfig(USART1, USART_IT_TC, ENABLE);
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
  
  PUTSTRING("Now config ZM516X\r\n");

readLocalInfo:
  memcpy(usart_buf,read_local_cfg,sizeof(read_local_cfg));
  usart_buf[4] = usart_buf[0] + usart_buf[1] + usart_buf[2] + usart_buf[3];

  uart_send_byte(5,usart_buf);
   
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
  pdata = (struct st_UartRcv *)data;
 
  if((pdata->buf[0] == 0xAB) && (pdata->buf[1] == 0xBC) && (pdata->buf[2] == 0xCD) && (pdata->buf[3] == 0xD1))
  {
  }
  else
  {
    goto readLocalInfo;
  }
  memcpy(&stDevInfo,&pdata->buf[4],65);

  usart_buf[0] = 0xab;
  usart_buf[1] = 0xbc;
  usart_buf[2] = 0xcd;
  usart_buf[3] = enModifyCfg;
  usart_buf[4] = stDevInfo.devLoacalNetAddr[0];
  usart_buf[5] = stDevInfo.devLoacalNetAddr[1];
  stDevInfo.devLoacalNetAddr[0] = 0x00;
  stDevInfo.devLoacalNetAddr[1] = 0x03;
  //memset(stDevInfo.devLoacalNetAddr,0,2);
  stDevInfo.devDestNetAddr[0] = 0x00;
  stDevInfo.devDestNetAddr[1] = 0x00;
  stDevInfo.devChannel = 0x19;
  stDevInfo.devPanid[0] = 0x10;
  stDevInfo.devPanid[1] = 0x01;
  memcpy(&usart_buf[6],&stDevInfo,65);

  for(i = 0;i < (6 + 65);i++)
  {
      sum += usart_buf[i];
  }
  usart_buf[6 + 65] = sum;
  
  uart_send_byte(6 + 65 + 1,usart_buf);
  
  etimer_set(&et, CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  GPIO_WriteBit(GPIOB,GPIO_Pin_5,RESET);
  etimer_set(&et, CLOCK_SECOND / CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  GPIO_WriteBit(GPIOB,GPIO_Pin_5,SET);
  
  process_start(&hmc5983_work, NULL);
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
    pdata = (struct st_UartRcv *)data;
    if(memcmp(pdata->buf,"get",3) == 0)
    {
      *(int*)&usart_buf[0] = x;
      *(int*)&usart_buf[2] = y;
      *(int*)&usart_buf[4] = z;
      usart_buf[6] = 0xff - (usart_buf[0] + usart_buf[1] + usart_buf[2] + usart_buf[3] + usart_buf[4] + usart_buf[5]);
      uart_send_byte(7,usart_buf);
    }
    //etimer_set(&et, CLOCK_SECOND / 10);
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    //memcpy(usart_buf,"hello!\r\n",8);
    
    //uart_send_byte(8,usart_buf);
  }
  PROCESS_EXIT();
  PROCESS_END();
}

PROCESS_THREAD(hmc5983_work, ev, data)
{
  //static struct etimer et;
  struct sensors_sensor *sensor;
  //static unsigned char wbuf[7];
  //double angle;
  unsigned char ida,idb,idc;
  PROCESS_BEGIN();
  /* I2C DeInit */
  /* Enable I2C1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
  /* I2C DeInit */
  I2C_DeInit(I2C1);

  /* I2C configuration */
  I2C_Init(I2C1, 300000, 0x00, I2C_Mode_SMBusHost,
           I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

  /*!< Enable SMBus Alert interrupt */
  I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);

  /*!< LM75_I2C Init */
  I2C_Cmd(I2C1, ENABLE);
  
  GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_In_FL_No_IT);       // DRDY
  
  Init_HMC5983();
  
  PUTSTRING("Now config HMC5983\r\n");
  
  ida = Single_Read_HMC5983(0x0a);
  idb = Single_Read_HMC5983(0x0b);
  idc = Single_Read_HMC5983(0x0c);
  
  /*uart_send_byte(12,"chip id is :");
  uart_send_byte(1,&ida);
  uart_send_byte(1,&idb);
  uart_send_byte(1,&idc);
  uart_send_byte(2,"\r\n");*/
  GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_In_FL_IT);       // DRDY
  EXTI_SetPinSensitivity(EXTI_Pin_7, EXTI_Trigger_Falling);
  
  while(1)
  {
    //etimer_set(&et, CLOCK_SECOND / 10);
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event);
    sensor = (struct sensors_sensor *)data;
    if(sensor == &button_sensor) {
      //PRINTF("Button Press\n");
      //leds_toggle(LEDS_GREEN);
    
      Multiple_read_HMC5983();
      x=BUF[0] << 8 | BUF[1]; //Combine MSB and LSB of X Data output register
      z=BUF[2] << 8 | BUF[3]; //Combine MSB and LSB of Z Data output register
      y=BUF[4] << 8 | BUF[5]; //Combine MSB and LSB of Y Data output register
    
      /**(int*)&wbuf[0] = x;
      *(int*)&wbuf[2] = y;
      *(int*)&wbuf[4] = z;
      wbuf[6] = 0xff - (wbuf[0] + wbuf[1] + wbuf[2] + wbuf[3] + wbuf[4] + wbuf[5]);
      uart_send_byte(7,wbuf);*/
    }
  }
  PROCESS_EXIT();
  PROCESS_END();
}
