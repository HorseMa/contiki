#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "list.h"
#include "contiki.h"
#include "packetsbuf.h"
#include "stm8s_conf.h"
#include "stm8s.h"

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
  unsigned char uartbaudrate;
  unsigned char uartparitybit;
  unsigned char uartdatabit;
  unsigned char uartstopbit;
  
  unsigned char airbaudrate;
  unsigned char airchannel[2];
  unsigned char destAddr[4];
  unsigned char mode;
}st_ModuleCfg,pst_ModuleCfg;

typedef struct{
  uint32_t BaudRate;
  UART1_WordLength_TypeDef WordLength;
  UART1_StopBits_TypeDef StopBits;
  UART1_Parity_TypeDef Parity;
  uint32_t airBaudRate;
  uint8_t airChannel;
  uint16_t destAddr;
  uint8_t mode;
}st_ModuleParam,pst_ModuleParam;

typedef enum{
  RADIO_IN_IDLE,
  RADIO_IN_RX,
  RADIO_IN_TX,
  RADIO_IN_SLEEP
}en_RadioState,pen_RadioState;

//extern st_ModuleCfg stModuleCfg;
extern const st_ModuleCfg stModuleCfgInRom;
extern st_ModuleParam stModuleParam;
extern const char *hardwareversion;
extern const char *softwareversion;
extern const char *sn ;
extern en_RadioState enRadioState;
extern process_event_t ev_checkradio;
extern process_event_t ev_radio_rcv;
extern process_event_t ev_uartSendOver;
extern process_event_t ev_uartRecvPkg;

extern struct ringbuf uartRcvRingBuf;
extern struct ringbuf radioRcvRingBuf;

extern unsigned char mode;

void globalInit(void);
PROCESS_NAME(checkradio_process);
PROCESS_NAME(radio_rcv_process);

PROCESS_NAME(uartSend_process);
PROCESS_NAME(uartRecv_process);


pst_Packet radioGetPktFromTxList(void);
void radioAddPktToTxList(pst_Packet pstPacket);
pst_Packet radioGetPktFromRxList(void);
void radioAddPktToRxList(pst_Packet pstPacket);
pst_Packet uartGetPktFromTxList(void);
void uartAddPktToTxList(pst_Packet pstPacket);
pst_Packet uartGetPktFromRxList(void);
void uartAddPktToRxList(pst_Packet pstPacket);

#endif