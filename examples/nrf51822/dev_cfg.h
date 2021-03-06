#ifndef __DEV_CFG_H__
#define __DEV_CFG_H__
#include "Types.h"
 typedef struct dev_cfg
 {
   uint8 rx_gain;
   uint8 local_ip[4];
   uint16 local_port;
   uint8 server_ip[4];
   uint16 server_port;
   uint8 tag_type;
   uint32 reserved1;
 }st_DevCfg,*pst_DevCfg;// 总大小保证是4的倍数
extern st_DevCfg stDevCfg;

typedef struct default_cfg
{
  uint16 dev_id;
  int8 rx_gain;
  uint8 local_ip[4];
  uint16 local_port;
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];
  uint8 server_ip[4];
  uint16 server_port;
  uint8 tag_type;
  uint32 active;
}st_DefaultCfg,*pst_DefaultCfg;
extern st_DefaultCfg stDefaultCfg;

typedef struct eth_pkg_format
{
  uint16 head;
  uint16 len;
  uint16 dev_id;
  uint8 cmd;
  uint8 sn;
  uint8 data[2048 - 10];
}st_EthPkgFormat,*pst_EthPkgFormat;

extern uint8 channel_433m;
void earase_cfg(void);
void flash_init(void);
void write_cfg(void);
void read_cfg(void);
#endif
