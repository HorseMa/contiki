#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "bps.h"
#include "spi_master_config.h" // This file must be in the application folder
#include "radio.h"
#include "spi_master.h"



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
        nrf_gpio_cfg_input(W5500_INT,GPIO_PIN_CNF_PULL_Disabled);
	//nrf_gpio_pin_set(SPI_PSELMISO0);
	nrf_gpio_cfg_output(SPI_PSELSS0);
	nrf_gpio_pin_set(SPI_PSELSS0);
        nrf_gpio_cfg_output(SPI_PSELSCK1);
	nrf_gpio_pin_set(SPI_PSELSCK1);
	nrf_gpio_cfg_output(SPI_PSELMOSI1);
	nrf_gpio_pin_set(SPI_PSELMOSI1);
        nrf_gpio_cfg_input(SPI_PSELMISO1,GPIO_PIN_CNF_PULL_Disabled);
	//nrf_gpio_pin_set(SPI_PSELMISO1);
	nrf_gpio_cfg_output(SPI_PSELSS1);
	nrf_gpio_pin_set(SPI_PSELSS1);
        nrf_gpio_cfg_output(9);// wps
	nrf_gpio_pin_set(9);
        nrf_gpio_cfg_output(0);// rst
	nrf_gpio_pin_set(0);
        nrf_gpio_cfg_output(LED3);
	nrf_gpio_pin_set(LED3);
        nrf_gpio_cfg_output(LED4);
	nrf_gpio_pin_set(LED4);
        nrf_gpio_cfg_input(SI4463_INT,GPIO_PIN_CNF_PULL_Disabled);
        nrf_gpio_cfg_output(SI4463_CEN);
        nrf_gpio_pin_set(SI4463_CEN);
        spi_master_init(SPI1, SPI_MODE0, 0);
        vRadio_Init();
}

void w5500_irq_cfg(void)
{
  NVIC_EnableIRQ(GPIOTE_IRQn);
  //配置工作模式为event
  //选择一个引脚作为event来源
  //配置该event的触发源为翻转
  NRF_GPIOTE->CONFIG[1] =  (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos)
  | (W5500_INT << GPIOTE_CONFIG_PSEL_Pos)
  | (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
  //开启4路GPIOTE通道中的IN0通道的中断
  NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos;/** @brief Function for handling the GPIOTE interrupt which is triggered on pin 0 change.*/
}

void si4463_irq_cfg(void)
{
  NVIC_EnableIRQ(GPIOTE_IRQn);
  //配置工作模式为event
  //选择一个引脚作为event来源
  //配置该event的触发源为翻转
  NRF_GPIOTE->CONFIG[0] =  (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos)
  | (SI4463_INT << GPIOTE_CONFIG_PSEL_Pos)
  | (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
  //开启4路GPIOTE通道中的IN0通道的中断
  NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;/** @brief Function for handling the GPIOTE interrupt which is triggered on pin 0 change.*/
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
