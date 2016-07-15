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
  ble_flash_page_write(pg_num,(uint32_t*)&stDevCfg,(uint8_t)(sizeof(st_DevCfg) / 4) + (uint8_t)(sizeof(st_DefaultCfg) / 4));
}

void read_cfg(void)
{
  uint16 loop = 0;
  pst_DevCfg pstDevCfg;
  pst_DefaultCfg pstDefaultCfg;
  memset(temp,1,1024);
  ble_flash_page_erase(pg_num);
  //ble_flash_page_read(pg_num,(uint32_t*)temp,(uint8_t*)&count);
  memcpy(temp,(uint8*)addr,1024);
  for(loop = 0;loop < 1024;loop ++)
  {
    if(temp[loop] != 0xff)
    {
      break;
    }
  }
  pstDevCfg = (pst_DevCfg)addr;
  pstDefaultCfg = (pst_DefaultCfg)((uint8*)addr + sizeof(st_DevCfg));
  memcpy((uint8_t*)&stDevCfg,(uint8_t*)pstDevCfg,sizeof(st_DevCfg));
  memcpy((uint8_t*)&stDefaultCfg,(uint8_t*)pstDefaultCfg,sizeof(st_DefaultCfg));

  if(loop >= 1024)//初次上电，需要写入默认值
  {
    stDevCfg.dev_id = 0x58;
    stDevCfg.tag_type = 1;
    stDefaultCfg.dev_id = 0x58;
    stDefaultCfg.local_ip[0] = 192;
    stDefaultCfg.local_ip[1] = 168;
    stDefaultCfg.local_ip[2] = 1;
    stDefaultCfg.local_ip[3] = 254;
    stDefaultCfg.local_port = 32100;
    /*stDevCfg.server_ip[0] = 192;
    stDevCfg.server_ip[1] = 168;
    stDevCfg.server_ip[2] = 1;
    stDevCfg.server_ip[3] = 253;
    stDevCfg.server_port = 32500;*/
    stDevCfg.server_ip[0] = 10;
    stDevCfg.server_ip[1] = 51;
    stDevCfg.server_ip[2] = 11;
    stDevCfg.server_ip[3] = 177;
    stDevCfg.server_port = 0x8888;

    stDefaultCfg.server_ip[0] = 192;
    stDefaultCfg.server_ip[1] = 168;
    stDefaultCfg.server_ip[2] = 1;
    stDefaultCfg.server_ip[3] = 253;
    stDefaultCfg.server_port = 32500;
    write_cfg();
  }
  //pstDevCfg = (pst_DevCfg)temp;
  //pstDefaultCfg = (pst_DefaultCfg)(temp + sizeof(st_DevCfg));
}