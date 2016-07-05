#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "bps.h"
#include "spi_master_config.h" // This file must be in the application folder


void gipo_init(void)
{
	nrf_gpio_cfg_output(W5500_CS);
	nrf_gpio_pin_set(W5500_CS);
	nrf_gpio_cfg_output(W5500_RST);
	nrf_gpio_pin_set(W5500_RST);
	nrf_gpio_cfg_output(SPI_PSELSCK0);
	nrf_gpio_pin_set(SPI_PSELSCK0);
	nrf_gpio_cfg_output(SPI_PSELMOSI0);
	nrf_gpio_pin_set(SPI_PSELMOSI0);
        nrf_gpio_cfg_input(SPI_PSELMISO0,GPIO_PIN_CNF_PULL_Disabled);
	//nrf_gpio_pin_set(SPI_PSELMISO0);
	nrf_gpio_cfg_output(SPI_PSELSS0);
	nrf_gpio_pin_set(SPI_PSELSS0);
        nrf_gpio_cfg_output(LED3);
	nrf_gpio_pin_set(LED3);
        nrf_gpio_cfg_output(LED4);
	nrf_gpio_pin_set(LED4);
}

void led_on(void)
{
	//nrf_gpio_pin_clear(LED_1);
}

void led_off(void)
{
	//nrf_gpio_pin_set(LED_1);
}

void led_toggle(void)
{
	//nrf_gpio_pin_toggle(LED_1);
}
