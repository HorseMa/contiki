#include "stm8s_conf.h"
#include "stm8s.h"
#include "bsp.h"
#include "global.h"
#include <string.h>


void eepromInit(void)
{
  FLASH_DeInit();
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}

void eepromWriteBytes(uint32_t eepromAddr,unsigned char *bytes,unsigned char cnt)
{
  unsigned char u8Loop;
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  for(u8Loop = 0;u8Loop < cnt;u8Loop ++)
  {
    FLASH_ProgramByte((uint32_t)eepromAddr + u8Loop, bytes[u8Loop]);
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void eepromReadBytes(uint32_t eepromAddr,unsigned char *bytes,unsigned char cnt)
{
  unsigned char u8Loop;
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  for(u8Loop = 0;u8Loop < cnt;u8Loop ++)
  {
    bytes[u8Loop] = FLASH_ReadByte(eepromAddr + (unsigned long)u8Loop);
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void clockInit(void)
{
  /* Turn on internal high speed clock and use it */
  CLK_HSICmd(ENABLE);
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, 
                        CLK_SOURCE_HSI, 
                        ENABLE, 
                        CLK_CURRENTCLOCKSTATE_ENABLE);
  CLK_LSICmd(DISABLE);
}

void gpioInit(void)
{
  
  GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT );//315模块报警信号输入脚
  GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST );//GPRS 复位
  GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW );//LED
  GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW );//ALARM
}

void timerInit(void)
{
  /* TIM2 initial */
  TIM2_TimeBaseInit(TIM2_PRESCALER_16,7812);
  TIM2_ITConfig( TIM2_IT_UPDATE , ENABLE);
  TIM2_SetCounter(0x0000);
  /*计时器打开*/
  TIM2_Cmd(ENABLE);
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
}

void spiInit(void)
{
  /* SPI initial*/
  //SPI_DeInit();
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST); // SPI_CS output high
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);  // SPI_SCLK output
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST); // SPI_MOSI output
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT); // SPI_MISO input
  //SPI_NSSInternalSoftwareCmd(ENABLE); // Use NSS pin
  
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER
  , SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, 
  SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT,0x07);
  SPI_Cmd(ENABLE);
}

void uartInit(void)
{
  UART2_DeInit();	/* 将寄存器的值复位 */
	
	/*
	 * 将UART2配置为：
	 * 波特率 = 115200
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 */


  UART2_Init((u32)115200, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, \
  UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);

  UART2_ITConfig(UART2_IT_RXNE, ENABLE); //开启接收中断
  UART2_Cmd(ENABLE);
}

void watchdog_start(void)
{
  
}

void watchdog_periodic(void)
{
  
}

void UART2_SendByte(u8 data)
{
  UART2_SendData8((unsigned char)data);
	
  /* 等待传输结束 */
  while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
}
void UART2_SendString(u8* Data)
{
  u16 i=0;
  //for(; i < len; i++)
  while(Data[i]!='\0')
  {
    UART2_SendByte(Data[i]);	/* 循环调用发送一个字符函数 */
    i++;
  }
}

void UART2_SendBytes(u8 *bytes,u8 len)//发送指定长度16进制数据
{
  u8 i=0;
  for(i=0;i<len;i++)
  UART2_SendByte(*(bytes+i));
}

