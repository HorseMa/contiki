#include "tasks.h"
#include "bps.h"
#include "list.h"
#include <string.h>
#include "radio_config.h"
#include "nrf51.h"
#include "spi_master.h"
#include "device.h"
#include "dhcp.h"
#include "config.h"
#include "sockutil.h"
#include "radio_config.h"
#include "w5500.h"
#include "socket.h"

void main(void)
{
  /* Hardware initialization */
  //eepromInit();
  
    uint8_t pc_ip[4]={192,168,1,101};/*??????IP??*/
    uint16_t anyport=3000;/*????????????*/
    uint16_t len=0;
    unsigned char buffer[100];
    clockInit();
    gipo_init();
    spi_w5500_init();
    Reset_W5500();
    set_default(); 
    //set_network();
	//os_dly_wait(10);
    //GetNetConfig();
    init_dhcp_client();
    uint8_t dhcpret=0;
    do{
      dhcpret = check_DHCP_state(SOCK_DHCP);
      if(dhcpret == DHCP_RET_NONE)
      {
      }
      if(dhcpret == DHCP_RET_TIMEOUT)
      {
      }
      if(dhcpret == DHCP_RET_UPDATE)
      {
        break;
      }
      if(dhcpret == DHCP_RET_CONFLICT)
      {
      }
    }while(dhcpret != DHCP_RET_UPDATE);
    while(1);
}