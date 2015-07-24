/*
 * Copyright (c) 2011, George Oikonomou - <oikonomou@users.sourceforge.net>
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
 *         Override core/dev/button-sensor.h
 *
 * \author
 *         George Oikonomou - <oikonomou@users.sourceforge.net>
 */

#ifndef BUTTON_SENSOR_H_
#define BUTTON_SENSOR_H_

#include "contiki-conf.h"
#include "lib/sensors.h"
#include "stm8l15x.h"

#define BUTTON_SENSOR "Button"

/*
 * SmartRF Buttons
 * B1: P0_1, B2: Not Connected
 *
 * USB Dongle Buttons
 * B1: P1_2
 * B2: P1_3
 *
 */

#define BUTTON1_PORT 0
#define BUTTON1_PIN  1

#ifdef BUTTON_SENSOR_CONF_ON
#define BUTTON_SENSOR_ON BUTTON_SENSOR_CONF_ON
#endif /* BUTTON_SENSOR_CONF_ON */

#define button_sensor button_1_sensor
extern const struct sensors_sensor button_1_sensor;


#if BUTTON_SENSOR_ON
/* SmartRF */
/* Button 1: P0_1 - Port 0 ISR needed */
#define   BUTTON_SENSOR_ACTIVATE() button_sensor.configure(SENSORS_ACTIVE, 1)

#else /* BUTTON_SENSOR_ON */
#define   BUTTON_SENSOR_ACTIVATE()
#endif /* BUTTON_SENSOR_ON */

/* Define macros for buttons */
#define BUTTON_READ(b)           GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)//PORT_READ(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_FUNC_GPIO(b)      //PORT_FUNC_GPIO(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_DIR_INPUT(b)      //PORT_DIR_INPUT(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_ENABLED(b)    (GPIOB->CR2 & GPIO_Pin_7)//PORT_IRQ_ENABLED(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_CHECK(b)      EXTI_GetITStatus(EXTI_IT_PortB)//PORT_IRQ_CHECK(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_ENABLE(b)     (GPIOB->CR2 |= GPIO_Pin_7)//PORT_IRQ_ENABLE(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_DISABLE(b)    GPIOB->CR2 &= (uint8_t)(~(GPIO_Pin_7))//PORT_IRQ_DISABLE(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_FLAG_OFF(b)   EXTI_ClearITPendingBit(EXTI_IT_Pin7);//PORT_IRQ_FLAG_OFF(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_ON_PRESS(b)   EXTI_SetPinSensitivity(EXTI_Pin_7, EXTI_Trigger_Falling)//PORT_IRQ_EDGE_RISE(BUTTON##b##_PORT, BUTTON##b##_PIN)
#define BUTTON_IRQ_ON_RELEASE(b) EXTI_SetPinSensitivity(EXTI_Pin_7, EXTI_Trigger_Rising)//PORT_IRQ_EDGE_FALL(BUTTON##b##_PORT, BUTTON##b##_PIN)

#endif /* BUTTON_SENSOR_H_ */
