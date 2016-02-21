#include "global.h"
#include "contiki.h"
#include "lib/ringbuf.h"

struct ringbuf uartRcvRingBuf;
struct ringbuf radioRcvRingBuf;

unsigned char uartRcv[128] = {0};
unsigned char radioRcv[128] = {0};

st_ModuleCfg stModuleCfg;
const st_ModuleCfg stModuleCfgInRom = 
{
  .uartbaudrate = 1,
  .uartparitybit = 1,
  .uartdatabit = 1,
  .uartstopbit = 1,
  .airbaudrate = 1,
  .airchannel = {'0','0'},
  .destAddr = {'F','F','F','F'},
  .mode = 1,
};
const char *hardwareversion = "MJ-MD1U10010";
const char *softwareversion = "MJ-MD1U10010";
const char *sn = "201602160001";

en_RadioState enRadioState;

unsigned char mode = 1;
process_event_t ev_uartSendOver;
process_event_t ev_uartRecvPkg;

void globalInit(void)
{
  stModuleCfg.uartbaudrate = 0;
  stModuleCfg.uartdatabit = UART_DATA_BIT_8;
  stModuleCfg.uartparitybit = UART_PARITY_NO;
  stModuleCfg.uartstopbit = UART_STOPBITS_1;
  
  stModuleCfg.airbaudrate = 10000;
  stModuleCfg.airchannel[0] = 0;
  
  enRadioState = RADIO_IN_IDLE;
  DISABLE_INTERRUPTS();
  ringbuf_init(&uartRcvRingBuf, uartRcv, 128);
  ringbuf_init(&radioRcvRingBuf, radioRcv, 128);
  ENABLE_INTERRUPTS();
}