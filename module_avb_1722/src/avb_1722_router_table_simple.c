/**
 * \file avb_1722_router_table_simple.c
 * \brief A simple non-hashed routing table for the 1722 packet router
 */


#include <xs1.h>
#include <print.h>
#include "hwlock.h"
#include "string.h"
#include "avb_1722_router_table.h"

typedef struct avb_1722_router_table_entry_t
{
  int id[2];
  int link;
} avb_1722_router_table_entry_t;

static avb_1722_router_table_entry_t router_table[AVB_NUM_SINKS];
static hwlock_t table_lock;

void init_avb_1722_router_table_simple()
{
  int i;
  table_lock = hwlock_alloc();
  for(i=0;i<AVB_NUM_SINKS;i++) {
    router_table[i].id[0] = 0;
    router_table[i].id[1] = 0;
  }
}


#define STRINGIFY0(X) #X
#define STRINGIFY(X) STRINGIFY0(X)

int avb_1722_router_table_lookup_simple(int key0,
                                        int key1,
                                        unsigned int *link,
                                        unsigned int *sink_num)
{

  if (key0==0 && key1==0)
    return 0;
  hwlock_acquire(table_lock);
  for(int i=0;i<AVB_NUM_SINKS;i++) {
    __asm__(".xtaloop " STRINGIFY(AVB_NUM_SINKS) "\n");
    if (key0 == router_table[i].id[0] &&
        key1 == router_table[i].id[1]) {
      *sink_num = i;
      *link = router_table[i].link;
      hwlock_release(table_lock);
      return 1;
    }
  }
  hwlock_release(table_lock);
  return 0;
}



void avb_1722_router_table_add_entry_simple(int key0,
                                            int key1,
                                            int link,
                                            int sink_num)
{
  hwlock_acquire(table_lock);
  router_table[sink_num].id[0] = key0;
  router_table[sink_num].id[1] = key1;
  router_table[sink_num].link = link;
  hwlock_release(table_lock);

  return;
}
