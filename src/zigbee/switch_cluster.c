#include "tl_common.h"
#include "zb_common.h"
#include "endpoint.h"
#include "switch_cluster.h"
#include "cluster_common.h"
#include "general.h"
#include "relay_cluster.h"
#include "custom_zcl/zcl_onoff_configuration.h"
#include "custom_zcl/zcl_multistate_input.h"
#include "zcl_include.h"
#include "base_components/relay.h"
#include "configs/nv_slots_cfg.h"

const u8  multistate_out_of_service = 0;
const u8  multistate_flags          = 0;
const u16 multistate_num_of_states  = 8;

extern zigbee_relay_cluster relay_clusters[];

void switch_cluster_on_button_press(zigbee_switch_cluster *cluster);
void switch_cluster_on_button_release(zigbee_switch_cluster *cluster);
void switch_cluster_on_button_long_press(zigbee_switch_cluster *cluster);
void switch_cluster_on_button_multi_press(zigbee_switch_cluster *cluster, u8 press_count);

zigbee_switch_cluster *switch_cluster_by_endpoint[10];

void switch_cluster_store_attrs_to_nv(zigbee_switch_cluster *cluster);
void switch_cluster_load_attrs_from_nv(zigbee_switch_cluster *cluster);
void switch_cluster_on_write_attr(zigbee_switch_cluster *cluster);

void switch_cluster_report_action(zigbee_switch_cluster *cluster);


status_t switch_cluster_callback_trampoline(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
  return ZCL_STA_SUCCESS;
}

void switch_cluster_callback_attr_write_trampoline(u8 clusterId)
{
  switch_cluster_on_write_attr(switch_cluster_by_endpoint[clusterId]);
}

void switch_cluster_add_to_endpoint(zigbee_switch_cluster *cluster, zigbee_endpoint *endpoint)
{
  switch_cluster_by_endpoint[endpoint->index] = cluster;
  cluster->endpoint = endpoint->index;
  switch_cluster_load_attrs_from_nv(cluster);

  cluster->button->on_press       = switch_cluster_on_button_press;
  cluster->button->on_release     = switch_cluster_on_button_release;
  cluster->button->on_long_press  = switch_cluster_on_button_long_press;
  cluster->button->on_multi_press = switch_cluster_on_button_multi_press;
  cluster->button->callback_param = cluster;

  SETUP_ATTR(0, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_TYPE, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ, cluster->button->mode); // only read because of zcl switch type is strict in lib
  SETUP_ATTR(1, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_ACTIONS, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->action);
  SETUP_ATTR(2, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_MODE, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->button->mode); // this is our implementation of switch type
  SETUP_ATTR(3, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_RELAY_MODE, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->relay_mode);
  SETUP_ATTR(4, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_RELAY_INDEX, ZCL_DATA_TYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->relay_index);
  SETUP_ATTR(5, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_BINDING_MODE, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->binded_mode);
  SETUP_ATTR(6, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_LONG_PRESS_DUR, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->button->long_press_duration_ms);
  SETUP_ATTR(7, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_MULTI_PRESS_DUR, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->button->multi_press_duration_ms);
  SETUP_ATTR(8, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_BOTH_PRESS_ACTION, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->both_press_action);
  SETUP_ATTR(9, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_BOTH_HOLD_ACTION, ZCL_DATA_TYPE_ENUM8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->both_hold_action);
  SETUP_ATTR(10, ZCL_ATTRID_ONOFF_CONFIGURATION_SWITCH_MOVE_RATE, ZCL_DATA_TYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, cluster->level_move->rate);

  // Configuration
  zigbee_endpoint_add_cluster(endpoint, 1, ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG);
  zcl_specClusterInfo_t *info_conf = zigbee_endpoint_reserve_info(endpoint);
  info_conf->clusterId           = ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG;
  info_conf->manuCode            = MANUFACTURER_CODE_NONE;
  info_conf->attrNum             = ZCL_ATTR_COUNT;
  info_conf->attrTbl             = cluster->attr_infos;
  info_conf->clusterRegisterFunc = zcl_onoff_configuration_register;
  info_conf->clusterAppCb        = switch_cluster_callback_trampoline;

  // Output ON OFF to bind to other devices
  zigbee_endpoint_add_cluster(endpoint, 0, ZCL_CLUSTER_GEN_ON_OFF);
  zcl_specClusterInfo_t *info = zigbee_endpoint_reserve_info(endpoint);
  info->clusterId           = ZCL_CLUSTER_GEN_ON_OFF;
  info->manuCode            = MANUFACTURER_CODE_NONE;
  info->attrNum             = 0;
  info->attrTbl             = NULL;
  info->clusterRegisterFunc = zcl_onOff_register;
  info->clusterAppCb        = switch_cluster_callback_trampoline;

  SETUP_ATTR_FOR_TABLE(cluster->multistate_attr_infos, 0, ZCL_ATTRID_MULTISTATE_INPUT_NUMBER_OF_STATES, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, multistate_num_of_states);
  SETUP_ATTR_FOR_TABLE(cluster->multistate_attr_infos, 1, ZCL_ATTRID_MULTISTATE_INPUT_OUT_OF_SERVICE, ZCL_DATA_TYPE_BOOLEAN, ACCESS_CONTROL_READ, multistate_out_of_service);
  SETUP_ATTR_FOR_TABLE(cluster->multistate_attr_infos, 2, ZCL_ATTRID_MULTISTATE_INPUT_PRESENT_VALUE, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, cluster->multistate_state);
  SETUP_ATTR_FOR_TABLE(cluster->multistate_attr_infos, 3, ZCL_ATTRID_MULTISTATE_INPUT_STATUS_FLAGS, ZCL_DATA_TYPE_BITMAP8, ACCESS_CONTROL_READ, multistate_flags);

  // Output
  zigbee_endpoint_add_cluster(endpoint, 1, ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC);
  zcl_specClusterInfo_t *info_multistate = zigbee_endpoint_reserve_info(endpoint);
  info_multistate->clusterId           = ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC;
  info_multistate->manuCode            = MANUFACTURER_CODE_NONE;
  info_multistate->attrNum             = 4;
  info_multistate->attrTbl             = cluster->multistate_attr_infos;
  info_multistate->clusterRegisterFunc = zcl_multistate_input_register;
  info_multistate->clusterAppCb        = NULL;
}

/// --- RELAY ACTIONS --- ///

// Perform the relay action for ON position (position 1 in ZCL docs)
void switch_cluster_relay_action_on(zigbee_switch_cluster *cluster) {
  zigbee_relay_cluster *relay_cluster = &relay_clusters[cluster->relay_index - 1];

  switch (cluster->action) {
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_ONOFF:
      relay_cluster_on(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_OFFON:
      relay_cluster_off(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SIMPLE:
      relay_cluster_toggle(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_SYNC:
      relay_cluster_toggle(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_OPPOSITE:
      relay_cluster_toggle(relay_cluster);
      break;
  }
}

// Perform the relay action for OFF position (position 2 in ZCL docs)
void switch_cluster_relay_action_off(zigbee_switch_cluster *cluster) {
  zigbee_relay_cluster *relay_cluster = &relay_clusters[cluster->relay_index - 1];

  switch (cluster->action) {
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_ONOFF:
      relay_cluster_off(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_OFFON:
      relay_cluster_on(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SIMPLE:
      relay_cluster_toggle(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_SYNC:
      relay_cluster_toggle(relay_cluster);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_OPPOSITE:
      relay_cluster_toggle(relay_cluster);
      break;
  }
}

/// --- BINDING SWITCHES --- ///

// Send OnOff command to binded device based on ON position (position 1 in ZCL docs)
void switch_cluster_binding_action_on(zigbee_switch_cluster *cluster) {
  if (!zb_isDeviceJoinedNwk()) return;

  zigbee_relay_cluster *relay_cluster = &relay_clusters[cluster->relay_index - 1];

  epInfo_t dstEpInfo;
  TL_SETSTRUCTCONTENT(dstEpInfo, 0);

  dstEpInfo.profileId   = HA_PROFILE_ID;
  dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
  switch (cluster->action) {
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_ONOFF:
      zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_OFFON:
      zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SIMPLE:
      zcl_onOff_toggleCmd(cluster->endpoint, &dstEpInfo, false);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_SYNC:
      if (relay_cluster->relay->on) {
        zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
      } else {
        zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
      }
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_OPPOSITE:
      if (relay_cluster->relay->on) {
        zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
      } else {
        zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
      }
      break;
  }
}
// Send OnOff command to binded device based on OFF position (position 2 in ZCL docs)
void switch_cluster_binding_action_off(zigbee_switch_cluster *cluster) {
  if (!zb_isDeviceJoinedNwk()) return;

  zigbee_relay_cluster *relay_cluster = &relay_clusters[cluster->relay_index - 1];

  epInfo_t dstEpInfo;
  TL_SETSTRUCTCONTENT(dstEpInfo, 0);

  dstEpInfo.profileId   = HA_PROFILE_ID;
  dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
  switch (cluster->action) {
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_ONOFF:
      zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_OFFON:
      zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SIMPLE:
      if (cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY &&
          cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY_INVERSE) {
        zcl_onOff_toggleCmd(cluster->endpoint, &dstEpInfo, false);
      }
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_SYNC:
      if (cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY &&
          cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY_INVERSE) {
        if (relay_cluster->relay->on) {
          zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
        } else {
          zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
        }
      }
      break;
    case ZCL_ONOFF_CONFIGURATION_SWITCH_ACTION_TOGGLE_SMART_OPPOSITE:
      if (cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY &&
          cluster->button->mode != ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY_INVERSE) {
        if (relay_cluster->relay->on) {
          zcl_onOff_offCmd(cluster->endpoint, &dstEpInfo, false);
        } else{
          zcl_onOff_onCmd(cluster->endpoint, &dstEpInfo, false);
        }
      }
      break;
  }
}
// Send Move command to binded device based on previous move postion
void switch_cluster_level_control(zigbee_switch_cluster *cluster) {
  if (!zb_isDeviceJoinedNwk()) return;

  epInfo_t dstEpInfo;
  TL_SETSTRUCTCONTENT(dstEpInfo, 0);

  dstEpInfo.profileId   = HA_PROFILE_ID;
  dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

  if (cluster->level_move->moveMode == LEVEL_MOVE_DOWN) {
    zcl_level_moveWithOnOffCmd(cluster->endpoint, &dstEpInfo, false, cluster->level_move);
    cluster->level_move->moveMode = LEVEL_MOVE_UP;
  } else {
    zcl_level_moveWithOnOffCmd(cluster->endpoint, &dstEpInfo, false, cluster->level_move);
    cluster->level_move->moveMode = LEVEL_MOVE_DOWN;
  }
}
// Send Stop command to binded device
void switch_cluster_level_stop(zigbee_switch_cluster *cluster) {
  if (!zb_isDeviceJoinedNwk()) return;

  epInfo_t dstEpInfo;
  TL_SETSTRUCTCONTENT(dstEpInfo, 0);

  dstEpInfo.profileId   = HA_PROFILE_ID;
  dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

  zcl_level_stopWithOnOffCmd(cluster->endpoint, &dstEpInfo, false, NULL);
}

/// --- SWITCH ACTIONS --- ///

void switch_cluster_on_button_press(zigbee_switch_cluster *cluster) {
  if (cluster->multistate_state == MULTISTATE_BOTH_PRESS) return; // never but avoid

  if (cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE ||
      cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE_INVERSE)
  { // Toggle does not support modes (RISE, SHORT, LONG)
    if (cluster->relay_mode != ZCL_ONOFF_CONFIGURATION_RELAY_MODE_DETACHED) {
      switch_cluster_relay_action_on(cluster);
    }
    switch_cluster_binding_action_on(cluster);
    return;
  }

  if (cluster->relay_mode == ZCL_ONOFF_CONFIGURATION_RELAY_MODE_RISE) {
    switch_cluster_relay_action_on(cluster);
  }
  if (cluster->binded_mode == ZCL_ONOFF_CONFIGURATION_BINDED_MODE_RISE) {
    switch_cluster_binding_action_on(cluster);
  }

  if (cluster->button->multi_press_cnt <= 1) { // only first press
    cluster->multistate_state = MULTISTATE_PRESS;
    switch_cluster_report_action(cluster);
  }
}

void switch_cluster_on_button_release(zigbee_switch_cluster *cluster) {
  if (cluster->multistate_state == MULTISTATE_BOTH_PRESS ||
      cluster->multistate_state == MULTISTATE_BOTH_HOLD ||
      cluster->multistate_state == MULTISTATE_BOTH_RELEASE) return; // never but avoid

  if (cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE ||
      cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE_INVERSE)
  { // Toggle does not support modes (RISE, SHORT, LONG)
    if (cluster->relay_mode != ZCL_ONOFF_CONFIGURATION_RELAY_MODE_DETACHED) {
       switch_cluster_relay_action_off(cluster);
    }
    switch_cluster_binding_action_off(cluster);
    return;
  }

  if (cluster->multistate_state == MULTISTATE_PRESS) { // release after press
    if (cluster->relay_mode == ZCL_ONOFF_CONFIGURATION_RELAY_MODE_SHORT) {
      switch_cluster_relay_action_on(cluster);
    }
    if (cluster->binded_mode == ZCL_ONOFF_CONFIGURATION_BINDED_MODE_SHORT) {
      switch_cluster_binding_action_on(cluster);
    }
  }

  if (cluster->multistate_state == MULTISTATE_HOLD) { // release after hold
    switch_cluster_level_stop(cluster);
  }

  cluster->multistate_state = MULTISTATE_RELEASE;
  switch_cluster_report_action(cluster);
}

void switch_cluster_on_button_long_press(zigbee_switch_cluster *cluster) {
  if (cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE ||
      cluster->button->mode == ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE_INVERSE) return;
  if (cluster->multistate_state == MULTISTATE_BOTH_PRESS ||
      cluster->multistate_state == MULTISTATE_BOTH_HOLD ||
      cluster->multistate_state == MULTISTATE_BOTH_RELEASE) return; // never but avoid

  if (cluster->relay_mode == ZCL_ONOFF_CONFIGURATION_RELAY_MODE_LONG) {
    switch_cluster_relay_action_on(cluster);
  }
  if (cluster->binded_mode == ZCL_ONOFF_CONFIGURATION_BINDED_MODE_LONG) {
    switch_cluster_binding_action_on(cluster);
  }
  switch_cluster_level_control(cluster);

  cluster->multistate_state = MULTISTATE_HOLD;
  switch_cluster_report_action(cluster);
}

void switch_cluster_on_button_multi_press(zigbee_switch_cluster *cluster, u8 press_count) {

  zigbee_relay_cluster *relay_cluster = &relay_clusters[cluster->relay_index - 1];

  if (cluster->multistate_state == MULTISTATE_BOTH_PRESS &&
      press_count == MULTI_PRESS_BOTH_HOLD)
  { // both hold after press
    switch (cluster->both_hold_action) {
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_HOLD_ACTION_ONOFF:
        relay_cluster_on(relay_cluster);
        break;
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_HOLD_ACTION_OFFON:
        relay_cluster_off(relay_cluster);
        break;
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_HOLD_ACTION_TOGGLE:
        relay_cluster_toggle(relay_cluster);
        break;
      default:
        break;
    }
  }

  if (cluster->multistate_state == MULTISTATE_BOTH_PRESS &&
      press_count == MULTI_PRESS_BOTH_RELEASE)
  { // both release after press
    switch (cluster->both_press_action) {
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_PRESS_ACTION_ONOFF:
        relay_cluster_on(relay_cluster);
        break;
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_PRESS_ACTION_OFFON:
        relay_cluster_off(relay_cluster);
        break;
      case ZCL_ONOFF_CONFIGURATION_SWITCH_BOTH_PRESS_ACTION_TOGGLE:
        relay_cluster_toggle(relay_cluster);
        break;
      default:
        break;
    }
  }

  switch (press_count) {
    case 1: // never
      cluster->multistate_state = MULTISTATE_PRESS;
      break;
    case 2:
      cluster->multistate_state = MULTISTATE_DOUBLE_PRESS;
      break;
    case 3:
      cluster->multistate_state = MULTISTATE_TRIPLE_PRESS;
      break;
    case 4:
      cluster->multistate_state = MULTISTATE_QUADRUPLE_PRESS;
      break;
    case 5:
      cluster->multistate_state = MULTISTATE_QUINTUPLE_PRESS;
      break;
    case MULTI_PRESS_CNT_TO_RESET:
      return factoryReset();
      break;
    case MULTI_PRESS_BOTH_RELEASE: // 250 BOTH_RELEASE
      cluster->multistate_state = MULTISTATE_RELEASE; // skip both_release
      break;
    case MULTI_PRESS_BOTH: // 252 BOTH
      cluster->multistate_state = MULTISTATE_BOTH_PRESS;
      break;
    case MULTI_PRESS_BOTH_HOLD: // 254 BOTH_HOLD
      cluster->multistate_state = MULTISTATE_BOTH_HOLD;
      break;
    default:
      cluster->multistate_state = MULTISTATE_PRESS;
      break;
  }

  switch_cluster_report_action(cluster);
}

/// --- NVRAM SECTION --- ///

void switch_cluster_on_write_attr(zigbee_switch_cluster *cluster)
{
  switch_cluster_store_attrs_to_nv(cluster);
}

zigbee_switch_cluster_config nv_config_buffer;

void switch_cluster_store_attrs_to_nv(zigbee_switch_cluster *cluster)
{
  nv_config_buffer.action                     = cluster->action;
  nv_config_buffer.both_press_action          = cluster->both_press_action;
  nv_config_buffer.both_hold_action           = cluster->both_hold_action;
  nv_config_buffer.relay_index                = cluster->relay_index;
  nv_config_buffer.relay_mode                 = cluster->relay_mode;
  nv_config_buffer.button_long_press_duration = cluster->button->long_press_duration_ms;
  nv_config_buffer.button_mode                = cluster->button->mode;
  nv_config_buffer.binded_mode                = cluster->binded_mode;
  nv_config_buffer.level_move_rate            = cluster->level_move->rate;

  nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_SWITCH_CLUSTER_DATA(cluster->switch_idx), sizeof(zigbee_switch_cluster_config), (u8 *)&nv_config_buffer);
}

void switch_cluster_load_attrs_from_nv(zigbee_switch_cluster *cluster)
{
  nv_sts_t st = nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_SWITCH_CLUSTER_DATA(cluster->switch_idx), sizeof(zigbee_switch_cluster_config), (u8 *)&nv_config_buffer);

  if (st != NV_SUCC)
  {
    return;
  }
  cluster->action                           = nv_config_buffer.action;
  cluster->both_press_action                = nv_config_buffer.both_press_action;
  cluster->both_hold_action                 = nv_config_buffer.both_hold_action;
  cluster->relay_index                      = nv_config_buffer.relay_index;
  cluster->relay_mode                       = nv_config_buffer.relay_mode;
  cluster->button->long_press_duration_ms   = nv_config_buffer.button_long_press_duration;
  cluster->button->mode                     = nv_config_buffer.button_mode;
  cluster->binded_mode                      = nv_config_buffer.binded_mode;
  cluster->level_move->rate                 = nv_config_buffer.level_move_rate;
}

void switch_cluster_report_action(zigbee_switch_cluster *cluster) {
  if (!zb_isDeviceJoinedNwk()) return;

  epInfo_t dstEpInfo;
  TL_SETSTRUCTCONTENT(dstEpInfo, 0);

  dstEpInfo.profileId   = HA_PROFILE_ID;
  dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

  zclAttrInfo_t *pAttrEntry;
  pAttrEntry = zcl_findAttribute(
    cluster->endpoint,
    ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC,
    ZCL_ATTRID_MULTISTATE_INPUT_PRESENT_VALUE
  );

  zcl_sendReportCmd(
    cluster->endpoint,
    &dstEpInfo,
    true,
    ZCL_FRAME_SERVER_CLIENT_DIR,
    ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC,
    pAttrEntry->id, pAttrEntry->type, pAttrEntry->data
  );
}
