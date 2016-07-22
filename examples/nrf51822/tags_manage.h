#ifndef __TAGS_MANAGE_H__
#define __TAGS_MANAGE_H__
#include "Types.h"

#define MAX_LEN 1
#define FIX_LEN 0
uint16 get_tags_2_4(uint8 *buf,uint8 *cnt,uint8 limit);
uint8 add_tags_2_4(uint8 *tag);
uint16 get_tags_433(uint8 *buf,uint8 *cnt,uint8 limit);
uint8 add_tags_433(uint8 *tag);

#endif