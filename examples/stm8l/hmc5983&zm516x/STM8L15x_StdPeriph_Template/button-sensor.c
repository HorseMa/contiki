/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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

/*
 * This file contains ISRs: Keep it in the HOME bank.
 */
//#include "dev/port.h"
#include "button-sensor.h"
#include "sensors.h"
//#include "dev/watchdog.h"
#include "stm8l15x_it.h"
/*---------------------------------------------------------------------------*/
static struct timer debouncetimer;
/*---------------------------------------------------------------------------*/
/* Button 1 - SmartRF and cc2531 USB Dongle */
/*---------------------------------------------------------------------------*/
static int
value_b1(int type)
{
  type;
  return BUTTON_READ(1) || !timer_expired(&debouncetimer);
}
/*---------------------------------------------------------------------------*/
static int
status_b1(int type)
{
  switch(type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return 1;//BUTTON_IRQ_ENABLED(1);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
configure_b1(int type, int value)
{
  switch(type) {
  case SENSORS_HW_INIT:
    //BUTTON_IRQ_ON_PRESS(1);
    //BUTTON_FUNC_GPIO(1);
    //BUTTON_DIR_INPUT(1);
    return 1;
  case SENSORS_ACTIVE:
    //if(value) {
      //if(!BUTTON_IRQ_ENABLED(1)) {
        //timer_set(&debouncetimer, 0);
        //BUTTON_IRQ_FLAG_OFF(1);
        //BUTTON_IRQ_ENABLE(1);
     // }
    //} else {
      //BUTTON_IRQ_DISABLE(1);
    //}
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/* ISRs */
/*---------------------------------------------------------------------------*/
/* avoid referencing bits, we don't call code which use them */

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  DISABLE_INTERRUPTS();
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  EXTI_ClearITPendingBit(EXTI_IT_Pin7);
  /* This ISR is for the entire port. Check if the interrupt was caused by our
   * button's pin. */
  //if(BUTTON_IRQ_CHECK(1)) {
    //if(timer_expired(&debouncetimer)) {
    //  timer_set(&debouncetimer, CLOCK_SECOND / 8);
      sensors_changed(&button_sensor);
    //}
  //}

  //BUTTON_IRQ_FLAG_OFF(1);

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
  ENABLE_INTERRUPTS();
}

/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_1_sensor, BUTTON_SENSOR, value_b1, configure_b1, status_b1);
SENSORS(&button_1_sensor);