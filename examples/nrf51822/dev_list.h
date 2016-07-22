#ifndef __DEV_LIST_H__
#define __DEV_LIST_H__

#include "Types.h"
typedef struct dev_list
{
  uint8 online;
  uint16 dev_id;
  uint16 time;
}st_DevList,*pst_DevList;

void dev_list_init(void);
uint8 dev_list_add_node(uint16 dev_id);
uint8 dev_list_get_node(uint16 *dev_id);
void dev_list_timer_update(void);
void dev_list_timer_reset(uint16 dev_id);

#endif