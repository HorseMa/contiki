#include "ad_hoc.h"
#include "dev_cfg.h"
#include "tasks.h"
#include "radio.h"
#include "tags_manage.h"

static uint8 work_mode = WORK_MODE_END_DEVICE;

void adhocSetWorkMode(uint8 mode)
{
  if(work_mode == mode)
  {
    return;
  }
  work_mode = mode;
}

uint8 adhocGetWorkMode(void)
{
  return work_mode;
}

void funBeaconSend(uint8 *buf)
{
  pst_PkgFormart pstPkgFormart = (pst_PkgFormart)buf;
  pstPkgFormart->cmd = enBeacon;
  pstPkgFormart->src_addr = stDevCfg.dev_id;
  pstPkgFormart->dest_addr = 0xFFFF;
  vRadio_StartTx_Variable_Packet(0,buf,64);
}

void funJoinRspSend(uint8 *buf,uint16 dev_id)
{
  pst_PkgFormart pstPkgFormarttx = (pst_PkgFormart)buf;
  pstPkgFormarttx->cmd = enJoinRsp;
  pstPkgFormarttx->src_addr = stDevCfg.dev_id;
  pstPkgFormarttx->dest_addr = dev_id;
  pstPkgFormarttx->data[0] = stDevCfg.net_433_channel;
  vRadio_StartTx_Variable_Packet(0,buf,64);
}

void funDataReqSend(uint8* buf,uint16 dev_id)
{
  pst_PkgFormart pstPkgFormart = (pst_PkgFormart)buf;
  pstPkgFormart->cmd = enDataReq;
  pstPkgFormart->src_addr = stDevCfg.dev_id;
  pstPkgFormart->dest_addr = dev_id;
  vRadio_StartTx_Variable_Packet(stDevCfg.net_433_channel,buf,64);
}

void funJoinReqSend(uint16 dev_id,uint8* buf)
{
  pst_PkgFormart pstPkgFormart = (pst_PkgFormart)buf;
  pstPkgFormart->cmd = enJoinReq;
  pstPkgFormart->dest_addr = dev_id;
  pstPkgFormart->src_addr = stDevCfg.dev_id;
  vRadio_StartTx_Variable_Packet(0,buf,64);
}

void funDataRspSend(void)
{
  
}
