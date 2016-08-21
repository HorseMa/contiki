#ifndef _BPS_H_
#define _BPS_H_

#define W5500_CS 07
#define W5500_INT 03
#define W5500_RST 02
#define LED3    27
#define LED4    27
#define RX_PIN_NUMBER  12
#define TX_PIN_NUMBER  11
//#define CTS_PIN_NUMBER 6
//#define RTS_PIN_NUMBER 7
#define SI4463_INT      26
#define SI4463_CEN      21
#define HWFC           false
	
void gipo_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
void si4463_irq_cfg(void);
void w5500_irq_cfg(void);

#endif
