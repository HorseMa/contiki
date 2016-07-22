#ifndef __AD_HOC_H__
#define __AD_HOC_H__
#include "contiki.h"
#include "Types.h"

#define WORK_MODE_CENTER        0
#define WORK_MODE_END_DEVICE    1



typedef struct pkg_formart
{
  uint8 cmd;
  uint16 src_addr;
  uint16 dest_addr;
  uint8 data[64 - 5];
}st_PkgFormat,*pst_PkgFormart;

typedef enum cmd
{
  enBeacon = 0x01,
  enJoinReq = 0x02,
  enJoinRsp = 0x03,
  enDataReq = 0x04,
  enDataRsp = 0x05
}en_cmd,*pen_cmd;

typedef enum net_center_state
{
  enBeaconSend = 0x01,
  enJoinReqWait = 0x02,
  enDataReqSend = 0x03,
  enDataRspWait = 0x04,
}en_NetCenterState,*pen_NetCenterState;

typedef enum net_end_dev_state
{
  enBeaconListen = 0x05,
  enJoinReqSend = 0x06,
  enJoinRspWait = 0x07,
  enDataReqWait = 0x08,
  enDataRspSend = 0x09,
}en_NetEndDevState,*pen_NetEndDevState;

//void adhocRoutine(process_event_t ev,void* data);
void adhocSetWorkMode(uint8 mode);
uint8 adhocGetWorkMode(void);
void funBeaconSend(uint8 *buf);
void funJoinRspSend(uint8 *buf,uint16 dev_id);
void funDataReqSend(uint8* buf,uint16 dev_id);
void funJoinReqSend(uint16 dev_id,uint8* buf);

#endif