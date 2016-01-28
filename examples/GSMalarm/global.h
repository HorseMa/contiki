#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "list.h"
#include "contiki.h"
#include "packetsbuf.h"

#ifndef NULL
#define NULL      0
#endif

typedef enum{
  UART_PARITY_NO,
  UART_PARITY_EVEN,
  UART_PARITY_ODD
}en_UartParityBit,pen_UartParityBit;

typedef enum{
  UART_DATA_BIT_8,
  UART_DATA_BIT_9
}en_UartDataBit,pen_UartDataBit;

typedef enum{
  UART_STOPBITS_1,
  UART_STOPBITS_0_5,
  UART_STOPBITS_2,
  UART_STOPBITS_1_5
}en_UartStopBit,pen_UartStopBit;

typedef struct{
  unsigned long uartbaudrate;
  unsigned char uartdatabit;
  unsigned char uartparitybit;
  unsigned char uartstopbit;
  
  unsigned long airbaudrate;
  unsigned char airchannel;
  unsigned char airpower;
}st_ModuleCfg,pst_ModuleCfg;

typedef struct{
  unsigned char buf[50];
  unsigned char len;
}st_UartBuf,*pst_UartBuf;

PROCESS_NAME(uartRecv_process);
PROCESS_NAME(read_gpio_process);
#endif