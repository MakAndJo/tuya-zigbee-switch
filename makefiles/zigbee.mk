
OUT_DIR += \
/zigbee/af \
/zigbee/aps \
/zigbee/bdb \
/zigbee/common \
/zigbee/gp \
/zigbee/mac \
/zigbee/ss \
/zigbee/ota \
/zigbee/zdo \
/zigbee/zcl \
/zigbee/wwah \
/zigbee/zcl/closures \
/zigbee/zcl/commissioning \
/zigbee/zcl/general \
/zigbee/zcl/hvac \
/zigbee/zcl/light_color_control \
/zigbee/zcl/measument_sensing \
/zigbee/zcl/ota_upgrading \
/zigbee/zcl/security_safety \
/zigbee/zcl/smart_energy \
/zigbee/zcl/zll_commissioning \
/zigbee/zcl/zcl_wwah


OBJS += \
$(BUILD_PATH)/zigbee/bdb/bdb.o \
$(BUILD_PATH)/zigbee/aps/aps_group.o \
$(BUILD_PATH)/zigbee/mac/mac_phy.o \
$(BUILD_PATH)/zigbee/mac/mac_pib.o \
$(BUILD_PATH)/zigbee/zdo/zdp.o \
$(BUILD_PATH)/zigbee/zcl/zcl.o \
$(BUILD_PATH)/zigbee/zcl/zcl_nv.o \
$(BUILD_PATH)/zigbee/zcl/zcl_reporting.o \
$(BUILD_PATH)/zigbee/zcl/hvac/zcl_thermostat.o \
$(BUILD_PATH)/zigbee/zcl/smart_energy/zcl_metering.o \
$(BUILD_PATH)/zigbee/zcl/smart_energy/zcl_metering_attr.o \
$(BUILD_PATH)/zigbee/zcl/zll_commissioning/zcl_toucklink_security.o \
$(BUILD_PATH)/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkDiscovery.o \
$(BUILD_PATH)/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkJoinOrStart.o \
$(BUILD_PATH)/zigbee/zcl/zll_commissioning/zcl_zll_commissioning.o \
$(BUILD_PATH)/zigbee/zcl/commissioning/zcl_commissioning.o \
$(BUILD_PATH)/zigbee/zcl/commissioning/zcl_commissioning_attr.o \
$(BUILD_PATH)/zigbee/zcl/hvac/zcl_thermostat.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_electrical_measurement.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_electrical_measurement_attr.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_illuminance_measurement.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_illuminance_measurement_attr.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_occupancy_sensing.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_occupancy_sensing_attr.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_temperature_measurement.o \
$(BUILD_PATH)/zigbee/zcl/measument_sensing/zcl_temperature_measurement_attr.o \
$(BUILD_PATH)/zigbee/zcl/light_color_control/zcl_light_colorCtrl.o \
$(BUILD_PATH)/zigbee/zcl/light_color_control/zcl_light_colorCtrl_attr.o \
$(BUILD_PATH)/zigbee/zcl/closures/zcl_door_lock.o \
$(BUILD_PATH)/zigbee/zcl/closures/zcl_door_lock_attr.o \
$(BUILD_PATH)/zigbee/zcl/closures/zcl_window_covering.o \
$(BUILD_PATH)/zigbee/zcl/closures/zcl_window_covering_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_alarm.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_alarm_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_basic.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_basic_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_binary_input.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_binary_input_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_binary_output.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_binary_output_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_devTemperatureCfg.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_devTemperatureCfg_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_diagnostics.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_diagnostics_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_greenPower.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_greenPower_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_group.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_group_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_identify.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_identify_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_level.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_level_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_multistate_input.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_multistate_input_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_multistate_output.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_multistate_output_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_onoff.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_onoff_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_pollCtrl.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_pollCtrl_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_powerCfg.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_powerCfg_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_scene.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_scene_attr.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_time.o \
$(BUILD_PATH)/zigbee/zcl/general/zcl_time_attr.o \
$(BUILD_PATH)/zigbee/zcl/security_safety/zcl_ias_ace.o \
$(BUILD_PATH)/zigbee/zcl/security_safety/zcl_ias_wd.o \
$(BUILD_PATH)/zigbee/zcl/security_safety/zcl_ias_wd_attr.o \
$(BUILD_PATH)/zigbee/zcl/security_safety/zcl_ias_zone.o \
$(BUILD_PATH)/zigbee/zcl/security_safety/zcl_ias_zone_attr.o \
$(BUILD_PATH)/zigbee/zcl/zcl_wwah/zcl_wwah.o \
$(BUILD_PATH)/zigbee/zcl/zcl_wwah/zcl_wwah_attr.o \
$(BUILD_PATH)/zigbee/zcl/ota_upgrading/zcl_ota.o \
$(BUILD_PATH)/zigbee/zcl/ota_upgrading/zcl_ota_attr.o \
$(BUILD_PATH)/zigbee/common/zb_config.o \
$(BUILD_PATH)/zigbee/af/zb_af.o \
$(BUILD_PATH)/zigbee/wwah/wwah.o \
$(BUILD_PATH)/zigbee/wwah/wwahEpCfg.o \
$(BUILD_PATH)/zigbee/gp/gp.o \
$(BUILD_PATH)/zigbee/gp/gpEpCfg.o \
${BUILD_PATH}/zigbee/gp/gp_proxy.o \
$(BUILD_PATH)/zigbee/gp/gp_proxyTab.o \
$(BUILD_PATH)/zigbee/ss/ss_nv.o \
$(BUILD_PATH)/zigbee/ota/ota.o \
$(BUILD_PATH)/zigbee/ota/otaEpCfg.o

# Each subdirectory must supply rules for building sources it contributes
$(BUILD_PATH)/zigbee/%.o: $(SDK_PATH)/zigbee/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
