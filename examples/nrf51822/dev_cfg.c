#include "dev_cfg.h"
#include "nrf51.h"
#include "ble_flash.h"
#include <string.h>
static uint32 pg_size;
static uint32 pg_num;  // Use last page in flash
uint32* addr;
st_DevCfg stDevCfg;
st_DefaultCfg stDefaultCfg;
static uint8 temp[1024];
static uint8 count;
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
  uint16 loop = 0;
  pst_DevCfg pstDevCfg;
  pst_DefaultCfg pstDefaultCfg;
  memset(temp,1,1024);
  ble_flash_page_erase(pg_num);
  ble_flash_page_read(pg_num,(uint32_t*)temp,(uint8_t*)&count);
  //memcpy(temp,(uint8*)addr,1024);
  /*for(loop = 0;loop < 1024;loop ++)
  {
    if(temp[loop] != 0xff)
    {
      break;
    }
  }*/
  //pstDevCfg = (pst_DevCfg)addr;
  //pstDefaultCfg = (pst_DefaultCfg)((uint8*)temp + sizeof(st_DevCfg));
  memcpy((uint8_t*)&stDevCfg,(uint8_t*)temp,sizeof(st_DevCfg));
  memcpy((uint8_t*)&stDefaultCfg,(uint8*)temp + sizeof(st_DevCfg),sizeof(st_DefaultCfg));
  if(count == 0)
  //if(loop >= 1024)//初次上电，需要写入默认值
  {
    stDevCfg.dev_id = 0x58;
    stDevCfg.tag_type = 1;
    stDefaultCfg.dev_id = 0x58;
#if 0
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
    
    stDevCfg.server_ip[0] = 192;
    stDevCfg.server_ip[1] = 168;
    stDevCfg.server_ip[2] = 1;
    stDevCfg.server_ip[3] = 253;
    stDevCfg.server_port = 32500;
    
    stDevCfg.net_433_channel = 0x01;
    
    stDefaultCfg.server_ip[0] = stDevCfg.server_ip[0];
    stDefaultCfg.server_ip[1] = stDevCfg.server_ip[1];
    stDefaultCfg.server_ip[2] = stDevCfg.server_ip[2];
    stDefaultCfg.server_ip[3] = stDevCfg.server_ip[3];
    stDefaultCfg.server_port = stDevCfg.server_port;
    
#else
    stDefaultCfg.local_ip[0] = 10;
    stDefaultCfg.local_ip[1] = 51;
    stDefaultCfg.local_ip[2] = 11;
    stDefaultCfg.local_ip[3] = 172;
    stDefaultCfg.local_port = 32100;
    
    stDefaultCfg.sub[0] = 255;
    stDefaultCfg.sub[1] = 255;
    stDefaultCfg.sub[2] = 255;
    stDefaultCfg.sub[3] = 0;
    
    stDefaultCfg.gw[0] = 10;
    stDefaultCfg.gw[1] = 51;
    stDefaultCfg.gw[2] = 11;
    stDefaultCfg.gw[3] = 1;
    
    stDevCfg.server_ip[0] = 10;
    stDevCfg.server_ip[1] = 51;
    stDevCfg.server_ip[2] = 11;
    stDevCfg.server_ip[3] = 177;
    stDevCfg.server_port = 32500;
    
    stDevCfg.net_433_channel = 0x01;
    
    stDefaultCfg.server_ip[0] = stDevCfg.server_ip[0];
    stDefaultCfg.server_ip[1] = stDevCfg.server_ip[1];
    stDefaultCfg.server_ip[2] = stDevCfg.server_ip[2];
    stDefaultCfg.server_ip[3] = stDevCfg.server_ip[3];
    stDefaultCfg.server_port = stDevCfg.server_port;

#endif


    write_cfg();
  }
  //pstDevCfg = (pst_DevCfg)temp;
  //pstDefaultCfg = (pst_DefaultCfg)(temp + sizeof(st_DevCfg));
}