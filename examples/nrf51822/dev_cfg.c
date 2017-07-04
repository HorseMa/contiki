#include "dev_cfg.h"
#include "nrf51.h"
#include "ble_flash.h"
#include <string.h>
static uint32 pg_size;
static uint32 pg_num;  // Use last page in flash
uint32* addr;
uint8 channel_433m;
st_DevCfg stDevCfg;
st_DefaultCfg stDefaultCfg;
static uint8 temp[1024];
static uint8 count;
const unsigned char blemac[8]@".MYSEG"={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
void flash_init(void)
{
  pg_size = NRF_FICR->CODEPAGESIZE;
  pg_num  = NRF_FICR->CODESIZE - 1;  // Use last page in flash
  addr = (uint32 *)(pg_size * pg_num);
}
void write_cfg(void)
{
  ble_flash_page_erase(pg_num);
  memcpy(temp,(uint8_t*)&stDevCfg,sizeof(st_DevCfg));
  memcpy(temp + sizeof(st_DevCfg),(uint8_t*)&stDefaultCfg,sizeof(st_DefaultCfg));
  ble_flash_page_write(pg_num,(uint32_t*)temp,(uint8_t)(sizeof(st_DevCfg) / 4) + (uint8_t)(sizeof(st_DefaultCfg) / 4));
}

void earase_cfg(void)
{
  ble_flash_page_erase(pg_num);
}
void read_cfg(void)
{
  //int len;
  uint16 loop = 0;
  pst_DevCfg pstDevCfg;
  pst_DefaultCfg pstDefaultCfg;
  //len = sizeof(st_DevCfg);
  //len = sizeof(st_DefaultCfg);

  memset(temp,1,1024);
  //ble_flash_page_erase(pg_num);
  ble_flash_page_read(pg_num,(uint32_t*)temp,(uint8_t*)&count);
  memcpy((uint8_t*)&stDevCfg,(uint8_t*)temp,sizeof(st_DevCfg));
  memcpy((uint8_t*)&stDefaultCfg,(uint8*)temp + sizeof(st_DevCfg),sizeof(st_DefaultCfg));
  if(count == 0)
  {
    if(*(uint16*)blemac == 0xffff)
    {
      stDefaultCfg.dev_id = 0x0058;
      stDefaultCfg.mac[4] = stDefaultCfg.dev_id >> 8;
      stDefaultCfg.mac[5] = stDefaultCfg.dev_id;
    }
    else
    {
      stDefaultCfg.dev_id = *(uint16*)blemac;
      stDefaultCfg.mac[4] = stDefaultCfg.dev_id >> 8;
      stDefaultCfg.mac[5] = stDefaultCfg.dev_id;
    }
    stDefaultCfg.rx_gain = 0;
    stDefaultCfg.active = 1;
#if 1
    stDefaultCfg.local_ip[0] = 192;
    stDefaultCfg.local_ip[1] = 168;
    stDefaultCfg.local_ip[2] = 1;
    stDefaultCfg.local_ip[3] = 254;
    stDefaultCfg.local_port = 32100;
    stDefaultCfg.sub[0] = 255;
    stDefaultCfg.sub[1] = 255;
    stDefaultCfg.sub[2] = 255;
    stDefaultCfg.sub[3] = 0;    
    stDefaultCfg.gw[0] = 192;
    stDefaultCfg.gw[1] = 168;
    stDefaultCfg.gw[2] = 1;
    stDefaultCfg.gw[3] = 1;
    stDefaultCfg.server_ip[0] = 192;
    stDefaultCfg.server_ip[1] = 168;
    stDefaultCfg.server_ip[2] = 1;
    stDefaultCfg.server_ip[3] = 253;
    stDefaultCfg.server_port = 32500;
    stDefaultCfg.tag_type = 2;
#else
    stDefaultCfg.local_ip[0] = 192;
    stDefaultCfg.local_ip[1] = 168;
    stDefaultCfg.local_ip[2] = 1;
    stDefaultCfg.local_ip[3] = 254;
    stDefaultCfg.local_port = 32100;
    stDefaultCfg.sub[0] = 255;
    stDefaultCfg.sub[1] = 255;
    stDefaultCfg.sub[2] = 255;
    stDefaultCfg.sub[3] = 0;    
    stDefaultCfg.gw[0] = 192;
    stDefaultCfg.gw[1] = 168;
    stDefaultCfg.gw[2] = 1;
    stDefaultCfg.gw[3] = 1;
    stDefaultCfg.server_ip[0] = 192;
    stDefaultCfg.server_ip[1] = 168;
    stDefaultCfg.server_ip[2] = 1;
    stDefaultCfg.server_ip[3] = 253;
    stDefaultCfg.server_port = 32500;
    stDefaultCfg.tag_type = 1;
#endif
    stDevCfg.rx_gain = stDefaultCfg.rx_gain;
    memcpy(stDevCfg.local_ip,stDefaultCfg.local_ip,4);
    memcpy(stDevCfg.gw,stDefaultCfg.gw,4);
    memcpy(stDevCfg.mac,stDefaultCfg.mac,8);
    stDevCfg.local_port = stDefaultCfg.local_port;
    memcpy(stDevCfg.server_ip,stDefaultCfg.server_ip,4);
    stDevCfg.server_port = stDefaultCfg.server_port;
    stDevCfg.tag_type = stDefaultCfg.tag_type;
    stDevCfg.reserved1 = 0;
    write_cfg();
  }
  channel_433m = stDefaultCfg.dev_id % 10;
}
