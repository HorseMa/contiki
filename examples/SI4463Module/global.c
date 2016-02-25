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
 * ���ַ�ת��Ϊ��ֵ 
 * */  
int c2i(char ch)  
{  
        // ��������֣��������ֵ�ASCII���ȥ48, ���ch = '2' ,�� '2' - 48 = 2  
        if(isdigit(ch))  
                return ch - 48;  
  
        // �������ĸ��������A~F,a~f�򷵻�  
        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
                return -1;  
  
        // ����Ǵ�д��ĸ���������ֵ�ASCII���ȥ55, ���ch = 'A' ,�� 'A' - 55 = 10  
        // �����Сд��ĸ���������ֵ�ASCII���ȥ87, ���ch = 'a' ,�� 'a' - 87 = 10  
        if(isalpha(ch))  
                return isupper(ch) ? ch - 55 : ch - 87;  
  
        return -1;  
}  
  
/* 
 * ���ܣ���ʮ�������ַ���ת��Ϊ����(int)��ֵ 
 * */  
int hex2dec(char *hex,int len)  
{  
        int num = 0;  
        int temp;  
        int bits;  
        int i;  
          
        // ������ hex = "1de" ����Ϊ3, hex��main�������ݵ�  
        //len = strlen(hex);  
  
        for (i=0, temp=0; i<len; i++, temp=0)  
        {  
                // ��һ�Σ�i=0, *(hex + i) = *(hex + 0) = '1', ��temp = 1  
                // �ڶ��Σ�i=1, *(hex + i) = *(hex + 1) = 'd', ��temp = 13  
                // �����Σ�i=2, *(hex + i) = *(hex + 2) = 'd', ��temp = 14  
                temp = c2i( *(hex + i) );  
                // �ܹ�3λ��һ��16����λ�� 4 bit����  
                // ��һ�Σ�'1'Ϊ���λ������temp���� (len - i -1) * 4 = 2 * 4 = 8 λ  
                // �ڶ��Σ�'d'Ϊ�θ�λ������temp���� (len - i -1) * 4 = 1 * 4 = 4 λ  
                // �����Σ�'e'Ϊ���λ������temp���� (len - i -1) * 4 = 0 * 4 = 0 λ  
                bits = (len - i - 1) * 4;  
                temp = temp << bits;  
  
                // �˴�Ҳ������ num += temp;�����ۼ�  
                num = num | temp;  
        }  
  
        // ���ؽ��  
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