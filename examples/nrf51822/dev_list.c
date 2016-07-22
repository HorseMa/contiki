#include "dev_list.h"

static st_DevList stDevList[30] = {0};
void dev_list_init(void)
{
  memset(&stDevList,0,sizeof(st_DevList));
}

uint8 dev_list_add_node(uint16 dev_id)
{
  uint8 loop = 0;
  uint8 add_index = 0xff;
  for(loop = 0;loop < 30;loop ++)
  {
    if(stDevList[loop].online == 0)
    {
      add_index = loop;
    }
    if(stDevList[loop].dev_id == dev_id)
    {
      stDevList[loop].online = 1;
      stDevList[loop].time = 0;
      break;
    }
  }
  if(loop >= 30)
  {
    if(add_index < 30)
    {
      stDevList[add_index].online = 1;
      stDevList[add_index].dev_id = dev_id;
      stDevList[add_index].time = 0;
    }
    else
    {
      return 0;
    }
  }
  return 1;
}

uint8 dev_list_get_node(uint16 *dev_id)
{
  static uint8 get_index = 0;
  for(uint8 loop = get_index;loop < 30;loop ++)
  {
    if(stDevList[loop].online == 1)
    {
      *dev_id = stDevList[loop].dev_id;
      get_index = loop + 1;
      return 1;
    }
  }
  get_index = 0;
  return 0;
}

void dev_list_timer_update(void)
{
  for(uint8 loop = 0;loop < 30;loop ++)
  {
    if(stDevList[loop].online == 1)
    {
      stDevList[loop].time ++;
      if(stDevList[loop].time > 50)
      {
        stDevList[loop].online = 0;
      }
    }
  }
}

void dev_list_timer_reset(uint16 dev_id)
{
  for(uint8 loop = 0;loop < 30;loop ++)
  {
    if(stDevList[loop].dev_id == dev_id)
    {
      stDevList[loop].time = 0;
      break;
    }
  }
}