from typing import Final

from zhaquirks import CustomCluster
from zigpy.quirks.v2 import QuirkBuilder, ReportingConfig, SensorDeviceClass
from zigpy.zcl import ClusterType, foundation
from zigpy.zcl.clusters.general import OnOffConfiguration, MultistateInput, OnOff
from zigpy.zcl.foundation import ZCLAttributeDef, ZCLAttributeAccess
import zigpy.types as t


class SwitchActions(t.enum8):
    OnOff = 0x00
    OffOn = 0x01
    ToggleToggle = 0x02
    ToggleSmartSync = 0x03
    ToggleSmartOpposite = 0x04

class SwitchMode(t.enum8):
    Toggle = 0x00
    Momentary = 0x01
    ToggleInverse = 0x10
    MomentaryInverse = 0x11

class RelayMode(t.enum8):
    Detached = 0x00
    Press = 0x01
    ShortPress = 0x02
    LongPress = 0x03

class BindedMode(t.enum8):
    Press = 0x01
    LongPress = 0x02
    ShortPress = 0x03

class BothPressAction(t.enum8):
    NoAction = 0x00
    OnOff = 0x01
    OffOn = 0x02
    ToggleToggle = 0x03

class BothHoldAction(t.enum8):
    NoAction = 0x00
    OnOff = 0x01
    OffOn = 0x02
    ToggleToggle = 0x03


class CustomOnOffConfigurationCluster(CustomCluster, OnOffConfiguration):

    class AttributeDefs(OnOffConfiguration.AttributeDefs):
        """Attribute Definitions"""
        # switch_actions and switch_type is defined in zigpy
        # we override it later
        # here we just set our custom defs

        switch_mode = ZCLAttributeDef(
            id = t.uint16_t(0xff00),
            type = SwitchMode,
            access = (ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific = True,
        )

        relay_mode = ZCLAttributeDef(
            id=t.uint16_t(0xff01),
            type=RelayMode,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        relay_index = ZCLAttributeDef(
            id=t.uint16_t(0xff02),
            type=t.uint8_t,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        binded_mode = ZCLAttributeDef(
            id=t.uint16_t(0xff03),
            type=BindedMode,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        long_press_duration = ZCLAttributeDef(
            id=t.uint16_t(0xff04),
            type=t.uint16_t,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        multi_press_duration = ZCLAttributeDef(
            id=t.uint16_t(0xff05),
            type=t.uint16_t,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        both_press_action = ZCLAttributeDef(
            id=t.uint16_t(0xff07),
            type=BothHoldAction,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )

        both_hold_action = ZCLAttributeDef(
            id=t.uint16_t(0xff08),
            type=BothHoldAction,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write),
            is_manufacturer_specific=True,
        )



class CustomMultistateInputCluster(CustomCluster, MultistateInput):

    class AttributeDefs(foundation.BaseAttributeDefs):
        present_value: Final = ZCLAttributeDef(
            id=t.uint16_t(0x0055),
            type=t.uint16_t,
            access=(ZCLAttributeAccess.Read | ZCLAttributeAccess.Write | ZCLAttributeAccess.Report),
            mandatory=True,
        )
        cluster_revision: Final = foundation.ZCL_CLUSTER_REVISION_ATTR
        reporting_status: Final = foundation.ZCL_REPORTING_STATUS_ATTR


CONFIGS = [
    "ji4araar;TS0011-custom;BA0f;LD7;SC2f;RC0;",
    "jl7qyupf;TS0012-custom;BA0f;LD7;SC2f;SC3f;RC0;RB4;",
]

for config in CONFIGS:
    zb_manufacturer, zb_model, *peripherals = config.rstrip(";").split(";")

    relay_cnt = 0
    switch_cnt = 0
    indicators_cnt = 0
    for peripheral in peripherals:
        if peripheral[0] == "R":
            relay_cnt += 1
        if peripheral[0] == 'S':
            switch_cnt += 1
        if peripheral[0] == 'I':
            indicators_cnt += 1

    builder =  QuirkBuilder(zb_manufacturer, zb_model)

    for endpoint_id in range(1, switch_cnt + 1):
        builder = (
            builder
            .removes(OnOffConfiguration.cluster_id, cluster_type=ClusterType.Client, endpoint_id=endpoint_id)
            .adds(CustomOnOffConfigurationCluster, endpoint_id=endpoint_id)
            .removes(MultistateInput.cluster_id, cluster_type=ClusterType.Client, endpoint_id=endpoint_id)
            .adds(CustomMultistateInputCluster, endpoint_id=endpoint_id)
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.switch_actions.name,
                SwitchActions,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="switch_actions",
                fallback_name=f"Switch actions {endpoint_id}",
                endpoint_id=endpoint_id,
                # Next is hack to force binding to make all attrs values visible.
                # TODO: find a better approach
                reporting_config=ReportingConfig(min_interval=0, max_interval=300, reportable_change=1),
            )
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.switch_mode.name,
                SwitchMode,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="switch_mode",
                fallback_name=f"Switch mode {endpoint_id}",
                endpoint_id=endpoint_id,
            )
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.relay_mode.name,
                RelayMode,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="relay_mode",
                fallback_name=f"Relay mode {endpoint_id}",
                endpoint_id=endpoint_id,
            )
            .number(
                CustomOnOffConfigurationCluster.AttributeDefs.relay_index.name,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="relay_index",
                fallback_name=f"Relay index {endpoint_id}",
                endpoint_id=endpoint_id,
                min_value=1,
                max_value=relay_cnt,
                step=1,
            )
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.binded_mode.name,
                BindedMode,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="binded_mode",
                fallback_name=f"Binded mode {endpoint_id}",
                endpoint_id=endpoint_id,
            )
            .number(
                CustomOnOffConfigurationCluster.AttributeDefs.long_press_duration.name,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="long_press_duration",
                fallback_name=f"Long press duration {endpoint_id}",
                endpoint_id=endpoint_id,
                min_value=50,
                max_value=5000,
                step=1,
            )
            .number(
                CustomOnOffConfigurationCluster.AttributeDefs.multi_press_duration.name,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="multi_press_duration",
                fallback_name=f"Multi press duration {endpoint_id}",
                endpoint_id=endpoint_id,
                min_value=50,
                max_value=5000,
                step=1,
            )
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.both_press_action.name,
                BothPressAction,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="both_press_action",
                fallback_name=f"Both press action {endpoint_id}",
                endpoint_id=endpoint_id,
            )
            .enum(
                CustomOnOffConfigurationCluster.AttributeDefs.both_hold_action.name,
                BothHoldAction,
                CustomOnOffConfigurationCluster.cluster_id,
                translation_key="both_hold_action",
                fallback_name=f"Both hold action {endpoint_id}",
                endpoint_id=endpoint_id,
            )
            .sensor(
                MultistateInput.AttributeDefs.present_value.name,
                MultistateInput.cluster_id,
                translation_key="press_action",
                fallback_name=f"Press action {endpoint_id}",
                endpoint_id=endpoint_id,
                reporting_config=ReportingConfig(min_interval=0, max_interval=300, reportable_change=1),
                device_class=SensorDeviceClass.ENUM,
                attribute_converter = lambda x: {
                    0: "release",
                    1: "single",
                    128: "hold",
                    2: "double",
                    3: "triple",
                    4: "quadruple",
                    5: "quintuple",
                    200: "both_release",
                    201: "both",
                    202: "both_hold",
                }[int(x)]
            )
        )



    builder.add_to_registry()