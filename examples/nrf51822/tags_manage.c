#include "tags_manage.h"
#include "Types.h"
#include "dev_cfg.h"
#include <string.h>

#define MAX_TAGS_NUM     200

static int tags_cnt_2_4 = 0;
static int tags_cnt_433 = 0;
static int tags_index_2_4 = 0;
static int tags_index_433 = 0;

static uint8 tags_2_4[MAX_TAGS_NUM][9] = {0};
static uint8 tags_433[MAX_TAGS_NUM][9] = {0};

uint8 add_tags_2_4(uint8 *tag)
{
  uint16 i = 0;
  for(i = 0;i < tags_cnt_2_4;i++)
  {
    if(stDevCfg.tag_type == 1)
    {
      if((!memcmp(&tag[2],&tags_2_4[i][2],2)) && ((tag[4] & 0x0f) == (tags_2_4[i][4] & 0x0f)))
      {
            memcpy(tags_2_4[i],tag,5);
            break;
      }
    }
    else if(stDevCfg.tag_type == 2)
    {
      if(!memcmp(&tag[2],&tags_2_4[i][2],3))
      {
            memcpy(tags_2_4[i],tag,9);
            break;
      }
    }
    else
    {
      
    }
      
  }
  if(i == tags_cnt_2_4)
  {
    if(stDevCfg.tag_type == 1)
    {
      memcpy(tags_2_4[tags_index_2_4++],tag,5);
    }
    else if(stDevCfg.tag_type == 2)
    {
      memcpy(tags_2_4[tags_index_2_4++],tag,9);
    }
    else
    {
      
    }
    if(tags_index_2_4 > tags_cnt_2_4)
    {
      tags_cnt_2_4 = tags_index_2_4;
    }
    if(tags_index_2_4 >= MAX_TAGS_NUM)
    {
      tags_index_2_4 = 0;
    }
  }
  if(stDevCfg.tag_type == 1)
  {
    return 5;
  }
  else if(stDevCfg.tag_type == 2)
  {
    return 9;
  }
  else
  {
  }
}

uint16 get_tags_2_4(uint8 *buf,uint8 *cnt,uint8 limit)
{
  static int tags_index = 0;
  uint16 i = 0;
  uint16 len;
  if(limit == MAX_LEN)
  {
    *cnt = tags_cnt_2_4 - tags_index;
    for(i = tags_index;i < tags_cnt_2_4;i++)
    {    
      if(stDevCfg.tag_type == 1)
      {
        memcpy(&buf[i * 5],tags_2_4[i],5);
        len = 5 * (*cnt);
      }
      else if(stDevCfg.tag_type == 2)
      {
        memcpy(&buf[i * 9],tags_2_4[i],9);
        len = 9 * (*cnt);
      }
      else
      {
      }
    }
    if(*cnt == 0)
    {
      len = 0;
    }
    tags_index = 0;
    tags_index_2_4 = 0;
    tags_cnt_2_4 = 0;
  }
  else
  {
    if(stDevCfg.tag_type == 1)
    {
      *cnt = ((tags_cnt_2_4 - tags_index) > 11)?11:(tags_cnt_2_4 - tags_index);
      for(i = tags_index;i < (*cnt + tags_index);i++)
      {
        memcpy(&buf[(i - tags_index) * 5],tags_2_4[i],5);
      }
      len = *cnt * 5;
      tags_index += *cnt;
      if(tags_index >= tags_cnt_2_4)
      {
        tags_index = 0;
        tags_index_2_4 = 0;
        tags_cnt_2_4 = 0;
      }
    }
    else if(stDevCfg.tag_type == 2)
    {
      *cnt = ((tags_cnt_2_4 - tags_index) > 6)?6:(tags_cnt_2_4 - tags_index);
      for(i = tags_index;i < (*cnt + tags_index);i++)
      {
        memcpy(&buf[(i - tags_index) * 9],tags_2_4[i],9);
      }
      len = *cnt * 9;
      tags_index += *cnt;
      if(tags_index >= tags_cnt_2_4)
      {
        tags_index = 0;
        tags_index_2_4 = 0;
        tags_cnt_2_4 = 0;
      }
    }
    else
    {
      
    }
  }
  return len;
}

uint8 add_tags_433(uint8 *tag)
{
  uint16 i = 0;
  for(i = 0;i < tags_cnt_433;i++)
  {
    if(stDevCfg.tag_type == 1)
    {
      if((!memcmp(&tag[2],&tags_433[i][2],2)) && ((tag[4] & 0x0f) == (tags_433[i][4] & 0x0f)))
      {
            memcpy(tags_433[i],tag,5);
            break;
      }
    }
    else if(stDevCfg.tag_type == 2)
    {
      if(!memcmp(&tag[2],&tags_433[i][2],3))
      {
            memcpy(tags_433[i],tag,9);
            break;
      }
    }
    else
    {
      
    }
  }
  if(i == tags_cnt_433)
  {
    if(stDevCfg.tag_type == 1)
    {
      memcpy(tags_433[tags_index_433++],tag,5);
    }
    else if(stDevCfg.tag_type == 2)
    {
      memcpy(tags_433[tags_index_433++],tag,9);
    }
    else
    {
      
    }
    if(tags_index_433 > tags_cnt_433)
    {
      tags_cnt_433 = tags_index_433;
    }
    if(tags_index_433 >= MAX_TAGS_NUM)
    {
      tags_index_433 = 0;
    }
  }
  if(stDevCfg.tag_type == 1)
  {
    return 5;
  }
  else if(stDevCfg.tag_type == 2)
  {
    return 9;
  }
  else
  {
  }
}

uint16 get_tags_433(uint8 *buf,uint8 *cnt,uint8 limit)
{
  static int tags_index = 0;
  uint16 i = 0;
  uint16 len;
  if(limit == MAX_LEN)
  {
    *cnt = tags_cnt_433 - tags_index;
    for(i = tags_index;i < tags_cnt_433;i++)
    {    
      if(stDevCfg.tag_type == 1)
      {
        memcpy(&buf[i * 5],tags_433[i],5);
        len = 5 * (*cnt);
      }
      else if(stDevCfg.tag_type == 2)
      {
        memcpy(&buf[i * 9],tags_433[i],9);
        len = 9 * (*cnt);
      }
      else
      {
      }
    }
    if(*cnt == 0)
    {
      len = 0;
    }
    tags_index = 0;
    tags_index_433 = 0;
    tags_cnt_433 = 0;
  }
  else
  {
    if(stDevCfg.tag_type == 1)
    {
      *cnt = ((tags_cnt_433 - tags_index) > 11)?11:(tags_cnt_433 - tags_index);
      for(i = tags_index;i < (*cnt + tags_index);i++)
      {
        memcpy(&buf[(i - tags_index) * 5],tags_433[i],5);
      }
      len = *cnt * 5;
      tags_index += *cnt;
      if(tags_index >= tags_cnt_433)
      {
        tags_index = 0;
        tags_index_433 = 0;
        tags_cnt_433 = 0;
      }
    }
    else if(stDevCfg.tag_type == 2)
    {
      *cnt = ((tags_cnt_433 - tags_index) > 6)?6:(tags_cnt_433 - tags_index);
      for(i = tags_index;i < (*cnt + tags_index);i++)
      {
        memcpy(&buf[(i - tags_index) * 9],tags_433[i],9);
      }
      len = *cnt * 9;
      tags_index += *cnt;
      if(tags_index >= tags_cnt_433)
      {
        tags_index = 0;
        tags_index_433 = 0;
        tags_cnt_433 = 0;
      }
    }
    else
    {
      
    }
  }
  return len;
}