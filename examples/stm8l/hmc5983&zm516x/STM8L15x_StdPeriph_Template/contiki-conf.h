#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

//#include "8051def.h"
#include "sys/cc.h"
#include <string.h>
#include "stm8l15x.h"
/* Include Project Specific conf */
#ifdef PROJECT_CONF_H
#include "project-conf.h"
#endif /* PROJECT_CONF_H */

/* Time type. */
typedef unsigned short clock_time_t;
#define MAX_TICKS (~((clock_time_t)0) / 2)
/* Defines tick counts for a second. */
#define CLOCK_CONF_SECOND   128

//#include "models.h"
#define CCIF
#define CLIF

#define DISABLE_INTERRUPTS()  __disable_interrupt()
#define ENABLE_INTERRUPTS()   __enable_interrupt()
/*
 * Define this as 1 to poll the etimer process from within main instead of from
 * the clock ISR. This reduces the ISR's stack usage and may prevent crashes.
 */
#ifndef CLOCK_CONF_STACK_FRIENDLY
#define CLOCK_CONF_STACK_FRIENDLY 1
#endif


/* Energest Module */
#ifndef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON      0
#endif

/* More CODE space savings by turning off process names */
#define PROCESS_CONF_NO_PROCESS_NAMES 1

/*
 * USARTs:
 *   SmartRF RS232 -> USART0 / Alternative 1 (UART)
 *   SmartRF LCD   -> USART1 / Alternative 2 (SPI)
 */
#define UART_ON_USART     0

#define UART1_CONF_ENABLE 0

#ifndef UART0_CONF_ENABLE
#define UART0_CONF_ENABLE  0
#endif
#ifndef UART0_CONF_WITH_INPUT
#define UART0_CONF_WITH_INPUT 0
#endif

#ifndef UART0_CONF_HIGH_SPEED
#define UART0_CONF_HIGH_SPEED 0
#endif



/*
 * Sensors
 * It is harmless to #define XYZ 1
 * even if the sensor is not present on our device
 */
#ifndef BUTTON_SENSOR_CONF_ON
#define BUTTON_SENSOR_CONF_ON   1  /* Buttons */
#endif

/* Low Power Modes - We only support PM0/Idle and PM1 */
#ifndef LPM_CONF_MODE
#define LPM_CONF_MODE         0 /* 0: no LPM, 1: MCU IDLE, 2: Drop to PM1 */
#endif



#endif /* CONTIKI_CONF_H_ */
