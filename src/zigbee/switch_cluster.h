#ifndef _SWITCH_CLUSTER_H_
#define _SWITCH_CLUSTER_H_

#include "tl_common.h"
#include "zb_common.h"
#include "zcl_include.h"

#include "endpoint.h"
#include "base_components/button.h"
#include "custom_zcl/zcl_onoff_configuration.h"

#define ZCL_ATTR_COUNT 10

#define MULTI_PRESS_CNT_TO_RESET    10
#define MULTI_PRESS_BOTH            252
#define MULTI_PRESS_BOTH_RELEASE    250
#define MULTI_PRESS_BOTH_HOLD       254

#define MULTISTATE_NOT_PRESSED      0
#define MULTISTATE_PRESS            1
#define MULTISTATE_HOLD             128
#define MULTISTATE_DOUBLE_PRESS     2
#define MULTISTATE_TRIPLE_PRESS     3
#define MULTISTATE_QUADRUPLE_PRESS  4
#define MULTISTATE_QUINTUPLE_PRESS  5
#define MULTISTATE_BOTH_RELEASE     200
#define MULTISTATE_BOTH_PRESS       201
#define MULTISTATE_BOTH_HOLD        202

typedef struct
{
  u8            switch_idx;
  u8            endpoint;
  u8            action;
  u8            both_press_action;
  u8            both_hold_action;
  u8            relay_mode;
  u8            relay_index;
  u8            binded_mode;
  button_t *    button;
  zclAttrInfo_t attr_infos[ZCL_ATTR_COUNT];
  u16           multistate_state;
  zclAttrInfo_t multistate_attr_infos[4];
} zigbee_switch_cluster;

typedef struct
{
  u8  action;
  u8  both_press_action;
  u8  both_hold_action;
  u8  relay_mode;
  u8  relay_index;
  u16 button_long_press_duration;
  u8 button_mode;
  u8 binded_mode;
} zigbee_switch_cluster_config;

void switch_cluster_add_to_endpoint(zigbee_switch_cluster *cluster, zigbee_endpoint *endpoint);

void switch_cluster_callback_attr_write_trampoline(u8 clusterId);

#endif
