/*
 * Copyright (c) 2009, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *         Implementation of the clock functions for the cc253x.
 *         Ported over from the cc243x original.
 * \author
 *         Zach Shelby (zach@sensinode.com) - original (cc243x)
 *         George Oikonomou - <oikonomou@users.sourceforge.net> - cc2530 port
 */
#include "stm8l15x_it.h"
//#include "sfr-bits.h"
#include "sys/clock.h"
#include "sys/etimer.h"
//#include "cc253x.h"
#include "sys/energest.h"
#include "uart_recv.h"

/* Sleep timer runs on the 32k RC osc. */
/* One clock tick is 7.8 ms */
#define TICK_VAL (32768/128)  /* 256 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Do NOT remove the absolute address and do NOT remove the initialiser here */
static unsigned long timer_value = 0;
unsigned char uarttimeout = 0;

static volatile clock_time_t count = 0; /* Uptime in ticks */
static volatile clock_time_t seconds = 0; /* Uptime in secs */
/*---------------------------------------------------------------------------*/
/**
 * Each iteration is ~1.0xy usec, so this function delays for roughly len usec
 */
void
clock_delay_usec(uint16_t len)
{
  DISABLE_INTERRUPTS();
  while(len--) {
    //ASM(nop);
  }
  ENABLE_INTERRUPTS();
}
/*---------------------------------------------------------------------------*/
/*
 * Wait for a multiple of ~8 ms (a tick)
 */
void
clock_wait(clock_time_t i)
{
  clock_time_t start;

  start = clock_time();
  while(clock_time() - start < (clock_time_t)i);
}
/*---------------------------------------------------------------------------*/
CCIF clock_time_t
clock_time(void)
{
  return count;
}
/*---------------------------------------------------------------------------*/
CCIF unsigned long
clock_seconds(void)
{
  return seconds;
}
/*---------------------------------------------------------------------------*/
/*
 * There is some ambiguity between TI cc2530 software examples and information
 * in the datasheet.
 *
 * TI examples appear to be writing to SLEEPCMD, initialising hardware in a
 * fashion semi-similar to cc2430
 *
 * However, the datasheet claims that those bits in SLEEPCMD are reserved
 *
 * The code here goes by the datasheet (ignore TI examples) and seems to work.
 */
void
clock_init(void)
{
  /* Make sure we know where we stand */
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC,ENABLE);
  CLK_DeInit();
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  /* Select HSE as system clock source */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);
  /* High speed external clock prescaler: 1*/
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSE)
  {}


  
  /* TIM2 init */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);


  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_Prescaler_8,TIM2_CounterMode_Up,15625);

  TIM2_SetAutoreload(15625);
  TIM2_ITConfig(TIM2_IT_Update,ENABLE);
  /* TIM2 counter enable */
  TIM2_Cmd(ENABLE);
  
  /* Enable general interrupts */
  enableInterrupts();
  
  /* Tickspeed 500 kHz for timers[1-4] */
  /* Initialize tick value */

  /* IEN0.STIE interrupt enable */
}
/*---------------------------------------------------------------------------*/
/* avoid referencing bits, we don't call code which use them */
#if CC_CONF_OPTIMIZE_STACK_SIZE
#pragma exclude bits
#endif
//INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
PROCESS_NAME(zigbee_comunication);
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
/*void
clock_isr(void) __interrupt(ST_VECTOR)*/
{
  DISABLE_INTERRUPTS();
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  TIM2_ClearITPendingBit(TIM2_IT_Update);
  /*
   * Read value of the ST0:ST1:ST2, add TICK_VAL and write it back.
   * Next interrupt occurs after the current time + TICK_VAL
   */
  if(stUartRcv.index > 0)
  {
    uarttimeout ++;
    if(uarttimeout > CLOCK_SECOND / 100)
    {
      stUartRcv.len = stUartRcv.index;
      stUartRcv.index = 0;
      process_post(&zigbee_comunication, PROCESS_EVENT_MSG, &stUartRcv);
    }
  }
  ++count;

  /* Make sure the CLOCK_CONF_SECOND is a power of two, to ensure
     that the modulo operation below becomes a logical and and not
     an expensive divide. Algorithm from Wikipedia:
     http://en.wikipedia.org/wiki/Power_of_two */
#if (CLOCK_CONF_SECOND & (CLOCK_CONF_SECOND - 1)) != 0
#error CLOCK_CONF_SECOND must be a power of two (i.e., 1, 2, 4, 8, 16, 32, 64, ...).
#error Change CLOCK_CONF_SECOND in contiki-conf.h.
#endif
  if(count % CLOCK_CONF_SECOND == 0) {
    ++seconds;
  }

  if(etimer_pending())
  {
    etimer_request_poll();
  }

  //STIF = 0; /* IRCON.STIF */
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
  ENABLE_INTERRUPTS();
}
/*---------------------------------------------------------------------------*/
