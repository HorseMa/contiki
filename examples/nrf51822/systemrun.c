#include "systemrun.h"
#include "contiki.h"
#include "tasks.h"

void systemrun(void)
{
  while(systemrun_d == 0);
  process_start(&ethernet_process,0);
}