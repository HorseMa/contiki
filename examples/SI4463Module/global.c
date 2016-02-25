#include "global.h"
#include "contiki.h"
#include "lib/ringbuf.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <ctype.h> 
#include "bsp.h"

struct ringbuf uartRcvRingBuf;
struct ringbuf radioRcvRingBuf;

unsigned char uartRcv[128] = {0};
unsigned char radioRcv[128] = {0};

const st_ModuleCfg stModuleCfgInRom = 
{
  .uartbaudrate = 4,
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

const uint32_t uartBaudRateList[10] = {1200,2400,4800,9600,14400,19200,38400,56000,57600,115200};
const uint32_t airBaudRateList[8] = {1000,2000,5000,8000,10000,15000,20000,25000};
const UART1_WordLength_TypeDef uartWordLengthList[2] = {UART1_WORDLENGTH_8D,UART1_WORDLENGTH_9D};
const UART1_StopBits_TypeDef uartStopBitsList[4] = {UART1_STOPBITS_1,UART1_STOPBITS_0_5,UART1_STOPBITS_2,UART1_STOPBITS_1_5};
const UART1_Parity_TypeDef uartParityList[3] = {UART1_PARITY_NO,UART1_PARITY_EVEN,UART1_PARITY_ODD};

st_ModuleParam stModuleParam;

en_RadioState enRadioState;

unsigned char mode = 1;
process_event_t ev_uartSendOver;
process_event_t ev_uartRecvPkg;

int asciitoint(unsigned char ascii)
{
  if((ascii >= '0') && (ascii <= '9'))
    return (ascii - '0');
  if((ascii >= 'A') && (ascii <= 'F'))
    return (ascii - 'A');
  return 0;
}

/* 
 * 将字符转换为数值 
 * */  
int c2i(char ch)  
{  
        // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
        if(isdigit(ch))  
                return ch - 48;  
  
        // 如果是字母，但不是A~F,a~f则返回  
        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
                return -1;  
  
        // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
        // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
        if(isalpha(ch))  
                return isupper(ch) ? ch - 55 : ch - 87;  
  
        return -1;  
}  
  
/* 
 * 功能：将十六进制字符串转换为整型(int)数值 
 * */  
int hex2dec(char *hex,int len)  
{  
        int num = 0;  
        int temp;  
        int bits;  
        int i;  
          
        // 此例中 hex = "1de" 长度为3, hex是main函数传递的  
        //len = strlen(hex);  
  
        for (i=0, temp=0; i<len; i++, temp=0)  
        {  
                // 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
                // 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
                // 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
                temp = c2i( *(hex + i) );  
                // 总共3位，一个16进制位用 4 bit保存  
                // 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
                // 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
                // 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
                bits = (len - i - 1) * 4;  
                temp = temp << bits;  
  
                // 此处也可以用 num += temp;进行累加  
                num = num | temp;  
        }  
  
        // 返回结果  
        return num;  
}

void globalInit(void)
{
  uint8_t cfg[sizeof(st_ModuleCfg) + 7];
  eepromReadBytes((unsigned char *)0x4000,cfg,sizeof(st_ModuleCfg) + 7);

  if(*(uint32_t*)cfg == 0xffffffff)
  {
    stModuleParam.BaudRate = 9600;
    stModuleParam.WordLength = UART1_WORDLENGTH_8D;
    stModuleParam.StopBits = UART1_STOPBITS_1;
    stModuleParam.Parity = UART1_PARITY_NO;
    stModuleParam.airBaudRate = 1000;
    stModuleParam.airChannel = 0;
    stModuleParam.destAddr = 0xffff;
    stModuleParam.mode = 0;
  }
  else
  {
    stModuleParam.BaudRate = uartBaudRateList[cfg[7] - 1];
    stModuleParam.WordLength = uartWordLengthList[cfg[9] - 1];
    stModuleParam.StopBits = uartStopBitsList[cfg[10] - 1];
    stModuleParam.Parity = uartParityList[cfg[8] - 1];
    stModuleParam.airBaudRate = airBaudRateList[cfg[11] - 1];
    stModuleParam.airChannel = hex2dec((&cfg[12]), 2);
    stModuleParam.destAddr = hex2dec((&cfg[14]), 4);
    stModuleParam.mode = cfg[18] - 1;
  }
  
  enRadioState = RADIO_IN_IDLE;
  DISABLE_INTERRUPTS();
  ringbuf_init(&uartRcvRingBuf, uartRcv, 128);
  ringbuf_init(&radioRcvRingBuf, radioRcv, 128);
  ENABLE_INTERRUPTS();
}