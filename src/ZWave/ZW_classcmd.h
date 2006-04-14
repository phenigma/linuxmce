//
// C++ Interface: PlutoZWSerialAPI
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _ZW_CLASSCMD_H_
#define _ZW_CLASSCMD_H_

typedef unsigned char BYTE;

/****************************************************************************
 *    TYPES and DEFINITIONS
 ***************************************************************************/
#define ZW_FRAME_COLLECTION_MACRO\
  ZW_COMMON_FRAME                         ZW_Common;\
  /* Basic command class */ \
  ZW_BASIC_SET_FRAME                      ZW_BasicSetFrame;\
  ZW_BASIC_GET_FRAME                      ZW_BasicGetFrame;\
  ZW_BASIC_REPORT_FRAME                   ZW_BasicReportFrame;\
  /* Controller Replication command class */\
  ZW_TRANSFER_GROUP_FRAME                 ZW_TransferGroupFrame;\
  ZW_TRANSFER_GROUP_NAME_FRAME            ZW_TransferGroupNameFrame;\
  ZW_TRANSFER_SCENE_FRAME                 ZW_TransferSceneFrame;\
  ZW_TRANSFER_SCENE_NAME_FRAME            ZW_TransferSceneNameFrame;\
  /* Application Status command class */\
  ZW_APPLICATION_BUSY_FRAME               ZW_ApplicationBusyFrame;\
  ZW_APPLICATION_REJECTED_REQUEST_FRAME   ZW_ApplicationRejectedRequestFrame;\
  /* Binary Switch command class */\
  ZW_BINARY_SWITCH_SET_FRAME              ZW_BinarySwitchSetFrame;\
  ZW_BINARY_SWITCH_GET_FRAME              ZW_BinarySwitchGetFrame;\
  ZW_BINARY_SWITCH_REPORT_FRAME           ZW_BinarySwitchReportFrame;\
  /* Multilevel Switch command class */\
  ZW_MULTILEVEL_SWITCH_SET_FRAME          ZW_MultilevelSwitchSetFrame;\
  ZW_MULTILEVEL_SWITCH_GET_FRAME          ZW_MultilevelSwitchGetFrame;\
  ZW_MULTILEVEL_SWITCH_REPORT_FRAME       ZW_MultilevelSwitchReportFrame;\
  ZW_MULTILEVEL_SWITCH_START_CHANGE_FRAME ZW_MultilevelSwitchStartChangeFrame;\
  ZW_MULTILEVEL_SWITCH_STOP_CHANGE_FRAME  ZW_MultilevelSwitchStopChangeFrame;\
  ZW_MULTILEVEL_SWITCH_DO_CHANGE_FRAME    ZW_MultilevelSwitchDoChangeFrame;\
  /* All Switch command class */\
  ZW_ALL_SWITCH_SET_FRAME                 ZW_AllSwitchSetFrame;\
  ZW_ALL_SWITCH_GET_FRAME                 ZW_AllSwitchGetFrame;\
  ZW_ALL_SWITCH_REPORT_FRAME              ZW_AllSwitchReportFrame;\
  ZW_ALL_SWITCH_ON_FRAME                  ZW_AllSwitchOnFrame;\
  ZW_ALL_SWITCH_OFF_FRAME                 ZW_AllSwitchOffFrame;\
  /* Binary Toggle Switch command class */\
  ZW_BINARY_TOGGLE_SWITCH_SET_FRAME                 ZW_BinaryToggleSwitchSetFrame;\
  ZW_BINARY_TOGGLE_SWITCH_GET_FRAME                 ZW_BinaryToggleSwitchGetFrame;\
  ZW_BINARY_TOGGLE_SWITCH_REPORT_FRAME              ZW_BinaryToggleSwitchReportFrame;\
  /* Multilevel Toggle Switch command class */\
  ZW_MULTILEVEL_TOGGLE_SWITCH_SET_FRAME             ZW_MultilevelToggleSwitchSetFrame;\
  ZW_MULTILEVEL_TOGGLE_SWITCH_GET_FRAME             ZW_MultilevelToggleSwitchGetFrame;\
  ZW_MULTILEVEL_TOGGLE_SWITCH_REPORT_FRAME          ZW_MultilevelToggleSwitchReportFrame;\
  ZW_MULTILEVEL_TOGGLE_SWITCH_START_CHANGE_FRAME    ZW_MultilevelToggleSwitchStartChangeFrame;\
  ZW_MULTILEVEL_TOGGLE_SWITCH_STOP_CHANGE_FRAME     ZW_MultilevelToggleSwitchStopChangeFrame;\
  /* Binary Sensor command class */\
  ZW_BINARY_SENSOR_GET_FRAME                        ZW_BinarySensorGetFrame;\
  ZW_BINARY_SENSOR_REPORT_FRAME                     ZW_BinarySensorReportFrame;\
  /* Multiple Sensor command class */\
  ZW_MULTILEVEL_SENSOR_GET_FRAME                    ZW_MultilevelSensorGetFrame;\
  ZW_MULTILEVEL_SENSOR_REPORT_1BYTE_FRAME           ZW_MultilevelSensor1ByteReportFrame;\
  ZW_MULTILEVEL_SENSOR_REPORT_2BYTE_FRAME           ZW_MultilevelSensor2ByteReportFrame;\
  ZW_MULTILEVEL_SENSOR_REPORT_4BYTE_FRAME           ZW_MultilevelSensor4ByteReportFrame;\
  /* Pulse Meter command class */\
  ZW_PULSE_METER_GET_FRAME                          ZW_PulseMeterGetFrame;\
  ZW_PULSE_METER_REPORT_FRAME                       ZW_PulseMeterReportFrame;\
  /* Thermostat Heating command class */\
  ZW_THERMOSTAT_HEATING_MODE_SET_FRAME              ZW_ThermostatHeatingModeSetFrame;\
  ZW_THERMOSTAT_HEATING_MODE_GET_FRAME              ZW_ThermostatHeatingModeGetFrame;\
  ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME           ZW_ThermostatHeatingModeReportFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_SET_FRAME    ZW_ThermostatHeatingSetpoint1ByteSetFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_SET_FRAME    ZW_ThermostatHeatingSetpoint2ByteSetFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_SET_FRAME    ZW_ThermostatHeatingSetpoint4ByteSetFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME          ZW_ThermostatHeatingSetpointGetFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_REPORT_FRAME ZW_ThermostatHeatingSetpoint1ByteReportFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_REPORT_FRAME ZW_ThermostatHeatingSetpoint2ByteReportFrame;\
  ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_REPORT_FRAME ZW_ThermostatHeatingSetpoint4ByteReportFrame;\
  ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME      ZW_ThermostatHeatingRelayStatusGetFrame;\
  ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME   ZW_ThermostatHeatingRelayStatusReportFrame;\
  ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME            ZW_ThermostatHeatingStatusSetFrame;\
  ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME            ZW_ThermostatHeatingStatusGetFrame;\
  ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME         ZW_ThermostatHeatingStatusReportFrame;\
  ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME         ZW_ThermostatHeatingTimedOffSetFrame;\
  /* Thermostat Mode command class */\
  ZW_THERMOSTAT_MODE_SET_FRAME                      ZW_ThermostatModeSetFrame;\
  ZW_THERMOSTAT_MODE_GET_FRAME                      ZW_ThermostatModeGetFrame;\
  ZW_THERMOSTAT_MODE_REPORT_FRAME                   ZW_ThermostatModeReportFrame;\
  /* Thermostat Hold Mode command class */\
  ZW_THERMOSTAT_HOLD_MODE_SET_FRAME                 ZW_ThermostatHoldModeSetFrame;\
  ZW_THERMOSTAT_HOLD_MODE_GET_FRAME                 ZW_ThermostatHoldModeGetFrame;\
  ZW_THERMOSTAT_HOLD_MODE_REPORT_FRAME              ZW_ThermostatHoldModeReportFrame;\
  /* Thermostat Operating State command class */\
  ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME           ZW_ThermostatOperatingStateGetFrame;\
  ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME        ZW_ThermostatOperatingState2ByteReportFrame;\
  /* Thermostat Setpoint Heat command class */\
  ZW_THERMOSTAT_SETPOINT_1BYTE_SET_FRAME            ZW_ThermostatSetpoint1ByteSetFrame;\
  ZW_THERMOSTAT_SETPOINT_2BYTE_SET_FRAME            ZW_ThermostatSetpoint2ByteSetFrame;\
  ZW_THERMOSTAT_SETPOINT_4BYTE_SET_FRAME            ZW_ThermostatSetpoint4ByteSetFrame;\
  ZW_THERMOSTAT_SETPOINT_GET_FRAME                  ZW_ThermostatSetpointGetFrame;\
  ZW_THERMOSTAT_SETPOINT_1BYTE_REPORT_FRAME         ZW_ThermostatSetpoint1ByteReportFrame;\
  ZW_THERMOSTAT_SETPOINT_2BYTE_REPORT_FRAME         ZW_ThermostatSetpoint2ByteReportFrame;\
  ZW_THERMOSTAT_SETPOINT_4BYTE_REPORT_FRAME         ZW_ThermostatSetpoint4ByteReportFrame;\
  /* Thermostat Fan command class */\
  ZW_THERMOSTAT_FAN_SET_FRAME                       ZW_ThermostatFanSetFrame;\
  ZW_THERMOSTAT_FAN_GET_FRAME                       ZW_ThermostatFanGetFrame;\
  ZW_THERMOSTAT_FAN_REPORT_FRAME                    ZW_ThermostatFanReportFrame;\
  /* Thermostat Fan State command class */\
  ZW_THERMOSTAT_FAN_STATE_GET_FRAME                 ZW_ThermostatFanStateGetFrame;\
  ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME              ZW_ThermostatFanStateReportFrame;\
  /* Thermostat Schedule command class */\
  ZW_THERMOSTAT_SCHEDULE_1BYTE_SET_FRAME            ZW_ThermostatSchedule1ByteSetFrame;\
  ZW_THERMOSTAT_SCHEDULE_2BYTE_SET_FRAME            ZW_ThermostatSchedule2ByteSetFrame;\
  ZW_THERMOSTAT_SCHEDULE_4BYTE_SET_FRAME            ZW_ThermostatSchedule4ByteSetFrame;\
  ZW_THERMOSTAT_SCHEDULE_GET_FRAME                  ZW_ThermostatScheduleGetFrame;\
  ZW_THERMOSTAT_SCHEDULE_1BYTE_REPORT_FRAME         ZW_ThermostatSchedule1ByteReportFrame;\
  ZW_THERMOSTAT_SCHEDULE_2BYTE_REPORT_FRAME         ZW_ThermostatSchedule2ByteReportFrame;\
  ZW_THERMOSTAT_SCHEDULE_4BYTE_REPORT_FRAME         ZW_ThermostatSchedule4ByteReportFrame;\
  ZW_THERMOSTAT_SCHEDULE_MODE_SET_FRAME             ZW_ThermostatScheduleModeSetFrame;\
  ZW_THERMOSTAT_SCHEDULE_MODE_GET_FRAME             ZW_ThermostatScheduleModeGetFrame;\
  ZW_THERMOSTAT_SCHEDULE_MODE_REPORT_FRAME          ZW_ThermostatScheduleModeReportFrame;\
  /* Multi Instance command class */\
  ZW_MULTI_INSTANCE_GET_FRAME                       ZW_MultiInstanceGetFrame;\
  ZW_MULTI_INSTANCE_REPORT_FRAME                    ZW_MultiInstanceReportFrame;\
  /* Configuration command class */\
  ZW_CONFIGURATION_SET_1BYTE_FRAME        ZW_ConfigurationSet1ByteFrame;\
  ZW_CONFIGURATION_SET_2BYTE_FRAME        ZW_ConfigurationSet2ByteFrame;\
  ZW_CONFIGURATION_SET_4BYTE_FRAME        ZW_ConfigurationSet4ByteFrame;\
  ZW_CONFIGURATION_GET_FRAME              ZW_ConfigurationGetFrame;\
  ZW_CONFIGURATION_REPORT_1BYTE_FRAME     ZW_ConfigurationReport1ByteFrame;\
  ZW_CONFIGURATION_REPORT_2BYTE_FRAME     ZW_ConfigurationReport2ByteFrame;\
  ZW_CONFIGURATION_REPORT_4BYTE_FRAME     ZW_ConfigurationReport4ByteFrame;\
  /* Alarm command class */\
  ZW_ALARM_GET_FRAME                      ZW_AlarmGetFrame;\
  ZW_ALARM_REPORT_FRAME                   ZW_AlarmReportFrame;\
  /* Manufacturer Specific command class */\
  ZW_MANUFACTURER_SPECIFIC_GET_FRAME      ZW_ManufacturerSpecificGetFrame;\
  ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME   ZW_ManufacturerSpecificReportFrame;\
  /* PowerLevel command class */\
  ZW_POWERLEVEL_SET_FRAME                 ZW_PowerLevelSetFrame;\
  ZW_POWERLEVEL_GET_FRAME                 ZW_PowerLevelGetFrame;\
  ZW_POWERLEVEL_REPORT_FRAME              ZW_PowerLevelReportFrame;\
  ZW_POWERLEVEL_TEST_NODE_SET_FRAME       ZW_PowerLevelTestNodeSetFrame;\
  ZW_POWERLEVEL_TEST_NODE_GET_FRAME       ZW_PowerLevelTestNodeGetFrame;\
  ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME    ZW_PowerLevelTestNodeReportFrame;\
  /* Protection command class */\
  ZW_PROTECTION_SET_FRAME                 ZW_ProtectionSetFrame;\
  ZW_PROTECTION_GET_FRAME                 ZW_ProtectionGetFrame;\
  ZW_PROTECTION_REPORT_FRAME              ZW_ProtectionReportFrame;\
  /* Lock command class */\
  ZW_LOCK_SET_FRAME                       ZW_LockSetFrame;\
  ZW_LOCK_GET_FRAME                       ZW_LockGetFrame;\
  ZW_LOCK_REPORT_FRAME                    ZW_LockReportFrame;\
  /* Battery command class */\
  ZW_BATTERY_GET_FRAME                    ZW_BatteryGetFrame;\
  ZW_BATTERY_REPORT_FRAME                 ZW_BatteryReportFrame;\
  /* Clock command class */\
  ZW_CLOCK_SET_FRAME                      ZW_ClockSetFrame;\
  ZW_CLOCK_GET_FRAME                      ZW_ClockGetFrame;\
  ZW_CLOCK_REPORT_FRAME                   ZW_ClockReportFrame;\
  /* Wake Up command class */\
  ZW_WAKE_UP_SET_FRAME                    ZW_WakeUpSetFrame;\
  ZW_WAKE_UP_GET_FRAME                    ZW_WakeUpGetFrame;\
  ZW_WAKE_UP_INTERVAL_REPORT_FRAME        ZW_WakeUpIntervalReportFrame;\
  ZW_WAKE_UP_NOTIFICATION_FRAME           ZW_WakeUpNotificationFrame;\
  /* Association command class */\
  ZW_ASSOCIATION_SET_FRAME                ZW_AssociationSetFrame;\
  ZW_ASSOCIATION_GET_FRAME                ZW_AssociationGetFrame;\
  ZW_ASSOCIATION_REPORT_FRAME             ZW_AssociationReportFrame;\
  ZW_ASSOCIATION_REMOVE_FRAME             ZW_AssociationRemoveFrame;\
  ZW_ASSOCIATION_GROUPINGS_GET_FRAME      ZW_AssociationGroupingsGetFrame;\
  ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME   ZW_AssociationGroupingsReportFrame;\
  /* Version command class */\
  ZW_VERSION_GET_FRAME                    ZW_VersionGetFrame;\
  ZW_VERSION_REPORT_FRAME                 ZW_VersionReportFrame;\
  ZW_VERSION_COMMAND_GET_FRAME            ZW_VersionCommandGetFrame;\
  ZW_VERSION_COMMAND_REPORT_FRAME         ZW_VersionCommandReportFrame;\
  /* Indicator command class */\
  ZW_INDICATOR_SET_FRAME                  ZW_IndicatorSetFrame;\
  ZW_INDICATOR_GET_FRAME                  ZW_IndicatorGetFrame;\
  ZW_INDICATOR_REPORT_FRAME               ZW_IndicatorReportFrame;\
  /* Proprietary command class */\
  ZW_PROPRIETARY_SET_2BYTE_FRAME          ZW_ProprietarySet2ByteFrame;\
  ZW_PROPRIETARY_GET_FRAME                ZW_ProprietaryGetFrame;\
  ZW_PROPRIETARY_REPORT_2BYTE_FRAME       ZW_ProprietaryReport2ByteFrame;\
  /* Language command class */\
  ZW_LANGUAGE_SET_FRAME                   ZW_LanguageSetFrame;\
  ZW_LANGUAGE_GET_FRAME                   ZW_LanguageGetFrame;\
  ZW_LANGUAGE_REPORT_FRAME                ZW_LanguageReportFrame;\
  /* Energy Production command class */\
  ZW_ENERGY_PRODUCTION_GET_FRAME          ZW_EnergyProductionGetFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME ZW_EnergyProductionReport1ByteFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME ZW_EnergyProductionReport2ByteFrame;\
  ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME ZW_EnergyProductionReport4ByteFrame;\
  /* Network Statistics command class */\
  ZW_NETWORK_STAT_SET_FRAME               ZW_NetworkStatSetFrame;\
  ZW_NETWORK_STAT_GET_FRAME               ZW_NetworkStatGetFrame;\
  ZW_NETWORK_STAT_REPORT_FRAME            ZW_NetworkStatReportFrame;\
  /* Garage Door command class */\
  ZW_GARAGE_DOOR_GET_FRAME                ZW_GarageDoorGetFrame;\
  ZW_GARAGE_DOOR_REPORT_FRAME             ZW_GarageDoorReportFrame;\
  ZW_GARAGE_DOOR_START_CHANGE_FRAME       ZW_GarageDoorStartChangeFrame;\
  ZW_GARAGE_DOOR_STOP_CHANGE_FRAME        ZW_GarageDoorStopChangeFrame;\
  /* Display command class */\
  ZW_DISPLAY_SET_FRAME                    ZW_DisplaySetFrame;\
  ZW_DISPLAY_SUPPORTED_GET_FRAME          ZW_DisplaySupportedGetFrame;\
  ZW_DISPLAY_SUPPORTED_REPORT_FRAME       ZW_DisplaySupportedReportFrame;

/*********** Basic Device Class identifiers *************/
#define BASIC_TYPE_CONTROLLER                         0x01
#define BASIC_TYPE_STATIC_CONTROLLER                  0x02
#define BASIC_TYPE_SLAVE                              0x03
#define BASIC_TYPE_ROUTING_SLAVE                      0x04

/**** Generic and Specific Device Class identifiers *****/
#define GENERIC_TYPE_GENERIC_CONTROLLER               0x01
#define SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER      0x01
 
#define GENERIC_TYPE_STATIC_CONTROLLER                0x02
#define SPECIFIC_TYPE_PC_CONTROLLER                   0x01

#define GENERIC_TYPE_DISPLAY                          0x06
#define SPECIFIC_TYPE_SIMPLE_DISPLAY                  0x01

#define GENERIC_TYPE_GARAGE_DOOR                      0x07
#define SPECIFIC_TYPE_SIMPLE_GARAGE_DOOR              0x01

#define GENERIC_TYPE_THERMOSTAT                       0x08
#define SPECIFIC_TYPE_THERMOSTAT_HEATING              0x01
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL              0x02

#define GENERIC_TYPE_REPEATER_SLAVE                   0x0F
#define SPECIFIC_TYPE_REPEATER_SLAVE                  0x01

#define GENERIC_TYPE_SWITCH_BINARY                    0x10
#define SPECIFIC_TYPE_POWER_SWITCH_BINARY             0x01

#define GENERIC_TYPE_SWITCH_MULTILEVEL                0x11
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL         0x01

#define GENERIC_TYPE_SWITCH_REMOTE                    0x12
#define SPECIFIC_TYPE_SWITCH_REMOTE_BINARY            0x01
#define SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL        0x02
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY     0x03
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL 0x04

#define GENERIC_TYPE_SWITCH_TOGGLE                    0x13
#define SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY            0x01
#define SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL        0x02

#define GENERIC_TYPE_SENSOR_BINARY                    0x20
#define SPECIFIC_TYPE_ROUTING_SENSOR_BINARY           0x01

#define GENERIC_TYPE_SENSOR_MULTILEVEL                0x21
#define SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL       0x01
#define SPECIFIC_TYPE_CHIMNEY_FAN_SENSOR_MULTILEVEL   0x02

#define GENERIC_TYPE_METER_PULSE                      0x30

#define GENERIC_TYPE_ENTRY_CONTROL                    0x40
#define SPECIFIC_TYPE_DOOR_LOCK                       0x01

#define GENERIC_TYPE_SEMI_INTEROPERABLE               0x50
#define SPECIFIC_TYPE_ENERGY_PRODUCTION               0x01

#define GENERIC_TYPE_NON_INTEROPERABLE                0xFF
#define SPECIFIC_TYPE_NOT_USED                        0x00

/************** command class identifiers ***************/
#define COMMAND_CLASS_BASIC                           0x20
#define COMMAND_CLASS_CONTROLLER_REPLICATION          0x21
#define COMMAND_CLASS_APPLICATION_STATUS              0x22
#define COMMAND_CLASS_SWITCH_BINARY                   0x25
#define COMMAND_CLASS_SWITCH_MULTILEVEL               0x26
#define COMMAND_CLASS_SWITCH_ALL                      0x27
#define COMMAND_CLASS_SWITCH_TOGGLE_BINARY            0x28
#define COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL        0x29
#define COMMAND_CLASS_CHIMNEY_FAN                     0x2A
#define COMMAND_CLASS_SENSOR_BINARY                   0x30
#define COMMAND_CLASS_SENSOR_MULTILEVEL               0x31
#define COMMAND_CLASS_METER_PULSE                     0x35
#define COMMAND_CLASS_THERMOSTAT_HEATING              0x38
#define COMMAND_CLASS_THERMOSTAT_MODE                 0x40
#define COMMAND_CLASS_THERMOSTAT_HOLD_MODE            0x41
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE      0x42
#define COMMAND_CLASS_THERMOSTAT_SETPOINT             0x43
#define COMMAND_CLASS_THERMOSTAT_FAN                  0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_STATE            0x45
#define COMMAND_CLASS_THERMOSTAT_SCHEDULE             0x46
#define COMMAND_CLASS_MULTI_INSTANCE                  0x60
#define COMMAND_CLASS_DISPLAY                         0x61
#define COMMAND_CLASS_GARAGE_DOOR                     0x64
#define COMMAND_CLASS_CONFIGURATION                   0x70
#define COMMAND_CLASS_ALARM                           0x71
#define COMMAND_CLASS_MANUFACTURER_SPECIFIC           0x72
#define COMMAND_CLASS_POWERLEVEL                      0x73
#define COMMAND_CLASS_PROTECTION                      0x75
#define COMMAND_CLASS_LOCK                            0x76
#define COMMAND_CLASS_BATTERY                         0x80
#define COMMAND_CLASS_CLOCK                           0x81
#define COMMAND_CLASS_NETWORK_STAT                    0x83
#define COMMAND_CLASS_WAKE_UP                         0x84
#define COMMAND_CLASS_ASSOCIATION                     0x85
#define COMMAND_CLASS_VERSION                         0x86
#define COMMAND_CLASS_INDICATOR                       0x87
#define COMMAND_CLASS_PROPRIETARY                     0x88
#define COMMAND_CLASS_LANGUAGE                        0x89
#define COMMAND_CLASS_ENERGY_PRODUCTION               0x90
#define COMMAND_CLASS_MARK                            0xEF
#define COMMAND_CLASS_NON_INTEROPERABLE               0xF0

/*********** Unknown command class commands ************/
#define UNKNOWN_VERSION                              0x00

/************ Basic command class commands *************/
#define BASIC_VERSION                                0x01
#define BASIC_SET                                    0x01
#define BASIC_GET                                    0x02
#define BASIC_REPORT                                 0x03
// Basic level definitions
#define BASIC_ON                                     0xFF
#define BASIC_OFF                                    0x00

/**** Controller Replication command class commands ****/
#define CTRL_REPLICATION_TRANSFER_VERSION            0x01
#define CTRL_REPLICATION_TRANSFER_GROUP              0x31
#define CTRL_REPLICATION_TRANSFER_GROUP_NAME         0x32
#define CTRL_REPLICATION_TRANSFER_SCENE              0x33
#define CTRL_REPLICATION_TRANSFER_SCENE_NAME         0x34
// Max. group and scene text length
#define CTRL_REPLICATION_NAME_MAX_TXT_LEN            12

/****** Application Status command class commands ******/
#define APPLICATION_VERSION                          0x01
#define APPLICATION_STATUS_BUSY                      0x01
#define APPLICATION_REJECTED_REQUEST                 0x02

/******** Binary Switch command class commands *********/
#define SWITCH_BINARY_VERSION                        0x01
#define SWITCH_BINARY_SET                            0x01
#define SWITCH_BINARY_GET                            0x02
#define SWITCH_BINARY_REPORT                         0x03
// Binary Switch definitions
#define SWITCHED_ON                                  0xFF
#define SWITCHED_OFF                                 0x00

/****** Multilevel Switch command class commands *******/
#define SWITCH_MULTILEVEL_VERSION                    0x01
#define SWITCH_MULTILEVEL_SET                        0x01
#define SWITCH_MULTILEVEL_GET                        0x02
#define SWITCH_MULTILEVEL_REPORT                     0x03
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE         0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE          0x05
#define SWITCH_MULTILEVEL_DO_LEVEL_CHANGE            0x06
// Multilevel switch definitions
#define SWITCH_MULTILEVEL_ROLL_OVER_BIT              0x80 // If this bit is set the dimmer should rollover
#define SWITCH_MULTILEVEL_DOWN_BIT                   0x40 // If this bit is 1 the dimmer
#define SWITCH_MULTILEVEL_LEVEL_CHANGE_MASK          0xC0

/********* All Switch command class commands ***********/
#define SWITCH_ALL_VERSION                           0x01
#define SWITCH_ALL_SET                               0x01
#define SWITCH_ALL_GET                               0x02
#define SWITCH_ALL_REPORT                            0x03
#define SWITCH_ALL_ON                                0x04
#define SWITCH_ALL_OFF                               0x05
// All switch definitions
#define ALL_ON_PROTECT_OFF                           0xFF
#define ALL_ON_PROTECT_ON                            0x00

/***** Toggle Switch Binary command class commands *****/
#define SWITCH_TOGGLE_BINARY_VERSION                 0x01
#define SWITCH_TOGGLE_BINARY_SET                     0x01
#define SWITCH_TOGGLE_BINARY_GET                     0x02
#define SWITCH_TOGGLE_BINARY_REPORT                  0x03

/*** Toggle Switch Multilevel command class commands ***/
#define SWITCH_TOGGLE_MULTILEVEL_VERSION             0x01
#define SWITCH_TOGGLE_MULTILEVEL_SET                 0x01
#define SWITCH_TOGGLE_MULTILEVEL_GET                 0x02
#define SWITCH_TOGGLE_MULTILEVEL_REPORT              0x03
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE  0x04
#define SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE   0x05

/********* Chimney Fan command class commands **********/
#define CHIMNEY_FAN_VERSION                          0x01
#define CHIMNEY_FAN_STATE_SET                        0x01
#define CHIMNEY_FAN_STATE_GET                        0x02
#define CHIMNEY_FAN_STATE_REPORT                     0x03
#define CHIMNEY_FAN_SPEED_SET                        0x04
#define CHIMNEY_FAN_SPEED_GET                        0x05
#define CHIMNEY_FAN_SPEED_REPORT                     0x06
#define CHIMNEY_FAN_START_TEMP_SET                   0x07
#define CHIMNEY_FAN_START_TEMP_GET                   0x08
#define CHIMNEY_FAN_START_TEMP_REPORT                0x09
#define CHIMNEY_FAN_STOP_TEMP_SET                    0x0A
#define CHIMNEY_FAN_STOP_TEMP_GET                    0x0B
#define CHIMNEY_FAN_STOP_TEMP_REPORT                 0x0C
#define CHIMNEY_FAN_ALARM_TEMP_SET                   0x0D
#define CHIMNEY_FAN_ALARM_TEMP_GET                   0x0E
#define CHIMNEY_FAN_ALARM_TEMP_REPORT                0x0F
#define CHIMNEY_FAN_BOOST_TIME_SET                   0x10
#define CHIMNEY_FAN_BOOST_TIME_GET                   0x11
#define CHIMNEY_FAN_BOOST_TIME_REPORT                0x12
#define CHIMNEY_FAN_STOP_TIME_SET                    0x13
#define CHIMNEY_FAN_STOP_TIME_GET                    0x14
#define CHIMNEY_FAN_STOP_TIME_REPORT                 0x15
#define CHIMNEY_FAN_MODE_SET                         0x16
#define CHIMNEY_FAN_MODE_GET                         0x17
#define CHIMNEY_FAN_MODE_REPORT                      0x18
#define CHIMNEY_FAN_SETUP_SET                        0x19
#define CHIMNEY_FAN_SETUP_GET                        0x1A
#define CHIMNEY_FAN_SETUP_REPORT                     0x1B
#define CHIMNEY_FAN_STATUS_SET                       0x1C
#define CHIMNEY_FAN_STATUS_GET                       0x1D
#define CHIMNEY_FAN_STATUS_REPORT                    0x1E
#define CHIMNEY_FAN_ALARM_LOG_SET                    0x1F
#define CHIMNEY_FAN_ALARM_LOG_GET                    0x20
#define CHIMNEY_FAN_ALARM_LOG_REPORT                 0x21
#define CHIMNEY_FAN_ALARM_STATUS_SET                 0x22
#define CHIMNEY_FAN_ALARM_STATUS_GET                 0x23
#define CHIMNEY_FAN_ALARM_STATUS_REPORT              0x24
#define CHIMNEY_FAN_MIN_SPEED_SET                    0x25
#define CHIMNEY_FAN_MIN_SPEED_GET                    0x26
#define CHIMNEY_FAN_MIN_SPEED_REPORT                 0x27
#define CHIMNEY_FAN_DEFAULT_SET                      0x28

/******** SENSOR BINARY command class commands *********/
#define SENSOR_BINARY_VERSION                        0x01
#define SENSOR_BINARY_GET                            0x02
#define SENSOR_BINARY_REPORT                         0x03

/****** Sensor Multilevel command class commands *******/
#define SENSOR_MULTILEVEL_VERSION                    0x01
#define SENSOR_MULTILEVEL_GET                        0x04
#define SENSOR_MULTILEVEL_REPORT                     0x05
// Defines for SENSOR_MULTILEVEL_REPORT
#define SENSOR_MULTILEVEL_PRECISION_MASK             0xE0
#define SENSOR_MULTILEVEL_SCALE_MASK                 0x18
#define SENSOR_MULTILEVEL_SIZE_MASK                  0x07
#define SENSOR_MULTILEVEL_PRECISION_SHIFT            0x05
#define SENSOR_MULTILEVEL_SCALE_SHIFT                0x03
#define SENSOR_TYPE_TEMPERATURE_SCALE_C              0x00
#define SENSOR_TYPE_TEMPERATURE_SCALE_F              0x01
#define SENSOR_TYPE_TEMPERATURE                      0x01

/********* Pulse Meter command class commands **********/
#define METER_PULSE_VERSION                          0x01
#define METER_PULSE_GET                              0x04
#define METER_PULSE_REPORT                           0x05

/***** Thermostat Heating command class commands *******/
#define THERMOSTAT_HEATING_VERSION                   0x01
#define THERMOSTAT_HEATING_MODE_SET                  0x01
#define THERMOSTAT_HEATING_MODE_GET                  0x02
#define THERMOSTAT_HEATING_MODE_REPORT               0x03
#define THERMOSTAT_HEATING_SETPOINT_SET              0x04
#define THERMOSTAT_HEATING_SETPOINT_GET              0x05
#define THERMOSTAT_HEATING_SETPOINT_REPORT           0x06
#define THERMOSTAT_HEATING_RELAY_STATUS_GET          0x09
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT       0x0A
#define THERMOSTAT_HEATING_STATUS_SET                0x0B
#define THERMOSTAT_HEATING_STATUS_GET                0x0C
#define THERMOSTAT_HEATING_STATUS_REPORT             0x0D
#define THERMOSTAT_HEATING_TIMED_OFF_SET             0x11

/*********** Thermostat Mode command class *************/
#define THERMOSTAT_MODE_VERSION                      0x01
#define THERMOSTAT_MODE_SET                          0x01
#define THERMOSTAT_MODE_GET                          0x02
#define THERMOSTAT_MODE_REPORT                       0x03

/******** Thermostat Hold Mode command class ***********/
#define THERMOSTAT_HOLD_MODE_VERSION                 0x01
#define THERMOSTAT_HOLD_MODE_SET                     0x01
#define THERMOSTAT_HOLD_MODE_GET                     0x02
#define THERMOSTAT_HOLD_MODE_REPORT                  0x03

/****** Thermostat Operating State command class *******/
#define THERMOSTAT_OPERATING_STATE_VERSION           0x01
#define THERMOSTAT_OPERATING_STATE_GET               0x02
#define THERMOSTAT_OPERATING_STATE_REPORT            0x03

/********* Thermostat Setpoint command class ***********/
#define THERMOSTAT_SETPOINT_VERSION                  0x01
#define THERMOSTAT_SETPOINT_SET                      0x01
#define THERMOSTAT_SETPOINT_GET                      0x02
#define THERMOSTAT_SETPOINT_REPORT                   0x03

/************ Thermostat Fan command class *************/
#define THERMOSTAT_FAN_VERSION                       0x01
#define THERMOSTAT_FAN_SET                           0x01
#define THERMOSTAT_FAN_GET                           0x02
#define THERMOSTAT_FAN_REPORT                        0x03

/********* Thermostat Fan State command class **********/
#define THERMOSTAT_FAN_STATE_VERSION                 0x01
#define THERMOSTAT_FAN_STATE_GET                     0x02
#define THERMOSTAT_FAN_STATE_REPORT                  0x03

/********** Thermostat Schedule command class **********/
#define THERMOSTAT_SCHEDULE_VERSION                  0x01
#define THERMOSTAT_SCHEDULE_SET                      0x01
#define THERMOSTAT_SCHEDULE_GET                      0x02
#define THERMOSTAT_SCHEDULE_REPORT                   0x03
#define THERMOSTAT_SCHEDULE_MODE_SET                 0x04
#define THERMOSTAT_SCHEDULE_MODE_GET                 0x05
#define THERMOSTAT_SCHEDULE_MODE_REPORT              0x06

/******** Multi Instance command class commands ********/
#define MULTI_INSTANCE_VERSION                       0x01
#define MULTI_INSTANCE_GET                           0x04
#define MULTI_INSTANCE_REPORT                        0x05
#define MULTI_INSTANCE_CMD_ENCAP                     0x06

/********* Garage Door command class commands **********/
#define GARAGE_DOOR_VERSION                          0x01
#define GARAGE_DOOR_GET                              0x01
#define GARAGE_DOOR_REPORT                           0x02
#define GARAGE_DOOR_START_LEVEL_CHANGE               0x03
#define GARAGE_DOOR_STOP_LEVEL_CHANGE                0x04

/******** Configuration command class commands *********/
#define CONFIGURATION_VERSION                        0x01
#define CONFIGURATION_SET                            0x04
#define CONFIGURATION_GET                            0x05
#define CONFIGURATION_REPORT                         0x06
// Define for CONFIGURATION SET and REPORT
#define CONFIGURATION_SIZE_MASK                      0x07

/************ Alarm command class commands *************/
#define ALARM_VERSION                                0x01
#define ALARM_GET                                    0x04
#define ALARM_REPORT                                 0x05

/**** Manufacturer Specific command class commands *****/
#define MANUFACTURER_SPECIFIC_VERSION                0x01
#define MANUFACTURER_SPECIFIC_GET                    0x04
#define MANUFACTURER_SPECIFIC_REPORT                 0x05

/********* Powerlevel command class commands ***********/
#define POWERLEVEL_VERSION                           0x01
#define POWERLEVEL_SET                               0x01
#define POWERLEVEL_GET                               0x02
#define POWERLEVEL_REPORT                            0x03
#define POWERLEVEL_TEST_NODE_SET                     0x04
#define POWERLEVEL_TEST_NODE_GET                     0x05
#define POWERLEVEL_TEST_NODE_REPORT                  0x06
// Power level definitions
#define ZW_TEST_FAILED                               0x00
#define ZW_TEST_SUCCES                               0x01
#define ZW_TEST_INPROGRESS                           0x02
#define ZW_TEST_NOT_A_NODEID                         0x00

/********** Protection command class commands **********/
#define PROTECTION_VERSION                           0x01
#define PROTECTION_SET                               0x01
#define PROTECTION_GET                               0x02
#define PROTECTION_REPORT                            0x03
// Protection level definitions
#define PROTECTION_OFF                               0x00
#define PROTECTION_CHILDPROTECT                      0x01

/************* Lock command class commands *************/
#define LOCK_VERSION                                 0x01
#define LOCK_SET                                     0x01
#define LOCK_GET                                     0x02
#define LOCK_REPORT                                  0x03
// Lock definitions
#define LOCK_ON                                      0x01
#define LOCK_OFF                                     0x00

/********** Battery command class commands *************/
#define BATTERY_VERSION                              0x01
#define BATTERY_GET                                  0x02
#define BATTERY_REPORT                               0x03

/*********** Clock command class commands **************/
#define CLOCK_VERSION                                0x01
#define CLOCK_SET                                    0x04
#define CLOCK_GET                                    0x05
#define CLOCK_REPORT                                 0x06
// Clock mask definitions
#define CLOCK_WEEKDAY_MASK                           0xE0
#define CLOCK_WEEKDAY_SHIFT                          0x05
#define CLOCK_HOUR_MASK                              0x1F

/*********** Wake Up command class commands ************/
#define WAKE_UP_VERSION                              0x01
#define WAKE_UP_INTERVAL_SET                         0x04
#define WAKE_UP_INTERVAL_GET                         0x05
#define WAKE_UP_INTERVAL_REPORT                      0x06
#define WAKE_UP_NOTIFICATION                         0x07
#define WAKE_UP_NO_MORE_INFORMATION                  0x08

/************ Association command class ****************/
#define ASSOCIATION_VERSION                          0x01
#define ASSOCIATION_SET                              0x01
#define ASSOCIATION_GET                              0x02
#define ASSOCIATION_REPORT                           0x03
#define ASSOCIATION_REMOVE                           0x04
#define ASSOCIATION_GROUPINGS_GET                    0x05
#define ASSOCIATION_GROUPINGS_REPORT                 0x06

/*************** Version command class *****************/
#define VERSION_VERSION                              0x01
#define VERSION_GET                                  0x11
#define VERSION_REPORT                               0x12
#define VERSION_COMMAND_CLASS_GET                    0x13
#define VERSION_COMMAND_CLASS_REPORT                 0x14

/********** Indicator command class commands ***********/
#define INDICATOR_VERSION                            0x01
#define INDICATOR_SET                                0x01
#define INDICATOR_GET                                0x02
#define INDICATOR_REPORT                             0x03

/********* Proprietary command class commands **********/
#define PROPRIETARY_VERSION                          0x01
#define PROPRIETARY_SET                              0x01
#define PROPRIETARY_GET                              0x02
#define PROPRIETARY_REPORT                           0x03

/*********** Language command class commands ***********/
#define LANGUAGE_VERSION                             0x01
#define LANGUAGE_SET                                 0x01
#define LANGUAGE_GET                                 0x02
#define LANGUAGE_REPORT                              0x03

/******* Energy Production command class commands *******/
#define ENERGY_PRODUCTION_VERSION                    0x01
#define ENERGY_PRODUCTION_GET                        0x02
#define ENERGY_PRODUCTION_REPORT                     0x03

/****** Network Statistics command class commands ******/
#define NETWORK_STAT_VERSION                         0x01
#define NETWORK_STAT_SET                             0x01
#define NETWORK_STAT_GET                             0x02
#define NETWORK_STAT_REPORT                          0x03

/*********** Display command class commands ************/
#define DISPLAY_VERSION                              0x01
#define DISPLAY_SET                                  0x01
#define DISPLAY_SUPPORTED_GET                        0x02
#define DISPLAY_SUPPORTED_REPORT                     0x03

/************************************************************/
/* structs and unions that can be used by the application   */
/* to construct the frames to be sent                       */
/************************************************************/

/************************************************************/
/* Common for all command classes                           */
/************************************************************/
typedef struct _ZW_COMMON_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_COMMON_FRAME;

/************************************************************/
/* Basic command class structs                              */
/************************************************************/
typedef struct _ZW_BASIC_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BASIC_SET_FRAME;

typedef struct _ZW_BASIC_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BASIC_GET_FRAME;

typedef struct _ZW_BASIC_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BASIC_REPORT_FRAME;

/************************************************************/
/* Controller Replication command class structs             */
/************************************************************/
typedef struct _ZW_TRANSFER_GROUP_FRAME_
{
  BYTE        cmdClass;   // The command class
  BYTE        cmd;        // The command
  BYTE        seqNo;      // sequence number. Set by protocol
  BYTE        groupID;    // The ID of the group
  BYTE        nodeID;     // node id
} ZW_TRANSFER_GROUP_FRAME;

typedef struct _ZW_TRANSFER_GROUP_NAME_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        seqNo;                                      // sequence number. Set by protocol
  BYTE        ID;                                         // The ID of the scene or group
  BYTE        string[CTRL_REPLICATION_NAME_MAX_TXT_LEN];  // text string
} ZW_TRANSFER_GROUP_NAME_FRAME;

typedef struct _ZW_TRANSFER_SCENE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        seqNo;                                      // sequence number. Set by protocol
  BYTE        sceneID;                                    // The ID of the scene
  BYTE        nodeID;                                     // node id
  BYTE        dimLevel;                                   // The light level for this switch
} ZW_TRANSFER_SCENE_FRAME;

typedef struct _ZW_TRANSFER_SCENE_NAME_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        seqNo;                                      // sequence number. Set by protocol
  BYTE        ID;                                         // The ID of the scene or group
  BYTE        string[CTRL_REPLICATION_NAME_MAX_TXT_LEN];  // text string
} ZW_TRANSFER_SCENE_NAME_FRAME;

/************************************************************/
/* Application Status command class structs                 */
/************************************************************/
typedef struct _ZW_APPLICATION_BUSY_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        status;                                     // Application busy status 
  BYTE        waitTime;                                   // Wait before retrying
} ZW_APPLICATION_BUSY_FRAME;
typedef struct _ZW_APPLICATION_REJECTED_REQUEST_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        status;                                     // Application status for rejected request 
} ZW_APPLICATION_REJECTED_REQUEST_FRAME;

/************************************************************/
/* Binary Switch command class structs                      */
/************************************************************/
typedef struct _ZW_BINARY_SWITCH_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BINARY_SWITCH_SET_FRAME;

typedef struct _ZW_BINARY_SWITCH_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BINARY_SWITCH_GET_FRAME;

typedef struct _ZW_BINARY_SWITCH_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BINARY_SWITCH_REPORT_FRAME;

/************************************************************/
/* Multilevel Switch command class structs                  */
/************************************************************/
typedef struct _ZW_MULTILEVEL_SWITCH_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_MULTILEVEL_SWITCH_SET_FRAME;

typedef struct _ZW_MULTILEVEL_SWITCH_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_MULTILEVEL_SWITCH_GET_FRAME;

typedef struct _ZW_MULTILEVEL_SWITCH_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_MULTILEVEL_SWITCH_REPORT_FRAME;

typedef struct _ZW_MULTILEVEL_SWITCH_START_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        dir;
  BYTE        startValue;
} ZW_MULTILEVEL_SWITCH_START_CHANGE_FRAME;

typedef struct _ZW_MULTILEVEL_SWITCH_STOP_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_MULTILEVEL_SWITCH_STOP_CHANGE_FRAME;

typedef struct _ZW_MULTILEVEL_SWITCH_DO_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_MULTILEVEL_SWITCH_DO_CHANGE_FRAME;

/************************************************************/
/* All Switch command class structs                         */
/************************************************************/
typedef struct _ZW_ALL_SWITCH_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_ALL_SWITCH_SET_FRAME;

typedef struct _ZW_ALL_SWITCH_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_ALL_SWITCH_GET_FRAME;

typedef struct _ZW_ALL_SWITCH_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_ALL_SWITCH_REPORT_FRAME;

typedef struct _ZW_ALL_SWITCH_ON_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_ALL_SWITCH_ON_FRAME;

typedef struct _ZW_ALL_SWITCH_OFF_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_ALL_SWITCH_OFF_FRAME;

/************************************************************/
/* Binary Toggle Switch command class structs               */
/************************************************************/
typedef struct _ZW_BINARY_TOGGLE_SWITCH_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BINARY_TOGGLE_SWITCH_SET_FRAME;

typedef struct _ZW_BINARY_TOGGLE_SWITCH_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BINARY_TOGGLE_SWITCH_GET_FRAME;

typedef struct _ZW_BINARY_TOGGLE_SWITCH_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BINARY_TOGGLE_SWITCH_REPORT_FRAME;

/************************************************************/
/* Multilevel Toggle Switch command class structs           */
/************************************************************/
typedef struct _ZW_MULTILEVEL_TOGGLE_SWITCH_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_MULTILEVEL_TOGGLE_SWITCH_SET_FRAME;

typedef struct _ZW_MULTILEVEL_TOGGLE_SWITCH_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_MULTILEVEL_TOGGLE_SWITCH_GET_FRAME;

typedef struct _ZW_MULTILEVEL_TOGGLE_SWITCH_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_MULTILEVEL_TOGGLE_SWITCH_REPORT_FRAME;

typedef struct _ZW_MULTILEVEL_TOGGLE_SWITCH_START_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        state;
  BYTE        startValue;
} ZW_MULTILEVEL_TOGGLE_SWITCH_START_CHANGE_FRAME;

typedef struct _ZW_MULTILEVEL_TOGGLE_SWITCH_STOP_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_MULTILEVEL_TOGGLE_SWITCH_STOP_CHANGE_FRAME;

/************************************************************/
/* Chimney Fan command class structs                        */
/************************************************************/

/************************************************************/
/* Binary Sensor command class structs                      */
/************************************************************/
typedef struct _ZW_BINARY_SENSOR_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BINARY_SENSOR_GET_FRAME;

typedef struct _ZW_BINARY_SENSOR_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_BINARY_SENSOR_REPORT_FRAME;

/************************************************************/
/* Multilevel Sensor command class structs                  */
/************************************************************/
typedef struct _ZW_MULTILEVEL_SENSOR_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_MULTILEVEL_SENSOR_GET_FRAME;

typedef struct _ZW_MULTILEVEL_SENSOR_REPORT_1BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        sensorType;//
  BYTE        sensorValueFormat; //Byte containing format specifiers
  BYTE        sensorValue;
} ZW_MULTILEVEL_SENSOR_REPORT_1BYTE_FRAME;

typedef struct _ZW_MULTILEVEL_SENSOR_REPORT_2BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        sensorType;//
  BYTE        sensorValueFormat; //Byte containing format specifiers
  BYTE        sensorValue1;      /*MSB*/
  BYTE        sensorValue2;      /*LSB*/
} ZW_MULTILEVEL_SENSOR_REPORT_2BYTE_FRAME;

typedef struct _ZW_MULTILEVEL_SENSOR_REPORT_4BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        sensorType;//
  BYTE        sensorValueFormat; //Byte containing format specifiers
  BYTE        sensorValue1;     /*MSB*/
  BYTE        sensorValue2;
  BYTE        sensorValue3;
  BYTE        sensorValue4;     /*LSB*/
} ZW_MULTILEVEL_SENSOR_REPORT_4BYTE_FRAME;

/************************************************************/
/* Pulse Meter command class structs                        */
/************************************************************/
typedef struct _ZW_PULSE_METER_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_PULSE_METER_GET_FRAME;
typedef struct _ZW_PULSE_METER_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        pulseCount1; /*MSB*/
  BYTE        pulseCount2;
  BYTE        pulseCount3;
  BYTE        pulseCount4;
} ZW_PULSE_METER_REPORT_FRAME;

/************************************************************/
/* Thermostat Heating command class structs                 */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HEATING_MODE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        mode;
} ZW_THERMOSTAT_HEATING_MODE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_MODE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_HEATING_MODE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        mode;
} ZW_THERMOSTAT_HEATING_MODE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE        precisionScaleSize;
  BYTE        value1;
} ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE        precisionScaleSize;
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE        precisionScaleSize;
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
} ZW_THERMOSTAT_HEATING_SETPOINT_GET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE	      precisionScaleSize;
  BYTE        value1;
} ZW_THERMOSTAT_HEATING_SETPOINT_1BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE	      precisionScaleSize;
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_HEATING_SETPOINT_2BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setPointNr;
  BYTE	      precisionScaleSize;
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_HEATING_SETPOINT_4BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_HEATING_RELAY_STATUS_GET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        relayStatus;
} ZW_THERMOSTAT_HEATING_RELAY_STATUS_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        status;
} ZW_THERMOSTAT_HEATING_STATUS_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_HEATING_STATUS_GET_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        status;
} ZW_THERMOSTAT_HEATING_STATUS_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        hours;
  BYTE        minutes;
} ZW_THERMOSTAT_HEATING_TIMED_OFF_SET_FRAME;

/************************************************************/
/* Thermostat Mode command class structs                    */
/************************************************************/
typedef struct _ZW_THERMOSTAT_MODE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        mode;
} ZW_THERMOSTAT_MODE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_MODE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_MODE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_MODE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        mode;
} ZW_THERMOSTAT_MODE_REPORT_FRAME;

/************************************************************/
/* Thermostat Hold Mode command class structs               */
/************************************************************/
typedef struct _ZW_THERMOSTAT_HOLD_MODE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        holdMode;
} ZW_THERMOSTAT_HOLD_MODE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_HOLD_MODE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_HOLD_MODE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_HOLD_MODE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        holdMode;
} ZW_THERMOSTAT_HOLD_MODE_REPORT_FRAME;

/************************************************************/
/* Thermostat Operating State command class structs         */
/************************************************************/
typedef struct _ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_OPERATING_STATE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        operatingState;
} ZW_THERMOSTAT_OPERATING_STATE_REPORT_FRAME;

/************************************************************/
/* Thermostat Setpoint command class structs                */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SETPOINT_1BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;
} ZW_THERMOSTAT_SETPOINT_1BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_2BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_SETPOINT_2BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_4BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_SETPOINT_4BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
} ZW_THERMOSTAT_SETPOINT_GET_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_1BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;
} ZW_THERMOSTAT_SETPOINT_1BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_2BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_SETPOINT_2BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_SETPOINT_4BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_SETPOINT_4BYTE_REPORT_FRAME;

/************************************************************/
/* Thermostat Fan command class structs                     */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        fanMode;
} ZW_THERMOSTAT_FAN_SET_FRAME;

typedef struct _ZW_THERMOSTAT_FAN_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_FAN_GET_FRAME;

typedef struct _ZW_THERMOSTAT_FAN_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        fanMode;
} ZW_THERMOSTAT_FAN_REPORT_FRAME;

/************************************************************/
/* Thermostat Fan State command class structs               */
/************************************************************/
typedef struct _ZW_THERMOSTAT_FAN_STATE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_FAN_STATE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        fanOperatingState;
} ZW_THERMOSTAT_FAN_STATE_REPORT_FRAME;

/************************************************************/
/* Thermostat Schedule command class structs                */
/************************************************************/
typedef struct _ZW_THERMOSTAT_SCHEDULE_1BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1; 
} ZW_THERMOSTAT_SCHEDULE_1BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_2BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_SCHEDULE_2BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_4BYTE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_SCHEDULE_4BYTE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
} ZW_THERMOSTAT_SCHEDULE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_1BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1; 
} ZW_THERMOSTAT_SCHEDULE_1BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_2BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_THERMOSTAT_SCHEDULE_2BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_4BYTE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        setpointNr;
  BYTE        weekdayIndex; 
  BYTE        hour;
  BYTE        weekdayMinute;
  BYTE        precisionScaleSize; // Size of parameter should be 2
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_THERMOSTAT_SCHEDULE_4BYTE_REPORT_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_MODE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        scheduleMode;
} ZW_THERMOSTAT_SCHEDULE_MODE_SET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_MODE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_THERMOSTAT_SCHEDULE_MODE_GET_FRAME;

typedef struct _ZW_THERMOSTAT_SCHEDULE_MODE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        scheduleMode;
} ZW_THERMOSTAT_SCHEDULE_MODE_REPORT_FRAME;

/************************************************************/
/* Configuration command class structs                      */
/************************************************************/
typedef struct _ZW_CONFIGURATION_SET_1BYTE_FRAME_
{
  BYTE        cmdClass;                                   // GENERIC The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 1
  BYTE        cValue1;                                    // value of parm
} ZW_CONFIGURATION_SET_1BYTE_FRAME;

typedef struct _ZW_CONFIGURATION_SET_2BYTE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 2
  BYTE        cValue1;                                    // value of parm
  BYTE        cValue2;                                    // value of parm
} ZW_CONFIGURATION_SET_2BYTE_FRAME;

typedef struct _ZW_CONFIGURATION_SET_4BYTE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 4
  BYTE        cValue1;                                    // value of parm
  BYTE        cValue2;                                    // value of parm
  BYTE        cValue3;                                    // value of parm
  BYTE        cValue4;                                    // value of parm
} ZW_CONFIGURATION_SET_4BYTE_FRAME;

typedef struct _ZW_CONFIGURATION_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to get
} ZW_CONFIGURATION_GET_FRAME;

typedef struct _ZW_CONFIGURATION_REPORT_1BYTE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 1
  BYTE        cValue1;                                    // value of parm
} ZW_CONFIGURATION_REPORT_1BYTE_FRAME;

typedef struct _ZW_CONFIGURATION_REPORT_2BYTE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 2
  BYTE        cValue1;                                    // value of parm
  BYTE        cValue2;                                    // value of parm
} ZW_CONFIGURATION_REPORT_2BYTE_FRAME;

typedef struct _ZW_CONFIGURATION_REPORT_4BYTE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        parmNumber;                                 // Parameter number to set
  BYTE        size;                                       // Size of parameter should be 4
  BYTE        cValue1;                                    // value of parm
  BYTE        cValue2;                                    // value of parm
  BYTE        cValue3;                                    // value of parm
  BYTE        cValue4;                                    // value of parm
} ZW_CONFIGURATION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Alarm command class structs                              */
/************************************************************/
typedef struct _ZW_ALARM_GET_FRAME_
{
  BYTE        cmdClass;       // The command class
  BYTE        cmd;            // The command
  BYTE        alarmType;
} ZW_ALARM_GET_FRAME;

typedef struct  _ZW_ALARM_REPORT_FRAME_
{
  BYTE        cmdClass;       // The command class
  BYTE        cmd;            // The command
  BYTE        alarmType;
  BYTE        alarmLevel;
} ZW_ALARM_REPORT_FRAME;

/************************************************************/
/* Manufacturer Specific command class structs              */
/************************************************************/
typedef struct _ZW_MANUFACTURER_SPECIFIC_GET_FRAME_
{
  BYTE        cmdClass;       // The command class
  BYTE        cmd;            // The command
} ZW_MANUFACTURER_SPECIFIC_GET_FRAME;

typedef struct  _ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME_
{
  BYTE        cmdClass;       // The command class
  BYTE        cmd;            // The command
  BYTE        manufacturerID1;
  BYTE        manufacturerID2;
  BYTE        productType1;
  BYTE        productType2;
  BYTE        productID1;
  BYTE        productID2;
} ZW_MANUFACTURER_SPECIFIC_REPORT_FRAME;

/************************************************************/
/* Power Level command class structs                        */
/************************************************************/
typedef struct _ZW_POWERLEVEL_TEST_NODE_SET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        nodeId;                                     // node Id to transmit to
  BYTE        powerLevel;                                 // Power level to test with
  BYTE        testFrameCountMsb;                          // Number of frames to transmit
  BYTE        testFrameCountLsb;
} ZW_POWERLEVEL_TEST_NODE_SET_FRAME;

typedef struct _ZW_POWERLEVEL_TEST_NODE_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_POWERLEVEL_TEST_NODE_GET_FRAME;

typedef struct _ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        nodeId;                                     // Test node Id
  BYTE        testStatus;                                 // test status
  BYTE        testFrameCountMsb;                          // Number of frames acked
  BYTE        testFrameCountLsb;
} ZW_POWERLEVEL_TEST_NODE_REPORT_FRAME;

typedef struct _ZW_POWERLEVEL_SET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        powerLevel;                                 // Power level to use
  BYTE        timeout;                                    // timeout
} ZW_POWERLEVEL_SET_FRAME;

typedef struct _ZW_POWERLEVEL_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_POWERLEVEL_GET_FRAME;

typedef struct _ZW_POWERLEVEL_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        powerLevel;                                 // Power used
  BYTE        timeout;                                    // timeout left
} ZW_POWERLEVEL_REPORT_FRAME;

/************************************************************/
/* Protection command class structs                         */
/************************************************************/
typedef struct _ZW_PROTECTION_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_PROTECTION_GET_FRAME;

typedef struct _ZW_PROTECTION_SET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        protectionState;
} ZW_PROTECTION_SET_FRAME;

typedef struct _ZW_PROTECTION_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        protectionState;
} ZW_PROTECTION_REPORT_FRAME;


/************************************************************/
/* Lock command class structs                               */
/************************************************************/
typedef struct _ZW_LOCK_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_LOCK_GET_FRAME;

typedef struct _ZW_LOCK_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        lockState;
} ZW_LOCK_REPORT_FRAME;

typedef struct _ZW_LOCK_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        lockState;
} ZW_LOCK_SET_FRAME;

/************************************************************/
/* Battery command class structs                            */
/************************************************************/
typedef struct _ZW_BATTERY_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_BATTERY_GET_FRAME;

typedef struct _ZW_BATTERY_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        batteryLevel;
} ZW_BATTERY_REPORT_FRAME;

/************************************************************/
/* Clock command class structs                              */
/************************************************************/
                              /*BYTE dayHour format is: */
                              /* y = hours, x = day bit*/
                              /* xxxy yyyy */
#define CLOCK_DAY_NOT_USED  0x00
#define CLOCK_MONDAY        0x20  /* 0010 0000 */
#define CLOCK_TUESDAY       0x40  /* 0100 0000 */
#define CLOCK_WEDNESDAY     0x60  /* 0110 0000 */
#define CLOCK_THURSDAY      0x80  /* 1000 0000 */
#define CLOCK_FRIDAY        0x90  /* 1010 0000 */
#define CLOCK_SATURDAY      0xC0  /* 1100 0000 */
#define CLOCK_SUNDAY        0xE0  /* 1110 0000 */

#define SET_CLOCK_HOUR(var,hours) var&=(0xE0|hours)
#define SET_CLOCK_DAY(var,weekday) var&=(0x1F|weekday)
#define GET_CLOCK_DAY(var)  (var&0xE0)
#define GET_CLOCK_HOUR(var) (var&0x1F)

typedef struct _ZW_CLOCK_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_CLOCK_GET_FRAME;

typedef struct _ZW_CLOCK_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        dayHour;
  BYTE        minute;
} ZW_CLOCK_SET_FRAME;

typedef struct _ZW_CLOCK_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        dayHour;
  BYTE        minute;
} ZW_CLOCK_REPORT_FRAME;

/************************************************************/
/* Wake Up command class structs                            */
/************************************************************/
typedef struct _ZW_WAKE_UP_SET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        sec1;                                       // number of seconds MSB
  BYTE        sec2;                                       // number of seconds
  BYTE        sec3;                                       // number of seconds LSB
  BYTE        nodeId;                                     // Node ID which should receive the notification
} ZW_WAKE_UP_SET_FRAME;

typedef struct _ZW_WAKE_UP_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_WAKE_UP_GET_FRAME;

typedef struct _ZW_WAKE_UP_INTERVAL_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        sec1;                                       // number of seconds MSB
  BYTE        sec2;                                       // number of seconds
  BYTE        sec3;                                       // number of seconds LSB
  BYTE        nodeId;                                     // Node ID which should receive the notification
} ZW_WAKE_UP_INTERVAL_REPORT_FRAME;

typedef struct _ZW_WAKE_UP_NOTIFICATION_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_WAKE_UP_NOTIFICATION_FRAME;

/************************************************************/
/* Association command class structs                        */
/************************************************************/
typedef struct _ZW_ASSOCIATION_SET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        groupId;                                    // The grouping identifier
  BYTE        nodeId;                                     // The Node Id
} ZW_ASSOCIATION_SET_FRAME;

typedef struct _ZW_ASSOCIATION_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        groupId;                                    // The grouping identifier
} ZW_ASSOCIATION_GET_FRAME;

typedef struct _ZW_ASSOCIATION_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        groupId;                                    // The grouping identifier
  BYTE        maxNodesSupported;                          // The max nodes supported
  BYTE        totalGroups;                                // The total groupings
  BYTE        reportsToFollow;                            // Reports to follow
  BYTE        nodeId;                                     // The Node Id
} ZW_ASSOCIATION_REPORT_FRAME;

typedef struct _ZW_ASSOCIATION_REMOVE_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        groupId;                                    // The grouping identifier
  BYTE        nodeId;                                     // The Node Id
} ZW_ASSOCIATION_REMOVE_FRAME;

typedef struct _ZW_ASSOCIATION_GROUPINGS_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_ASSOCIATION_GROUPINGS_GET_FRAME;

typedef struct _ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        supportedGroupings;                         // The number of groupings that this node supports
} ZW_ASSOCIATION_GROUPINGS_REPORT_FRAME;

/************************************************************/
/* Version command class structs                            */
/************************************************************/
typedef struct _ZW_VERSION_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
} ZW_VERSION_GET_FRAME;

typedef struct _ZW_VERSION_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        libType;                                    // The library type
  BYTE        proVersion;                                 // The protocol version
  BYTE        proSubVersion;                              // The protocol subversion
  BYTE        appVersion;                                 // The application version
  BYTE        appSubVersion;                              // The application subversion
} ZW_VERSION_REPORT_FRAME;

typedef struct _ZW_VERSION_COMMAND_GET_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        cmdReq;                                     // The command class requested
} ZW_VERSION_COMMAND_GET_FRAME;

typedef struct _ZW_VERSION_COMMAND_REPORT_FRAME_
{
  BYTE        cmdClass;                                   // The command class
  BYTE        cmd;                                        // The command
  BYTE        cmdReq;                                     // The command class requested
  BYTE        cmdVersion;                                 // The command class version
} ZW_VERSION_COMMAND_REPORT_FRAME;

/************************************************************/
/* Indicator command class structs                          */
/************************************************************/
typedef struct _ZW_INDICATOR_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_INDICATOR_SET_FRAME;

typedef struct _ZW_INDICATOR_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_INDICATOR_GET_FRAME;

typedef struct _ZW_INDICATOR_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value;
} ZW_INDICATOR_REPORT_FRAME;

/************************************************************/
/* Proprietary command class structs                        */
/************************************************************/
typedef struct _ZW_PROPRIETARY_SET_2BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value1;   
  BYTE        value2;    
} ZW_PROPRIETARY_SET_2BYTE_FRAME;

typedef struct _ZW_PROPRIETARY_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_PROPRIETARY_GET_FRAME;

typedef struct _ZW_PROPRIETARY_REPORT_2BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        value1;   
  BYTE        value2;    
} ZW_PROPRIETARY_REPORT_2BYTE_FRAME;

/************************************************************/
/* Language command class structs                           */
/************************************************************/
typedef struct _ZW_LANGUAGE_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        language1;   
  BYTE        language2;   
  BYTE        language3;   
  BYTE        country1;    
  BYTE        country2;    
} ZW_LANGUAGE_SET_FRAME;

typedef struct _ZW_LANGUAGE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_LANGUAGE_GET_FRAME;

typedef struct _ZW_LANGUAGE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        language1;   
  BYTE        language2;   
  BYTE        language3;   
  BYTE        country1;    
  BYTE        country2;    
} ZW_LANGUAGE_REPORT_FRAME;

/************************************************************/
/* Energy Production command class structs                  */
/************************************************************/
typedef struct _ZW_ENERGY_PRODUCTION_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        parameterNr;
} ZW_ENERGY_PRODUCTION_GET_FRAME;

typedef struct _ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        parameterNr;
  BYTE	      precisionScaleSize; // Size of parameter should be 4
  BYTE        value1;
} ZW_ENERGY_PRODUCTION_REPORT_1BYTE_FRAME;

typedef struct _ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        parameterNr;
  BYTE	      precisionScaleSize; // Size of parameter should be 4
  BYTE        value1;    // MSB
  BYTE        value2;    // LSB
} ZW_ENERGY_PRODUCTION_REPORT_2BYTE_FRAME;

typedef struct _ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        parameterNr;
  BYTE	      precisionScaleSize; // Size of parameter should be 4
  BYTE        value1;    // MSB
  BYTE        value2;
  BYTE        value3;
  BYTE        value4;    // LSB
} ZW_ENERGY_PRODUCTION_REPORT_4BYTE_FRAME;

/************************************************************/
/* Network Statistics command class structs                 */
/************************************************************/
typedef struct _ZW_NETWORK_STAT_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        state;     // State of network statistics
} ZW_NETWORK_STAT_SET_FRAME;

typedef struct _ZW_NETWORK_STAT_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_NETWORK_STAT_GET_FRAME;

typedef struct _ZW_NETWORK_STAT_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        tries1;    // Number of tries (MSB)
  BYTE        tries2;   
  BYTE        tries3;   
  BYTE        tries4;    // LSB
  BYTE        failed1;   // Number of failed tries (MSB)
  BYTE        failed2;   
  BYTE        failed3;   
  BYTE        failed4;   //LSB 
} ZW_NETWORK_STAT_REPORT_FRAME;

/************************************************************/
/* Garage Door command class structs                        */
/************************************************************/
typedef struct _ZW_GARAGE_DOOR_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_GARAGE_DOOR_GET_FRAME;

typedef struct _ZW_GARAGE_DOOR_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        state;
} ZW_GARAGE_DOOR_REPORT_FRAME;

typedef struct _ZW_GARAGE_DOOR_START_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        dirAuth;   // No authentication
} ZW_GARAGE_DOOR_START_CHANGE_FRAME;

typedef struct _ZW_GARAGE_DOOR_STOP_CHANGE_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        auth;  
} ZW_GARAGE_DOOR_STOP_CHANGE_FRAME;

/************************************************************/
/* Display command class structs                            */
/************************************************************/
typedef struct _ZW_DISPLAY_SET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        clearLineNumber;
  BYTE        charPosition;
  BYTE        char1;
} ZW_DISPLAY_SET_FRAME;

typedef struct _ZW_DISPLAY_SUPPORTED_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
} ZW_DISPLAY_SUPPORTED_GET_FRAME;

typedef struct _ZW_DISPLAY_SUPPORTED_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        numberOfLines;    
  BYTE        charPerLine;   
  BYTE        sizeOfLineBuffer;   
} ZW_DISPLAY_SUPPORTED_REPORT_FRAME;

/************************************************************/
/* Multi Instance command class structs                     */
/************************************************************/
typedef struct _ZW_MULTI_INSTANCE_GET_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        cmdClassToGet;
} ZW_MULTI_INSTANCE_GET_FRAME;

typedef struct _ZW_MULTI_INSTANCE_REPORT_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        cmdClassReported;   /*type of command class reported */
  BYTE        cmdClassCount;      /*Number of instances that support this commandclass*/
} ZW_MULTI_INSTANCE_REPORT_FRAME;

typedef union _ALL_EXCEPT_MULTI
{
  ZW_FRAME_COLLECTION_MACRO
} ALL_EXCEPT_MULTI;

typedef struct _ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME_
{
  BYTE        cmdClass;  // The command class
  BYTE        cmd;       // The command
  BYTE        instance;  // The instance to access
  ALL_EXCEPT_MULTI     encapFrame;
} ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME;

/************************************************************/
/* Union of all command classes                            */
/************************************************************/
typedef union _ZW_APPLICATION_TX_BUFFER_
{
  ZW_MULTI_INSTANCE_CMD_ENCAP_FRAME       ZW_MultiInstanceCmdEncapFrame;
  ZW_FRAME_COLLECTION_MACRO
} ZW_APPLICATION_TX_BUFFER;


#endif
