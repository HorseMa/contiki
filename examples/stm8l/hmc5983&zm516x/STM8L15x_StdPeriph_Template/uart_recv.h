#ifndef __UART_RECV_H__
#define __UART_RECV_H__

struct st_UartRcv
{
  unsigned char len;
  unsigned char buf[128];
  unsigned char index;
};

extern struct st_UartRcv stUartRcv;
#endif