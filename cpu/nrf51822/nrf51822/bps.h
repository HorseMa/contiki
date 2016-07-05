#ifndef _BPS_H_
#define _BPS_H_

#define W5500_CS 23
#define W5500_RST 30
#define LED3    00
#define LED4    01
#define RX_PIN_NUMBER  5
#define TX_PIN_NUMBER  4
#define CTS_PIN_NUMBER 6
#define RTS_PIN_NUMBER 7
#define HWFC           false
	
void gipo_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

#endif
