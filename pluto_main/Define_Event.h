#ifndef __Define_Event_H__
#define __Define_Event_H__

#define EVENT_Touch_or_click_CONST 1
#define EVENT_Playback_Events_CONST 2
#define EVENT_Media_Inserted_CONST 3
#define EVENT_Mobile_orbiter_detected_CONST 4
#define EVENT_Mobile_orbiter_linked_CONST 5
#define EVENT_Mobile_orbiter_lost_CONST 6
#define EVENT_Menu_Onscreen_CONST 7

#define EVENT_TABLE "Event"
#define EVENT_PK_EVENT_FIELD "PK_Event"
#define EVENT_PK_EVENT_TABLE_FIELD "Event.PK_Event"
#define EVENT_FK_EVENTCATEGORY_FIELD "FK_EventCategory"
#define EVENT_FK_EVENTCATEGORY_TABLE_FIELD "Event.FK_EventCategory"
#define EVENT_DESCRIPTION_FIELD "Description"
#define EVENT_DESCRIPTION_TABLE_FIELD "Event.Description"
#define EVENT_DEFINE_FIELD "Define"
#define EVENT_DEFINE_TABLE_FIELD "Event.Define"
#define EVENT_MODIFICATION_RECORDINFO_FIELD "Modification_RecordInfo"
#define EVENT_MODIFICATION_RECORDINFO_TABLE_FIELD "Event.Modification_RecordInfo"
#define EVENT_ISNEW_RECORDINFO_FIELD "IsNew_RecordInfo"
#define EVENT_ISNEW_RECORDINFO_TABLE_FIELD "Event.IsNew_RecordInfo"
#define EVENT_ISDELETED_RECORDINFO_FIELD "IsDeleted_RecordInfo"
#define EVENT_ISDELETED_RECORDINFO_TABLE_FIELD "Event.IsDeleted_RecordInfo"
#define EVENT_FK_USERS_RECORDINFO_FIELD "FK_Users_RecordInfo"
#define EVENT_FK_USERS_RECORDINFO_TABLE_FIELD "Event.FK_Users_RecordInfo"


#endif

