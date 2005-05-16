#ifndef __Define_Event_H__
#define __Define_Event_H__

#define EVENT_Interval_Timer_CONST -1
#define EVENT_Touch_or_click_CONST 1
#define EVENT_Playback_Info_Changed_CONST 2
#define EVENT_Media_Inserted_CONST 3
#define EVENT_Mobile_orbiter_detected_CONST 4
#define EVENT_Mobile_orbiter_linked_CONST 5
#define EVENT_Mobile_orbiter_lost_CONST 6
#define EVENT_Menu_Onscreen_CONST 7
#define EVENT_Error_Occured_CONST 8
#define EVENT_Sensor_Tripped_CONST 9
#define EVENT_Learned_IR_code_CONST 10
#define EVENT_PVR_Rec_Sched_Conflict_CONST 11
#define EVENT_Playback_Completed_CONST 12
#define EVENT_PBX_CommandResult_CONST 14
#define EVENT_PBX_Ring_CONST 15
#define EVENT_Security_Breach_CONST 16
#define EVENT_Fire_Alarm_CONST 17
#define EVENT_Reset_Alarm_CONST 18
#define EVENT_Watching_Media_CONST 19
#define EVENT_Stopped_Watching_Media_CONST 20
#define EVENT_Listening_to_Media_CONST 21
#define EVENT_Stopped_Listening_To_Medi_CONST 22
#define EVENT_MythTV_Channel_Changed_CONST 23
#define EVENT_New_PNP_Device_Detected_CONST 24
#define EVENT_Temperature_Changed_CONST 25
#define EVENT_Humidity_Changed_CONST 26
#define EVENT_Thermostat_Set_Point_Chan_CONST 27
#define EVENT_On_CONST 28
#define EVENT_Off_CONST 29
#define EVENT_Follow_Me_Lighting_CONST 30
#define EVENT_Follow_Me_Climate_CONST 31
#define EVENT_Follow_Me_Media_CONST 32
#define EVENT_Follow_Me_Telecom_CONST 33
#define EVENT_Follow_Me_Security_CONST 34
#define EVENT_Ripping_Completed_CONST 35
#define EVENT_Incoming_Call_CONST 36
#define EVENT_Voice_Mail_Changed_CONST 37
#define EVENT_Reload_Aborted_CONST 38
#define EVENT_Air_Quality_CONST 39
#define EVENT_Doorbell_CONST 40
#define EVENT_Monitor_Mode_CONST 41
#define EVENT_Movement_Detected_CONST 42
#define EVENT_House_Mode_Changed_CONST 43

#define EVENT_TABLE "Event"
#define EVENT_PK_EVENT_FIELD "PK_Event"
#define EVENT_PK_EVENT_TABLE_FIELD "Event.PK_Event"
#define EVENT_FK_EVENTCATEGORY_FIELD "FK_EventCategory"
#define EVENT_FK_EVENTCATEGORY_TABLE_FIELD "Event.FK_EventCategory"
#define EVENT_DESCRIPTION_FIELD "Description"
#define EVENT_DESCRIPTION_TABLE_FIELD "Event.Description"
#define EVENT_DEFINE_FIELD "Define"
#define EVENT_DEFINE_TABLE_FIELD "Event.Define"
#define EVENT_PSC_ID_FIELD "psc_id"
#define EVENT_PSC_ID_TABLE_FIELD "Event.psc_id"
#define EVENT_PSC_BATCH_FIELD "psc_batch"
#define EVENT_PSC_BATCH_TABLE_FIELD "Event.psc_batch"
#define EVENT_PSC_USER_FIELD "psc_user"
#define EVENT_PSC_USER_TABLE_FIELD "Event.psc_user"
#define EVENT_PSC_FROZEN_FIELD "psc_frozen"
#define EVENT_PSC_FROZEN_TABLE_FIELD "Event.psc_frozen"
#define EVENT_PSC_MOD_FIELD "psc_mod"
#define EVENT_PSC_MOD_TABLE_FIELD "Event.psc_mod"


#endif

