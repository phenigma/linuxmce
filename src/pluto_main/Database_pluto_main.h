#ifndef __Database_pluto_main_H_
#define __Database_pluto_main_H_
#include <mysql.h>
#include "PlutoUtils/MySQLHelper.h"
#ifdef WIN32
#ifdef EXPORT_DLL
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllexport)
	#endif
#else
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllimport)
	#endif
#endif
#else
#define DECLSPECIFIER
#endif
class DECLSPECIFIER MySqlHelper;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Database_pluto_main: public MySqlHelper
{
public:
Database_pluto_main();
~Database_pluto_main();
void DeleteAllTables();
private:
class Table_Array* tblArray;
class Table_Broadcast* tblBroadcast;
class Table_Button* tblButton;
class Table_CachedScreens* tblCachedScreens;
class Table_CannedEvents* tblCannedEvents;
class Table_CannedEvents_CriteriaParmList* tblCannedEvents_CriteriaParmList;
class Table_Command* tblCommand;
class Table_CommandCategory* tblCommandCategory;
class Table_CommandGroup* tblCommandGroup;
class Table_CommandGroup_Command* tblCommandGroup_Command;
class Table_CommandGroup_Command_CommandParameter* tblCommandGroup_Command_CommandParameter;
class Table_CommandGroup_D* tblCommandGroup_D;
class Table_CommandGroup_D_Command* tblCommandGroup_D_Command;
class Table_CommandGroup_D_Command_CommandParameter* tblCommandGroup_D_Command_CommandParameter;
class Table_CommandGroup_EntertainArea* tblCommandGroup_EntertainArea;
class Table_CommandGroup_Room* tblCommandGroup_Room;
class Table_CommandParameter* tblCommandParameter;
class Table_Command_CommandParameter* tblCommand_CommandParameter;
class Table_Command_Pipe* tblCommand_Pipe;
class Table_ConfigType* tblConfigType;
class Table_ConfigType_File* tblConfigType_File;
class Table_ConfigType_Setting* tblConfigType_Setting;
class Table_ConfigType_Token* tblConfigType_Token;
class Table_ConnectorType* tblConnectorType;
class Table_Country* tblCountry;
class Table_Criteria* tblCriteria;
class Table_CriteriaList* tblCriteriaList;
class Table_CriteriaList_CriteriaParmList* tblCriteriaList_CriteriaParmList;
class Table_CriteriaParm* tblCriteriaParm;
class Table_CriteriaParmList* tblCriteriaParmList;
class Table_CriteriaParmNesting* tblCriteriaParmNesting;
class Table_CriteriaParmNesting_D* tblCriteriaParmNesting_D;
class Table_CriteriaParm_D* tblCriteriaParm_D;
class Table_Criteria_D* tblCriteria_D;
class Table_DHCPDevice* tblDHCPDevice;
class Table_DHCPDevice_DeviceData* tblDHCPDevice_DeviceData;
class Table_DataGrid* tblDataGrid;
class Table_DesignObj* tblDesignObj;
class Table_DesignObjCategory* tblDesignObjCategory;
class Table_DesignObjParameter* tblDesignObjParameter;
class Table_DesignObjType* tblDesignObjType;
class Table_DesignObjType_DesignObjParameter* tblDesignObjType_DesignObjParameter;
class Table_DesignObjVariation* tblDesignObjVariation;
class Table_DesignObjVariation_DesignObj* tblDesignObjVariation_DesignObj;
class Table_DesignObjVariation_DesignObjParameter* tblDesignObjVariation_DesignObjParameter;
class Table_DesignObjVariation_Text* tblDesignObjVariation_Text;
class Table_DesignObjVariation_Text_Skin_Language* tblDesignObjVariation_Text_Skin_Language;
class Table_DesignObjVariation_Zone* tblDesignObjVariation_Zone;
class Table_Device* tblDevice;
class Table_DeviceCategory* tblDeviceCategory;
class Table_DeviceCategory_DeviceData* tblDeviceCategory_DeviceData;
class Table_DeviceCategory_Event* tblDeviceCategory_Event;
class Table_DeviceCommandGroup* tblDeviceCommandGroup;
class Table_DeviceCommandGroup_Command* tblDeviceCommandGroup_Command;
class Table_DeviceCommandGroup_DeviceCommandGroup_Parent* tblDeviceCommandGroup_DeviceCommandGroup_Parent;
class Table_DeviceData* tblDeviceData;
class Table_DeviceGroup* tblDeviceGroup;
class Table_DeviceTemplate* tblDeviceTemplate;
class Table_DeviceTemplate_AV* tblDeviceTemplate_AV;
class Table_DeviceTemplate_DSPMode* tblDeviceTemplate_DSPMode;
class Table_DeviceTemplate_DesignObj* tblDeviceTemplate_DesignObj;
class Table_DeviceTemplate_DeviceCategory_ControlledVia* tblDeviceTemplate_DeviceCategory_ControlledVia;
class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe;
class Table_DeviceTemplate_DeviceCommandGroup* tblDeviceTemplate_DeviceCommandGroup;
class Table_DeviceTemplate_DeviceData* tblDeviceTemplate_DeviceData;
class Table_DeviceTemplate_DeviceTemplate_ControlledVia* tblDeviceTemplate_DeviceTemplate_ControlledVia;
class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe* tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe;
class Table_DeviceTemplate_DeviceTemplate_Related* tblDeviceTemplate_DeviceTemplate_Related;
class Table_DeviceTemplate_Event* tblDeviceTemplate_Event;
class Table_DeviceTemplate_InfraredGroup* tblDeviceTemplate_InfraredGroup;
class Table_DeviceTemplate_Input* tblDeviceTemplate_Input;
class Table_DeviceTemplate_MediaType* tblDeviceTemplate_MediaType;
class Table_DeviceTemplate_MediaType_DesignObj* tblDeviceTemplate_MediaType_DesignObj;
class Table_DeviceTemplate_Output* tblDeviceTemplate_Output;
class Table_DeviceTemplate_PageSetup* tblDeviceTemplate_PageSetup;
class Table_Device_Command* tblDevice_Command;
class Table_Device_CommandGroup* tblDevice_CommandGroup;
class Table_Device_DeviceData* tblDevice_DeviceData;
class Table_Device_DeviceGroup* tblDevice_DeviceGroup;
class Table_Device_Device_Pipe* tblDevice_Device_Pipe;
class Table_Device_Device_Related* tblDevice_Device_Related;
class Table_Device_EntertainArea* tblDevice_EntertainArea;
class Table_Device_HouseMode* tblDevice_HouseMode;
class Table_Device_MRU* tblDevice_MRU;
class Table_Device_Orbiter* tblDevice_Orbiter;
class Table_Device_QuickStart* tblDevice_QuickStart;
class Table_Device_StartupScript* tblDevice_StartupScript;
class Table_Device_Users* tblDevice_Users;
class Table_Direction* tblDirection;
class Table_Directory* tblDirectory;
class Table_Distro* tblDistro;
class Table_Document* tblDocument;
class Table_Document_Comment* tblDocument_Comment;
class Table_EntertainArea* tblEntertainArea;
class Table_Event* tblEvent;
class Table_EventCategory* tblEventCategory;
class Table_EventHandler* tblEventHandler;
class Table_EventParameter* tblEventParameter;
class Table_Event_EventParameter* tblEvent_EventParameter;
class Table_FAQ* tblFAQ;
class Table_Firewall* tblFirewall;
class Table_Floorplan* tblFloorplan;
class Table_FloorplanObjectType* tblFloorplanObjectType;
class Table_FloorplanObjectType_Color* tblFloorplanObjectType_Color;
class Table_FloorplanType* tblFloorplanType;
class Table_HorizAlignment* tblHorizAlignment;
class Table_HouseMode* tblHouseMode;
class Table_Household* tblHousehold;
class Table_Household_Installation* tblHousehold_Installation;
class Table_Icon* tblIcon;
class Table_Image* tblImage;
class Table_InfraredGroup* tblInfraredGroup;
class Table_InfraredGroup_Command* tblInfraredGroup_Command;
class Table_InfraredGroup_Command_Preferred* tblInfraredGroup_Command_Preferred;
class Table_InstallWizard* tblInstallWizard;
class Table_InstallWizard_Distro* tblInstallWizard_Distro;
class Table_Installation* tblInstallation;
class Table_Installation_RepositorySource_URL* tblInstallation_RepositorySource_URL;
class Table_Installation_Users* tblInstallation_Users;
class Table_Language* tblLanguage;
class Table_License* tblLicense;
class Table_Licensing* tblLicensing;
class Table_Manufacturer* tblManufacturer;
class Table_MediaType* tblMediaType;
class Table_MediaType_Broadcast* tblMediaType_Broadcast;
class Table_MediaType_DesignObj* tblMediaType_DesignObj;
class Table_News* tblNews;
class Table_OperatingSystem* tblOperatingSystem;
class Table_Orbiter* tblOrbiter;
class Table_Orbiter_Users_PasswordReq* tblOrbiter_Users_PasswordReq;
class Table_Orbiter_Variable* tblOrbiter_Variable;
class Table_Package* tblPackage;
class Table_PackageType* tblPackageType;
class Table_Package_Compat* tblPackage_Compat;
class Table_Package_Device* tblPackage_Device;
class Table_Package_Directory* tblPackage_Directory;
class Table_Package_Directory_File* tblPackage_Directory_File;
class Table_Package_Package* tblPackage_Package;
class Table_Package_Source* tblPackage_Source;
class Table_Package_Source_Compat* tblPackage_Source_Compat;
class Table_Package_Users* tblPackage_Users;
class Table_Package_Version* tblPackage_Version;
class Table_PageSetup* tblPageSetup;
class Table_PaidLicense* tblPaidLicense;
class Table_ParameterType* tblParameterType;
class Table_PhoneLineType* tblPhoneLineType;
class Table_Pipe* tblPipe;
class Table_RemoteControl* tblRemoteControl;
class Table_RepositorySource* tblRepositorySource;
class Table_RepositorySource_URL* tblRepositorySource_URL;
class Table_RepositoryType* tblRepositoryType;
class Table_Room* tblRoom;
class Table_RoomType* tblRoomType;
class Table_Room_Users* tblRoom_Users;
class Table_Schema* tblSchema;
class Table_SetupStep* tblSetupStep;
class Table_Size* tblSize;
class Table_Skin* tblSkin;
class Table_StabilityStatus* tblStabilityStatus;
class Table_StartupScript* tblStartupScript;
class Table_Style* tblStyle;
class Table_StyleVariation* tblStyleVariation;
class Table_System* tblSystem;
class Table_Template* tblTemplate;
class Table_Text* tblText;
class Table_TextCategory* tblTextCategory;
class Table_Text_LS* tblText_LS;
class Table_Text_LS_AltVersions* tblText_LS_AltVersions;
class Table_UnknownDevices* tblUnknownDevices;
class Table_UserMode* tblUserMode;
class Table_Users* tblUsers;
class Table_Variable* tblVariable;
class Table_Version* tblVersion;
class Table_VertAlignment* tblVertAlignment;
class Table_groupaddresses* tblgroupaddresses;
class Table_psc_constants_batdet* tblpsc_constants_batdet;
class Table_psc_constants_bathdr* tblpsc_constants_bathdr;
class Table_psc_constants_batuser* tblpsc_constants_batuser;
class Table_psc_constants_repset* tblpsc_constants_repset;
class Table_psc_constants_schema* tblpsc_constants_schema;
class Table_psc_constants_tables* tblpsc_constants_tables;
class Table_psc_dce_batdet* tblpsc_dce_batdet;
class Table_psc_dce_bathdr* tblpsc_dce_bathdr;
class Table_psc_dce_batuser* tblpsc_dce_batuser;
class Table_psc_dce_repset* tblpsc_dce_repset;
class Table_psc_dce_schema* tblpsc_dce_schema;
class Table_psc_dce_tables* tblpsc_dce_tables;
class Table_psc_designer_batdet* tblpsc_designer_batdet;
class Table_psc_designer_bathdr* tblpsc_designer_bathdr;
class Table_psc_designer_batuser* tblpsc_designer_batuser;
class Table_psc_designer_repset* tblpsc_designer_repset;
class Table_psc_designer_schema* tblpsc_designer_schema;
class Table_psc_designer_tables* tblpsc_designer_tables;
class Table_psc_document_batdet* tblpsc_document_batdet;
class Table_psc_document_bathdr* tblpsc_document_bathdr;
class Table_psc_document_batuser* tblpsc_document_batuser;
class Table_psc_document_repset* tblpsc_document_repset;
class Table_psc_document_schema* tblpsc_document_schema;
class Table_psc_document_tables* tblpsc_document_tables;
class Table_psc_ir_batdet* tblpsc_ir_batdet;
class Table_psc_ir_bathdr* tblpsc_ir_bathdr;
class Table_psc_ir_batuser* tblpsc_ir_batuser;
class Table_psc_ir_repset* tblpsc_ir_repset;
class Table_psc_ir_schema* tblpsc_ir_schema;
class Table_psc_ir_tables* tblpsc_ir_tables;
class Table_psc_local_batdet* tblpsc_local_batdet;
class Table_psc_local_bathdr* tblpsc_local_bathdr;
class Table_psc_local_batuser* tblpsc_local_batuser;
class Table_psc_local_repset* tblpsc_local_repset;
class Table_psc_local_schema* tblpsc_local_schema;
class Table_psc_local_tables* tblpsc_local_tables;
class Table_psc_website_batdet* tblpsc_website_batdet;
class Table_psc_website_bathdr* tblpsc_website_bathdr;
class Table_psc_website_batuser* tblpsc_website_batuser;
class Table_psc_website_repset* tblpsc_website_repset;
class Table_psc_website_schema* tblpsc_website_schema;
class Table_psc_website_tables* tblpsc_website_tables;
public:
class Table_Array* Array_get() { if( !tblArray ) CreateTable_Array(); return tblArray; }
class Table_Broadcast* Broadcast_get() { if( !tblBroadcast ) CreateTable_Broadcast(); return tblBroadcast; }
class Table_Button* Button_get() { if( !tblButton ) CreateTable_Button(); return tblButton; }
class Table_CachedScreens* CachedScreens_get() { if( !tblCachedScreens ) CreateTable_CachedScreens(); return tblCachedScreens; }
class Table_CannedEvents* CannedEvents_get() { if( !tblCannedEvents ) CreateTable_CannedEvents(); return tblCannedEvents; }
class Table_CannedEvents_CriteriaParmList* CannedEvents_CriteriaParmList_get() { if( !tblCannedEvents_CriteriaParmList ) CreateTable_CannedEvents_CriteriaParmList(); return tblCannedEvents_CriteriaParmList; }
class Table_Command* Command_get() { if( !tblCommand ) CreateTable_Command(); return tblCommand; }
class Table_CommandCategory* CommandCategory_get() { if( !tblCommandCategory ) CreateTable_CommandCategory(); return tblCommandCategory; }
class Table_CommandGroup* CommandGroup_get() { if( !tblCommandGroup ) CreateTable_CommandGroup(); return tblCommandGroup; }
class Table_CommandGroup_Command* CommandGroup_Command_get() { if( !tblCommandGroup_Command ) CreateTable_CommandGroup_Command(); return tblCommandGroup_Command; }
class Table_CommandGroup_Command_CommandParameter* CommandGroup_Command_CommandParameter_get() { if( !tblCommandGroup_Command_CommandParameter ) CreateTable_CommandGroup_Command_CommandParameter(); return tblCommandGroup_Command_CommandParameter; }
class Table_CommandGroup_D* CommandGroup_D_get() { if( !tblCommandGroup_D ) CreateTable_CommandGroup_D(); return tblCommandGroup_D; }
class Table_CommandGroup_D_Command* CommandGroup_D_Command_get() { if( !tblCommandGroup_D_Command ) CreateTable_CommandGroup_D_Command(); return tblCommandGroup_D_Command; }
class Table_CommandGroup_D_Command_CommandParameter* CommandGroup_D_Command_CommandParameter_get() { if( !tblCommandGroup_D_Command_CommandParameter ) CreateTable_CommandGroup_D_Command_CommandParameter(); return tblCommandGroup_D_Command_CommandParameter; }
class Table_CommandGroup_EntertainArea* CommandGroup_EntertainArea_get() { if( !tblCommandGroup_EntertainArea ) CreateTable_CommandGroup_EntertainArea(); return tblCommandGroup_EntertainArea; }
class Table_CommandGroup_Room* CommandGroup_Room_get() { if( !tblCommandGroup_Room ) CreateTable_CommandGroup_Room(); return tblCommandGroup_Room; }
class Table_CommandParameter* CommandParameter_get() { if( !tblCommandParameter ) CreateTable_CommandParameter(); return tblCommandParameter; }
class Table_Command_CommandParameter* Command_CommandParameter_get() { if( !tblCommand_CommandParameter ) CreateTable_Command_CommandParameter(); return tblCommand_CommandParameter; }
class Table_Command_Pipe* Command_Pipe_get() { if( !tblCommand_Pipe ) CreateTable_Command_Pipe(); return tblCommand_Pipe; }
class Table_ConfigType* ConfigType_get() { if( !tblConfigType ) CreateTable_ConfigType(); return tblConfigType; }
class Table_ConfigType_File* ConfigType_File_get() { if( !tblConfigType_File ) CreateTable_ConfigType_File(); return tblConfigType_File; }
class Table_ConfigType_Setting* ConfigType_Setting_get() { if( !tblConfigType_Setting ) CreateTable_ConfigType_Setting(); return tblConfigType_Setting; }
class Table_ConfigType_Token* ConfigType_Token_get() { if( !tblConfigType_Token ) CreateTable_ConfigType_Token(); return tblConfigType_Token; }
class Table_ConnectorType* ConnectorType_get() { if( !tblConnectorType ) CreateTable_ConnectorType(); return tblConnectorType; }
class Table_Country* Country_get() { if( !tblCountry ) CreateTable_Country(); return tblCountry; }
class Table_Criteria* Criteria_get() { if( !tblCriteria ) CreateTable_Criteria(); return tblCriteria; }
class Table_CriteriaList* CriteriaList_get() { if( !tblCriteriaList ) CreateTable_CriteriaList(); return tblCriteriaList; }
class Table_CriteriaList_CriteriaParmList* CriteriaList_CriteriaParmList_get() { if( !tblCriteriaList_CriteriaParmList ) CreateTable_CriteriaList_CriteriaParmList(); return tblCriteriaList_CriteriaParmList; }
class Table_CriteriaParm* CriteriaParm_get() { if( !tblCriteriaParm ) CreateTable_CriteriaParm(); return tblCriteriaParm; }
class Table_CriteriaParmList* CriteriaParmList_get() { if( !tblCriteriaParmList ) CreateTable_CriteriaParmList(); return tblCriteriaParmList; }
class Table_CriteriaParmNesting* CriteriaParmNesting_get() { if( !tblCriteriaParmNesting ) CreateTable_CriteriaParmNesting(); return tblCriteriaParmNesting; }
class Table_CriteriaParmNesting_D* CriteriaParmNesting_D_get() { if( !tblCriteriaParmNesting_D ) CreateTable_CriteriaParmNesting_D(); return tblCriteriaParmNesting_D; }
class Table_CriteriaParm_D* CriteriaParm_D_get() { if( !tblCriteriaParm_D ) CreateTable_CriteriaParm_D(); return tblCriteriaParm_D; }
class Table_Criteria_D* Criteria_D_get() { if( !tblCriteria_D ) CreateTable_Criteria_D(); return tblCriteria_D; }
class Table_DHCPDevice* DHCPDevice_get() { if( !tblDHCPDevice ) CreateTable_DHCPDevice(); return tblDHCPDevice; }
class Table_DHCPDevice_DeviceData* DHCPDevice_DeviceData_get() { if( !tblDHCPDevice_DeviceData ) CreateTable_DHCPDevice_DeviceData(); return tblDHCPDevice_DeviceData; }
class Table_DataGrid* DataGrid_get() { if( !tblDataGrid ) CreateTable_DataGrid(); return tblDataGrid; }
class Table_DesignObj* DesignObj_get() { if( !tblDesignObj ) CreateTable_DesignObj(); return tblDesignObj; }
class Table_DesignObjCategory* DesignObjCategory_get() { if( !tblDesignObjCategory ) CreateTable_DesignObjCategory(); return tblDesignObjCategory; }
class Table_DesignObjParameter* DesignObjParameter_get() { if( !tblDesignObjParameter ) CreateTable_DesignObjParameter(); return tblDesignObjParameter; }
class Table_DesignObjType* DesignObjType_get() { if( !tblDesignObjType ) CreateTable_DesignObjType(); return tblDesignObjType; }
class Table_DesignObjType_DesignObjParameter* DesignObjType_DesignObjParameter_get() { if( !tblDesignObjType_DesignObjParameter ) CreateTable_DesignObjType_DesignObjParameter(); return tblDesignObjType_DesignObjParameter; }
class Table_DesignObjVariation* DesignObjVariation_get() { if( !tblDesignObjVariation ) CreateTable_DesignObjVariation(); return tblDesignObjVariation; }
class Table_DesignObjVariation_DesignObj* DesignObjVariation_DesignObj_get() { if( !tblDesignObjVariation_DesignObj ) CreateTable_DesignObjVariation_DesignObj(); return tblDesignObjVariation_DesignObj; }
class Table_DesignObjVariation_DesignObjParameter* DesignObjVariation_DesignObjParameter_get() { if( !tblDesignObjVariation_DesignObjParameter ) CreateTable_DesignObjVariation_DesignObjParameter(); return tblDesignObjVariation_DesignObjParameter; }
class Table_DesignObjVariation_Text* DesignObjVariation_Text_get() { if( !tblDesignObjVariation_Text ) CreateTable_DesignObjVariation_Text(); return tblDesignObjVariation_Text; }
class Table_DesignObjVariation_Text_Skin_Language* DesignObjVariation_Text_Skin_Language_get() { if( !tblDesignObjVariation_Text_Skin_Language ) CreateTable_DesignObjVariation_Text_Skin_Language(); return tblDesignObjVariation_Text_Skin_Language; }
class Table_DesignObjVariation_Zone* DesignObjVariation_Zone_get() { if( !tblDesignObjVariation_Zone ) CreateTable_DesignObjVariation_Zone(); return tblDesignObjVariation_Zone; }
class Table_Device* Device_get() { if( !tblDevice ) CreateTable_Device(); return tblDevice; }
class Table_DeviceCategory* DeviceCategory_get() { if( !tblDeviceCategory ) CreateTable_DeviceCategory(); return tblDeviceCategory; }
class Table_DeviceCategory_DeviceData* DeviceCategory_DeviceData_get() { if( !tblDeviceCategory_DeviceData ) CreateTable_DeviceCategory_DeviceData(); return tblDeviceCategory_DeviceData; }
class Table_DeviceCategory_Event* DeviceCategory_Event_get() { if( !tblDeviceCategory_Event ) CreateTable_DeviceCategory_Event(); return tblDeviceCategory_Event; }
class Table_DeviceCommandGroup* DeviceCommandGroup_get() { if( !tblDeviceCommandGroup ) CreateTable_DeviceCommandGroup(); return tblDeviceCommandGroup; }
class Table_DeviceCommandGroup_Command* DeviceCommandGroup_Command_get() { if( !tblDeviceCommandGroup_Command ) CreateTable_DeviceCommandGroup_Command(); return tblDeviceCommandGroup_Command; }
class Table_DeviceCommandGroup_DeviceCommandGroup_Parent* DeviceCommandGroup_DeviceCommandGroup_Parent_get() { if( !tblDeviceCommandGroup_DeviceCommandGroup_Parent ) CreateTable_DeviceCommandGroup_DeviceCommandGroup_Parent(); return tblDeviceCommandGroup_DeviceCommandGroup_Parent; }
class Table_DeviceData* DeviceData_get() { if( !tblDeviceData ) CreateTable_DeviceData(); return tblDeviceData; }
class Table_DeviceGroup* DeviceGroup_get() { if( !tblDeviceGroup ) CreateTable_DeviceGroup(); return tblDeviceGroup; }
class Table_DeviceTemplate* DeviceTemplate_get() { if( !tblDeviceTemplate ) CreateTable_DeviceTemplate(); return tblDeviceTemplate; }
class Table_DeviceTemplate_AV* DeviceTemplate_AV_get() { if( !tblDeviceTemplate_AV ) CreateTable_DeviceTemplate_AV(); return tblDeviceTemplate_AV; }
class Table_DeviceTemplate_DSPMode* DeviceTemplate_DSPMode_get() { if( !tblDeviceTemplate_DSPMode ) CreateTable_DeviceTemplate_DSPMode(); return tblDeviceTemplate_DSPMode; }
class Table_DeviceTemplate_DesignObj* DeviceTemplate_DesignObj_get() { if( !tblDeviceTemplate_DesignObj ) CreateTable_DeviceTemplate_DesignObj(); return tblDeviceTemplate_DesignObj; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia* DeviceTemplate_DeviceCategory_ControlledVia_get() { if( !tblDeviceTemplate_DeviceCategory_ControlledVia ) CreateTable_DeviceTemplate_DeviceCategory_ControlledVia(); return tblDeviceTemplate_DeviceCategory_ControlledVia; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get() { if( !tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe ) CreateTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(); return tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceCommandGroup* DeviceTemplate_DeviceCommandGroup_get() { if( !tblDeviceTemplate_DeviceCommandGroup ) CreateTable_DeviceTemplate_DeviceCommandGroup(); return tblDeviceTemplate_DeviceCommandGroup; }
class Table_DeviceTemplate_DeviceData* DeviceTemplate_DeviceData_get() { if( !tblDeviceTemplate_DeviceData ) CreateTable_DeviceTemplate_DeviceData(); return tblDeviceTemplate_DeviceData; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia* DeviceTemplate_DeviceTemplate_ControlledVia_get() { if( !tblDeviceTemplate_DeviceTemplate_ControlledVia ) CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia(); return tblDeviceTemplate_DeviceTemplate_ControlledVia; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe* DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_get() { if( !tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe ) CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(); return tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceTemplate_Related* DeviceTemplate_DeviceTemplate_Related_get() { if( !tblDeviceTemplate_DeviceTemplate_Related ) CreateTable_DeviceTemplate_DeviceTemplate_Related(); return tblDeviceTemplate_DeviceTemplate_Related; }
class Table_DeviceTemplate_Event* DeviceTemplate_Event_get() { if( !tblDeviceTemplate_Event ) CreateTable_DeviceTemplate_Event(); return tblDeviceTemplate_Event; }
class Table_DeviceTemplate_InfraredGroup* DeviceTemplate_InfraredGroup_get() { if( !tblDeviceTemplate_InfraredGroup ) CreateTable_DeviceTemplate_InfraredGroup(); return tblDeviceTemplate_InfraredGroup; }
class Table_DeviceTemplate_Input* DeviceTemplate_Input_get() { if( !tblDeviceTemplate_Input ) CreateTable_DeviceTemplate_Input(); return tblDeviceTemplate_Input; }
class Table_DeviceTemplate_MediaType* DeviceTemplate_MediaType_get() { if( !tblDeviceTemplate_MediaType ) CreateTable_DeviceTemplate_MediaType(); return tblDeviceTemplate_MediaType; }
class Table_DeviceTemplate_MediaType_DesignObj* DeviceTemplate_MediaType_DesignObj_get() { if( !tblDeviceTemplate_MediaType_DesignObj ) CreateTable_DeviceTemplate_MediaType_DesignObj(); return tblDeviceTemplate_MediaType_DesignObj; }
class Table_DeviceTemplate_Output* DeviceTemplate_Output_get() { if( !tblDeviceTemplate_Output ) CreateTable_DeviceTemplate_Output(); return tblDeviceTemplate_Output; }
class Table_DeviceTemplate_PageSetup* DeviceTemplate_PageSetup_get() { if( !tblDeviceTemplate_PageSetup ) CreateTable_DeviceTemplate_PageSetup(); return tblDeviceTemplate_PageSetup; }
class Table_Device_Command* Device_Command_get() { if( !tblDevice_Command ) CreateTable_Device_Command(); return tblDevice_Command; }
class Table_Device_CommandGroup* Device_CommandGroup_get() { if( !tblDevice_CommandGroup ) CreateTable_Device_CommandGroup(); return tblDevice_CommandGroup; }
class Table_Device_DeviceData* Device_DeviceData_get() { if( !tblDevice_DeviceData ) CreateTable_Device_DeviceData(); return tblDevice_DeviceData; }
class Table_Device_DeviceGroup* Device_DeviceGroup_get() { if( !tblDevice_DeviceGroup ) CreateTable_Device_DeviceGroup(); return tblDevice_DeviceGroup; }
class Table_Device_Device_Pipe* Device_Device_Pipe_get() { if( !tblDevice_Device_Pipe ) CreateTable_Device_Device_Pipe(); return tblDevice_Device_Pipe; }
class Table_Device_Device_Related* Device_Device_Related_get() { if( !tblDevice_Device_Related ) CreateTable_Device_Device_Related(); return tblDevice_Device_Related; }
class Table_Device_EntertainArea* Device_EntertainArea_get() { if( !tblDevice_EntertainArea ) CreateTable_Device_EntertainArea(); return tblDevice_EntertainArea; }
class Table_Device_HouseMode* Device_HouseMode_get() { if( !tblDevice_HouseMode ) CreateTable_Device_HouseMode(); return tblDevice_HouseMode; }
class Table_Device_MRU* Device_MRU_get() { if( !tblDevice_MRU ) CreateTable_Device_MRU(); return tblDevice_MRU; }
class Table_Device_Orbiter* Device_Orbiter_get() { if( !tblDevice_Orbiter ) CreateTable_Device_Orbiter(); return tblDevice_Orbiter; }
class Table_Device_QuickStart* Device_QuickStart_get() { if( !tblDevice_QuickStart ) CreateTable_Device_QuickStart(); return tblDevice_QuickStart; }
class Table_Device_StartupScript* Device_StartupScript_get() { if( !tblDevice_StartupScript ) CreateTable_Device_StartupScript(); return tblDevice_StartupScript; }
class Table_Device_Users* Device_Users_get() { if( !tblDevice_Users ) CreateTable_Device_Users(); return tblDevice_Users; }
class Table_Direction* Direction_get() { if( !tblDirection ) CreateTable_Direction(); return tblDirection; }
class Table_Directory* Directory_get() { if( !tblDirectory ) CreateTable_Directory(); return tblDirectory; }
class Table_Distro* Distro_get() { if( !tblDistro ) CreateTable_Distro(); return tblDistro; }
class Table_Document* Document_get() { if( !tblDocument ) CreateTable_Document(); return tblDocument; }
class Table_Document_Comment* Document_Comment_get() { if( !tblDocument_Comment ) CreateTable_Document_Comment(); return tblDocument_Comment; }
class Table_EntertainArea* EntertainArea_get() { if( !tblEntertainArea ) CreateTable_EntertainArea(); return tblEntertainArea; }
class Table_Event* Event_get() { if( !tblEvent ) CreateTable_Event(); return tblEvent; }
class Table_EventCategory* EventCategory_get() { if( !tblEventCategory ) CreateTable_EventCategory(); return tblEventCategory; }
class Table_EventHandler* EventHandler_get() { if( !tblEventHandler ) CreateTable_EventHandler(); return tblEventHandler; }
class Table_EventParameter* EventParameter_get() { if( !tblEventParameter ) CreateTable_EventParameter(); return tblEventParameter; }
class Table_Event_EventParameter* Event_EventParameter_get() { if( !tblEvent_EventParameter ) CreateTable_Event_EventParameter(); return tblEvent_EventParameter; }
class Table_FAQ* FAQ_get() { if( !tblFAQ ) CreateTable_FAQ(); return tblFAQ; }
class Table_Firewall* Firewall_get() { if( !tblFirewall ) CreateTable_Firewall(); return tblFirewall; }
class Table_Floorplan* Floorplan_get() { if( !tblFloorplan ) CreateTable_Floorplan(); return tblFloorplan; }
class Table_FloorplanObjectType* FloorplanObjectType_get() { if( !tblFloorplanObjectType ) CreateTable_FloorplanObjectType(); return tblFloorplanObjectType; }
class Table_FloorplanObjectType_Color* FloorplanObjectType_Color_get() { if( !tblFloorplanObjectType_Color ) CreateTable_FloorplanObjectType_Color(); return tblFloorplanObjectType_Color; }
class Table_FloorplanType* FloorplanType_get() { if( !tblFloorplanType ) CreateTable_FloorplanType(); return tblFloorplanType; }
class Table_HorizAlignment* HorizAlignment_get() { if( !tblHorizAlignment ) CreateTable_HorizAlignment(); return tblHorizAlignment; }
class Table_HouseMode* HouseMode_get() { if( !tblHouseMode ) CreateTable_HouseMode(); return tblHouseMode; }
class Table_Household* Household_get() { if( !tblHousehold ) CreateTable_Household(); return tblHousehold; }
class Table_Household_Installation* Household_Installation_get() { if( !tblHousehold_Installation ) CreateTable_Household_Installation(); return tblHousehold_Installation; }
class Table_Icon* Icon_get() { if( !tblIcon ) CreateTable_Icon(); return tblIcon; }
class Table_Image* Image_get() { if( !tblImage ) CreateTable_Image(); return tblImage; }
class Table_InfraredGroup* InfraredGroup_get() { if( !tblInfraredGroup ) CreateTable_InfraredGroup(); return tblInfraredGroup; }
class Table_InfraredGroup_Command* InfraredGroup_Command_get() { if( !tblInfraredGroup_Command ) CreateTable_InfraredGroup_Command(); return tblInfraredGroup_Command; }
class Table_InfraredGroup_Command_Preferred* InfraredGroup_Command_Preferred_get() { if( !tblInfraredGroup_Command_Preferred ) CreateTable_InfraredGroup_Command_Preferred(); return tblInfraredGroup_Command_Preferred; }
class Table_InstallWizard* InstallWizard_get() { if( !tblInstallWizard ) CreateTable_InstallWizard(); return tblInstallWizard; }
class Table_InstallWizard_Distro* InstallWizard_Distro_get() { if( !tblInstallWizard_Distro ) CreateTable_InstallWizard_Distro(); return tblInstallWizard_Distro; }
class Table_Installation* Installation_get() { if( !tblInstallation ) CreateTable_Installation(); return tblInstallation; }
class Table_Installation_RepositorySource_URL* Installation_RepositorySource_URL_get() { if( !tblInstallation_RepositorySource_URL ) CreateTable_Installation_RepositorySource_URL(); return tblInstallation_RepositorySource_URL; }
class Table_Installation_Users* Installation_Users_get() { if( !tblInstallation_Users ) CreateTable_Installation_Users(); return tblInstallation_Users; }
class Table_Language* Language_get() { if( !tblLanguage ) CreateTable_Language(); return tblLanguage; }
class Table_License* License_get() { if( !tblLicense ) CreateTable_License(); return tblLicense; }
class Table_Licensing* Licensing_get() { if( !tblLicensing ) CreateTable_Licensing(); return tblLicensing; }
class Table_Manufacturer* Manufacturer_get() { if( !tblManufacturer ) CreateTable_Manufacturer(); return tblManufacturer; }
class Table_MediaType* MediaType_get() { if( !tblMediaType ) CreateTable_MediaType(); return tblMediaType; }
class Table_MediaType_Broadcast* MediaType_Broadcast_get() { if( !tblMediaType_Broadcast ) CreateTable_MediaType_Broadcast(); return tblMediaType_Broadcast; }
class Table_MediaType_DesignObj* MediaType_DesignObj_get() { if( !tblMediaType_DesignObj ) CreateTable_MediaType_DesignObj(); return tblMediaType_DesignObj; }
class Table_News* News_get() { if( !tblNews ) CreateTable_News(); return tblNews; }
class Table_OperatingSystem* OperatingSystem_get() { if( !tblOperatingSystem ) CreateTable_OperatingSystem(); return tblOperatingSystem; }
class Table_Orbiter* Orbiter_get() { if( !tblOrbiter ) CreateTable_Orbiter(); return tblOrbiter; }
class Table_Orbiter_Users_PasswordReq* Orbiter_Users_PasswordReq_get() { if( !tblOrbiter_Users_PasswordReq ) CreateTable_Orbiter_Users_PasswordReq(); return tblOrbiter_Users_PasswordReq; }
class Table_Orbiter_Variable* Orbiter_Variable_get() { if( !tblOrbiter_Variable ) CreateTable_Orbiter_Variable(); return tblOrbiter_Variable; }
class Table_Package* Package_get() { if( !tblPackage ) CreateTable_Package(); return tblPackage; }
class Table_PackageType* PackageType_get() { if( !tblPackageType ) CreateTable_PackageType(); return tblPackageType; }
class Table_Package_Compat* Package_Compat_get() { if( !tblPackage_Compat ) CreateTable_Package_Compat(); return tblPackage_Compat; }
class Table_Package_Device* Package_Device_get() { if( !tblPackage_Device ) CreateTable_Package_Device(); return tblPackage_Device; }
class Table_Package_Directory* Package_Directory_get() { if( !tblPackage_Directory ) CreateTable_Package_Directory(); return tblPackage_Directory; }
class Table_Package_Directory_File* Package_Directory_File_get() { if( !tblPackage_Directory_File ) CreateTable_Package_Directory_File(); return tblPackage_Directory_File; }
class Table_Package_Package* Package_Package_get() { if( !tblPackage_Package ) CreateTable_Package_Package(); return tblPackage_Package; }
class Table_Package_Source* Package_Source_get() { if( !tblPackage_Source ) CreateTable_Package_Source(); return tblPackage_Source; }
class Table_Package_Source_Compat* Package_Source_Compat_get() { if( !tblPackage_Source_Compat ) CreateTable_Package_Source_Compat(); return tblPackage_Source_Compat; }
class Table_Package_Users* Package_Users_get() { if( !tblPackage_Users ) CreateTable_Package_Users(); return tblPackage_Users; }
class Table_Package_Version* Package_Version_get() { if( !tblPackage_Version ) CreateTable_Package_Version(); return tblPackage_Version; }
class Table_PageSetup* PageSetup_get() { if( !tblPageSetup ) CreateTable_PageSetup(); return tblPageSetup; }
class Table_PaidLicense* PaidLicense_get() { if( !tblPaidLicense ) CreateTable_PaidLicense(); return tblPaidLicense; }
class Table_ParameterType* ParameterType_get() { if( !tblParameterType ) CreateTable_ParameterType(); return tblParameterType; }
class Table_PhoneLineType* PhoneLineType_get() { if( !tblPhoneLineType ) CreateTable_PhoneLineType(); return tblPhoneLineType; }
class Table_Pipe* Pipe_get() { if( !tblPipe ) CreateTable_Pipe(); return tblPipe; }
class Table_RemoteControl* RemoteControl_get() { if( !tblRemoteControl ) CreateTable_RemoteControl(); return tblRemoteControl; }
class Table_RepositorySource* RepositorySource_get() { if( !tblRepositorySource ) CreateTable_RepositorySource(); return tblRepositorySource; }
class Table_RepositorySource_URL* RepositorySource_URL_get() { if( !tblRepositorySource_URL ) CreateTable_RepositorySource_URL(); return tblRepositorySource_URL; }
class Table_RepositoryType* RepositoryType_get() { if( !tblRepositoryType ) CreateTable_RepositoryType(); return tblRepositoryType; }
class Table_Room* Room_get() { if( !tblRoom ) CreateTable_Room(); return tblRoom; }
class Table_RoomType* RoomType_get() { if( !tblRoomType ) CreateTable_RoomType(); return tblRoomType; }
class Table_Room_Users* Room_Users_get() { if( !tblRoom_Users ) CreateTable_Room_Users(); return tblRoom_Users; }
class Table_Schema* Schema_get() { if( !tblSchema ) CreateTable_Schema(); return tblSchema; }
class Table_SetupStep* SetupStep_get() { if( !tblSetupStep ) CreateTable_SetupStep(); return tblSetupStep; }
class Table_Size* Size_get() { if( !tblSize ) CreateTable_Size(); return tblSize; }
class Table_Skin* Skin_get() { if( !tblSkin ) CreateTable_Skin(); return tblSkin; }
class Table_StabilityStatus* StabilityStatus_get() { if( !tblStabilityStatus ) CreateTable_StabilityStatus(); return tblStabilityStatus; }
class Table_StartupScript* StartupScript_get() { if( !tblStartupScript ) CreateTable_StartupScript(); return tblStartupScript; }
class Table_Style* Style_get() { if( !tblStyle ) CreateTable_Style(); return tblStyle; }
class Table_StyleVariation* StyleVariation_get() { if( !tblStyleVariation ) CreateTable_StyleVariation(); return tblStyleVariation; }
class Table_System* System_get() { if( !tblSystem ) CreateTable_System(); return tblSystem; }
class Table_Template* Template_get() { if( !tblTemplate ) CreateTable_Template(); return tblTemplate; }
class Table_Text* Text_get() { if( !tblText ) CreateTable_Text(); return tblText; }
class Table_TextCategory* TextCategory_get() { if( !tblTextCategory ) CreateTable_TextCategory(); return tblTextCategory; }
class Table_Text_LS* Text_LS_get() { if( !tblText_LS ) CreateTable_Text_LS(); return tblText_LS; }
class Table_Text_LS_AltVersions* Text_LS_AltVersions_get() { if( !tblText_LS_AltVersions ) CreateTable_Text_LS_AltVersions(); return tblText_LS_AltVersions; }
class Table_UnknownDevices* UnknownDevices_get() { if( !tblUnknownDevices ) CreateTable_UnknownDevices(); return tblUnknownDevices; }
class Table_UserMode* UserMode_get() { if( !tblUserMode ) CreateTable_UserMode(); return tblUserMode; }
class Table_Users* Users_get() { if( !tblUsers ) CreateTable_Users(); return tblUsers; }
class Table_Variable* Variable_get() { if( !tblVariable ) CreateTable_Variable(); return tblVariable; }
class Table_Version* Version_get() { if( !tblVersion ) CreateTable_Version(); return tblVersion; }
class Table_VertAlignment* VertAlignment_get() { if( !tblVertAlignment ) CreateTable_VertAlignment(); return tblVertAlignment; }
class Table_groupaddresses* groupaddresses_get() { if( !tblgroupaddresses ) CreateTable_groupaddresses(); return tblgroupaddresses; }
class Table_psc_constants_batdet* psc_constants_batdet_get() { if( !tblpsc_constants_batdet ) CreateTable_psc_constants_batdet(); return tblpsc_constants_batdet; }
class Table_psc_constants_bathdr* psc_constants_bathdr_get() { if( !tblpsc_constants_bathdr ) CreateTable_psc_constants_bathdr(); return tblpsc_constants_bathdr; }
class Table_psc_constants_batuser* psc_constants_batuser_get() { if( !tblpsc_constants_batuser ) CreateTable_psc_constants_batuser(); return tblpsc_constants_batuser; }
class Table_psc_constants_repset* psc_constants_repset_get() { if( !tblpsc_constants_repset ) CreateTable_psc_constants_repset(); return tblpsc_constants_repset; }
class Table_psc_constants_schema* psc_constants_schema_get() { if( !tblpsc_constants_schema ) CreateTable_psc_constants_schema(); return tblpsc_constants_schema; }
class Table_psc_constants_tables* psc_constants_tables_get() { if( !tblpsc_constants_tables ) CreateTable_psc_constants_tables(); return tblpsc_constants_tables; }
class Table_psc_dce_batdet* psc_dce_batdet_get() { if( !tblpsc_dce_batdet ) CreateTable_psc_dce_batdet(); return tblpsc_dce_batdet; }
class Table_psc_dce_bathdr* psc_dce_bathdr_get() { if( !tblpsc_dce_bathdr ) CreateTable_psc_dce_bathdr(); return tblpsc_dce_bathdr; }
class Table_psc_dce_batuser* psc_dce_batuser_get() { if( !tblpsc_dce_batuser ) CreateTable_psc_dce_batuser(); return tblpsc_dce_batuser; }
class Table_psc_dce_repset* psc_dce_repset_get() { if( !tblpsc_dce_repset ) CreateTable_psc_dce_repset(); return tblpsc_dce_repset; }
class Table_psc_dce_schema* psc_dce_schema_get() { if( !tblpsc_dce_schema ) CreateTable_psc_dce_schema(); return tblpsc_dce_schema; }
class Table_psc_dce_tables* psc_dce_tables_get() { if( !tblpsc_dce_tables ) CreateTable_psc_dce_tables(); return tblpsc_dce_tables; }
class Table_psc_designer_batdet* psc_designer_batdet_get() { if( !tblpsc_designer_batdet ) CreateTable_psc_designer_batdet(); return tblpsc_designer_batdet; }
class Table_psc_designer_bathdr* psc_designer_bathdr_get() { if( !tblpsc_designer_bathdr ) CreateTable_psc_designer_bathdr(); return tblpsc_designer_bathdr; }
class Table_psc_designer_batuser* psc_designer_batuser_get() { if( !tblpsc_designer_batuser ) CreateTable_psc_designer_batuser(); return tblpsc_designer_batuser; }
class Table_psc_designer_repset* psc_designer_repset_get() { if( !tblpsc_designer_repset ) CreateTable_psc_designer_repset(); return tblpsc_designer_repset; }
class Table_psc_designer_schema* psc_designer_schema_get() { if( !tblpsc_designer_schema ) CreateTable_psc_designer_schema(); return tblpsc_designer_schema; }
class Table_psc_designer_tables* psc_designer_tables_get() { if( !tblpsc_designer_tables ) CreateTable_psc_designer_tables(); return tblpsc_designer_tables; }
class Table_psc_document_batdet* psc_document_batdet_get() { if( !tblpsc_document_batdet ) CreateTable_psc_document_batdet(); return tblpsc_document_batdet; }
class Table_psc_document_bathdr* psc_document_bathdr_get() { if( !tblpsc_document_bathdr ) CreateTable_psc_document_bathdr(); return tblpsc_document_bathdr; }
class Table_psc_document_batuser* psc_document_batuser_get() { if( !tblpsc_document_batuser ) CreateTable_psc_document_batuser(); return tblpsc_document_batuser; }
class Table_psc_document_repset* psc_document_repset_get() { if( !tblpsc_document_repset ) CreateTable_psc_document_repset(); return tblpsc_document_repset; }
class Table_psc_document_schema* psc_document_schema_get() { if( !tblpsc_document_schema ) CreateTable_psc_document_schema(); return tblpsc_document_schema; }
class Table_psc_document_tables* psc_document_tables_get() { if( !tblpsc_document_tables ) CreateTable_psc_document_tables(); return tblpsc_document_tables; }
class Table_psc_ir_batdet* psc_ir_batdet_get() { if( !tblpsc_ir_batdet ) CreateTable_psc_ir_batdet(); return tblpsc_ir_batdet; }
class Table_psc_ir_bathdr* psc_ir_bathdr_get() { if( !tblpsc_ir_bathdr ) CreateTable_psc_ir_bathdr(); return tblpsc_ir_bathdr; }
class Table_psc_ir_batuser* psc_ir_batuser_get() { if( !tblpsc_ir_batuser ) CreateTable_psc_ir_batuser(); return tblpsc_ir_batuser; }
class Table_psc_ir_repset* psc_ir_repset_get() { if( !tblpsc_ir_repset ) CreateTable_psc_ir_repset(); return tblpsc_ir_repset; }
class Table_psc_ir_schema* psc_ir_schema_get() { if( !tblpsc_ir_schema ) CreateTable_psc_ir_schema(); return tblpsc_ir_schema; }
class Table_psc_ir_tables* psc_ir_tables_get() { if( !tblpsc_ir_tables ) CreateTable_psc_ir_tables(); return tblpsc_ir_tables; }
class Table_psc_local_batdet* psc_local_batdet_get() { if( !tblpsc_local_batdet ) CreateTable_psc_local_batdet(); return tblpsc_local_batdet; }
class Table_psc_local_bathdr* psc_local_bathdr_get() { if( !tblpsc_local_bathdr ) CreateTable_psc_local_bathdr(); return tblpsc_local_bathdr; }
class Table_psc_local_batuser* psc_local_batuser_get() { if( !tblpsc_local_batuser ) CreateTable_psc_local_batuser(); return tblpsc_local_batuser; }
class Table_psc_local_repset* psc_local_repset_get() { if( !tblpsc_local_repset ) CreateTable_psc_local_repset(); return tblpsc_local_repset; }
class Table_psc_local_schema* psc_local_schema_get() { if( !tblpsc_local_schema ) CreateTable_psc_local_schema(); return tblpsc_local_schema; }
class Table_psc_local_tables* psc_local_tables_get() { if( !tblpsc_local_tables ) CreateTable_psc_local_tables(); return tblpsc_local_tables; }
class Table_psc_website_batdet* psc_website_batdet_get() { if( !tblpsc_website_batdet ) CreateTable_psc_website_batdet(); return tblpsc_website_batdet; }
class Table_psc_website_bathdr* psc_website_bathdr_get() { if( !tblpsc_website_bathdr ) CreateTable_psc_website_bathdr(); return tblpsc_website_bathdr; }
class Table_psc_website_batuser* psc_website_batuser_get() { if( !tblpsc_website_batuser ) CreateTable_psc_website_batuser(); return tblpsc_website_batuser; }
class Table_psc_website_repset* psc_website_repset_get() { if( !tblpsc_website_repset ) CreateTable_psc_website_repset(); return tblpsc_website_repset; }
class Table_psc_website_schema* psc_website_schema_get() { if( !tblpsc_website_schema ) CreateTable_psc_website_schema(); return tblpsc_website_schema; }
class Table_psc_website_tables* psc_website_tables_get() { if( !tblpsc_website_tables ) CreateTable_psc_website_tables(); return tblpsc_website_tables; }
string m_sLastMySqlError;
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Array();
void CreateTable_Broadcast();
void CreateTable_Button();
void CreateTable_CachedScreens();
void CreateTable_CannedEvents();
void CreateTable_CannedEvents_CriteriaParmList();
void CreateTable_Command();
void CreateTable_CommandCategory();
void CreateTable_CommandGroup();
void CreateTable_CommandGroup_Command();
void CreateTable_CommandGroup_Command_CommandParameter();
void CreateTable_CommandGroup_D();
void CreateTable_CommandGroup_D_Command();
void CreateTable_CommandGroup_D_Command_CommandParameter();
void CreateTable_CommandGroup_EntertainArea();
void CreateTable_CommandGroup_Room();
void CreateTable_CommandParameter();
void CreateTable_Command_CommandParameter();
void CreateTable_Command_Pipe();
void CreateTable_ConfigType();
void CreateTable_ConfigType_File();
void CreateTable_ConfigType_Setting();
void CreateTable_ConfigType_Token();
void CreateTable_ConnectorType();
void CreateTable_Country();
void CreateTable_Criteria();
void CreateTable_CriteriaList();
void CreateTable_CriteriaList_CriteriaParmList();
void CreateTable_CriteriaParm();
void CreateTable_CriteriaParmList();
void CreateTable_CriteriaParmNesting();
void CreateTable_CriteriaParmNesting_D();
void CreateTable_CriteriaParm_D();
void CreateTable_Criteria_D();
void CreateTable_DHCPDevice();
void CreateTable_DHCPDevice_DeviceData();
void CreateTable_DataGrid();
void CreateTable_DesignObj();
void CreateTable_DesignObjCategory();
void CreateTable_DesignObjParameter();
void CreateTable_DesignObjType();
void CreateTable_DesignObjType_DesignObjParameter();
void CreateTable_DesignObjVariation();
void CreateTable_DesignObjVariation_DesignObj();
void CreateTable_DesignObjVariation_DesignObjParameter();
void CreateTable_DesignObjVariation_Text();
void CreateTable_DesignObjVariation_Text_Skin_Language();
void CreateTable_DesignObjVariation_Zone();
void CreateTable_Device();
void CreateTable_DeviceCategory();
void CreateTable_DeviceCategory_DeviceData();
void CreateTable_DeviceCategory_Event();
void CreateTable_DeviceCommandGroup();
void CreateTable_DeviceCommandGroup_Command();
void CreateTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
void CreateTable_DeviceData();
void CreateTable_DeviceGroup();
void CreateTable_DeviceTemplate();
void CreateTable_DeviceTemplate_AV();
void CreateTable_DeviceTemplate_DSPMode();
void CreateTable_DeviceTemplate_DesignObj();
void CreateTable_DeviceTemplate_DeviceCategory_ControlledVia();
void CreateTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
void CreateTable_DeviceTemplate_DeviceCommandGroup();
void CreateTable_DeviceTemplate_DeviceData();
void CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia();
void CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
void CreateTable_DeviceTemplate_DeviceTemplate_Related();
void CreateTable_DeviceTemplate_Event();
void CreateTable_DeviceTemplate_InfraredGroup();
void CreateTable_DeviceTemplate_Input();
void CreateTable_DeviceTemplate_MediaType();
void CreateTable_DeviceTemplate_MediaType_DesignObj();
void CreateTable_DeviceTemplate_Output();
void CreateTable_DeviceTemplate_PageSetup();
void CreateTable_Device_Command();
void CreateTable_Device_CommandGroup();
void CreateTable_Device_DeviceData();
void CreateTable_Device_DeviceGroup();
void CreateTable_Device_Device_Pipe();
void CreateTable_Device_Device_Related();
void CreateTable_Device_EntertainArea();
void CreateTable_Device_HouseMode();
void CreateTable_Device_MRU();
void CreateTable_Device_Orbiter();
void CreateTable_Device_QuickStart();
void CreateTable_Device_StartupScript();
void CreateTable_Device_Users();
void CreateTable_Direction();
void CreateTable_Directory();
void CreateTable_Distro();
void CreateTable_Document();
void CreateTable_Document_Comment();
void CreateTable_EntertainArea();
void CreateTable_Event();
void CreateTable_EventCategory();
void CreateTable_EventHandler();
void CreateTable_EventParameter();
void CreateTable_Event_EventParameter();
void CreateTable_FAQ();
void CreateTable_Firewall();
void CreateTable_Floorplan();
void CreateTable_FloorplanObjectType();
void CreateTable_FloorplanObjectType_Color();
void CreateTable_FloorplanType();
void CreateTable_HorizAlignment();
void CreateTable_HouseMode();
void CreateTable_Household();
void CreateTable_Household_Installation();
void CreateTable_Icon();
void CreateTable_Image();
void CreateTable_InfraredGroup();
void CreateTable_InfraredGroup_Command();
void CreateTable_InfraredGroup_Command_Preferred();
void CreateTable_InstallWizard();
void CreateTable_InstallWizard_Distro();
void CreateTable_Installation();
void CreateTable_Installation_RepositorySource_URL();
void CreateTable_Installation_Users();
void CreateTable_Language();
void CreateTable_License();
void CreateTable_Licensing();
void CreateTable_Manufacturer();
void CreateTable_MediaType();
void CreateTable_MediaType_Broadcast();
void CreateTable_MediaType_DesignObj();
void CreateTable_News();
void CreateTable_OperatingSystem();
void CreateTable_Orbiter();
void CreateTable_Orbiter_Users_PasswordReq();
void CreateTable_Orbiter_Variable();
void CreateTable_Package();
void CreateTable_PackageType();
void CreateTable_Package_Compat();
void CreateTable_Package_Device();
void CreateTable_Package_Directory();
void CreateTable_Package_Directory_File();
void CreateTable_Package_Package();
void CreateTable_Package_Source();
void CreateTable_Package_Source_Compat();
void CreateTable_Package_Users();
void CreateTable_Package_Version();
void CreateTable_PageSetup();
void CreateTable_PaidLicense();
void CreateTable_ParameterType();
void CreateTable_PhoneLineType();
void CreateTable_Pipe();
void CreateTable_RemoteControl();
void CreateTable_RepositorySource();
void CreateTable_RepositorySource_URL();
void CreateTable_RepositoryType();
void CreateTable_Room();
void CreateTable_RoomType();
void CreateTable_Room_Users();
void CreateTable_Schema();
void CreateTable_SetupStep();
void CreateTable_Size();
void CreateTable_Skin();
void CreateTable_StabilityStatus();
void CreateTable_StartupScript();
void CreateTable_Style();
void CreateTable_StyleVariation();
void CreateTable_System();
void CreateTable_Template();
void CreateTable_Text();
void CreateTable_TextCategory();
void CreateTable_Text_LS();
void CreateTable_Text_LS_AltVersions();
void CreateTable_UnknownDevices();
void CreateTable_UserMode();
void CreateTable_Users();
void CreateTable_Variable();
void CreateTable_Version();
void CreateTable_VertAlignment();
void CreateTable_groupaddresses();
void CreateTable_psc_constants_batdet();
void CreateTable_psc_constants_bathdr();
void CreateTable_psc_constants_batuser();
void CreateTable_psc_constants_repset();
void CreateTable_psc_constants_schema();
void CreateTable_psc_constants_tables();
void CreateTable_psc_dce_batdet();
void CreateTable_psc_dce_bathdr();
void CreateTable_psc_dce_batuser();
void CreateTable_psc_dce_repset();
void CreateTable_psc_dce_schema();
void CreateTable_psc_dce_tables();
void CreateTable_psc_designer_batdet();
void CreateTable_psc_designer_bathdr();
void CreateTable_psc_designer_batuser();
void CreateTable_psc_designer_repset();
void CreateTable_psc_designer_schema();
void CreateTable_psc_designer_tables();
void CreateTable_psc_document_batdet();
void CreateTable_psc_document_bathdr();
void CreateTable_psc_document_batuser();
void CreateTable_psc_document_repset();
void CreateTable_psc_document_schema();
void CreateTable_psc_document_tables();
void CreateTable_psc_ir_batdet();
void CreateTable_psc_ir_bathdr();
void CreateTable_psc_ir_batuser();
void CreateTable_psc_ir_repset();
void CreateTable_psc_ir_schema();
void CreateTable_psc_ir_tables();
void CreateTable_psc_local_batdet();
void CreateTable_psc_local_bathdr();
void CreateTable_psc_local_batuser();
void CreateTable_psc_local_repset();
void CreateTable_psc_local_schema();
void CreateTable_psc_local_tables();
void CreateTable_psc_website_batdet();
void CreateTable_psc_website_bathdr();
void CreateTable_psc_website_batuser();
void CreateTable_psc_website_repset();
void CreateTable_psc_website_schema();
void CreateTable_psc_website_tables();
void DeleteTable_Array();
void DeleteTable_Broadcast();
void DeleteTable_Button();
void DeleteTable_CachedScreens();
void DeleteTable_CannedEvents();
void DeleteTable_CannedEvents_CriteriaParmList();
void DeleteTable_Command();
void DeleteTable_CommandCategory();
void DeleteTable_CommandGroup();
void DeleteTable_CommandGroup_Command();
void DeleteTable_CommandGroup_Command_CommandParameter();
void DeleteTable_CommandGroup_D();
void DeleteTable_CommandGroup_D_Command();
void DeleteTable_CommandGroup_D_Command_CommandParameter();
void DeleteTable_CommandGroup_EntertainArea();
void DeleteTable_CommandGroup_Room();
void DeleteTable_CommandParameter();
void DeleteTable_Command_CommandParameter();
void DeleteTable_Command_Pipe();
void DeleteTable_ConfigType();
void DeleteTable_ConfigType_File();
void DeleteTable_ConfigType_Setting();
void DeleteTable_ConfigType_Token();
void DeleteTable_ConnectorType();
void DeleteTable_Country();
void DeleteTable_Criteria();
void DeleteTable_CriteriaList();
void DeleteTable_CriteriaList_CriteriaParmList();
void DeleteTable_CriteriaParm();
void DeleteTable_CriteriaParmList();
void DeleteTable_CriteriaParmNesting();
void DeleteTable_CriteriaParmNesting_D();
void DeleteTable_CriteriaParm_D();
void DeleteTable_Criteria_D();
void DeleteTable_DHCPDevice();
void DeleteTable_DHCPDevice_DeviceData();
void DeleteTable_DataGrid();
void DeleteTable_DesignObj();
void DeleteTable_DesignObjCategory();
void DeleteTable_DesignObjParameter();
void DeleteTable_DesignObjType();
void DeleteTable_DesignObjType_DesignObjParameter();
void DeleteTable_DesignObjVariation();
void DeleteTable_DesignObjVariation_DesignObj();
void DeleteTable_DesignObjVariation_DesignObjParameter();
void DeleteTable_DesignObjVariation_Text();
void DeleteTable_DesignObjVariation_Text_Skin_Language();
void DeleteTable_DesignObjVariation_Zone();
void DeleteTable_Device();
void DeleteTable_DeviceCategory();
void DeleteTable_DeviceCategory_DeviceData();
void DeleteTable_DeviceCategory_Event();
void DeleteTable_DeviceCommandGroup();
void DeleteTable_DeviceCommandGroup_Command();
void DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
void DeleteTable_DeviceData();
void DeleteTable_DeviceGroup();
void DeleteTable_DeviceTemplate();
void DeleteTable_DeviceTemplate_AV();
void DeleteTable_DeviceTemplate_DSPMode();
void DeleteTable_DeviceTemplate_DesignObj();
void DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia();
void DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
void DeleteTable_DeviceTemplate_DeviceCommandGroup();
void DeleteTable_DeviceTemplate_DeviceData();
void DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia();
void DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
void DeleteTable_DeviceTemplate_DeviceTemplate_Related();
void DeleteTable_DeviceTemplate_Event();
void DeleteTable_DeviceTemplate_InfraredGroup();
void DeleteTable_DeviceTemplate_Input();
void DeleteTable_DeviceTemplate_MediaType();
void DeleteTable_DeviceTemplate_MediaType_DesignObj();
void DeleteTable_DeviceTemplate_Output();
void DeleteTable_DeviceTemplate_PageSetup();
void DeleteTable_Device_Command();
void DeleteTable_Device_CommandGroup();
void DeleteTable_Device_DeviceData();
void DeleteTable_Device_DeviceGroup();
void DeleteTable_Device_Device_Pipe();
void DeleteTable_Device_Device_Related();
void DeleteTable_Device_EntertainArea();
void DeleteTable_Device_HouseMode();
void DeleteTable_Device_MRU();
void DeleteTable_Device_Orbiter();
void DeleteTable_Device_QuickStart();
void DeleteTable_Device_StartupScript();
void DeleteTable_Device_Users();
void DeleteTable_Direction();
void DeleteTable_Directory();
void DeleteTable_Distro();
void DeleteTable_Document();
void DeleteTable_Document_Comment();
void DeleteTable_EntertainArea();
void DeleteTable_Event();
void DeleteTable_EventCategory();
void DeleteTable_EventHandler();
void DeleteTable_EventParameter();
void DeleteTable_Event_EventParameter();
void DeleteTable_FAQ();
void DeleteTable_Firewall();
void DeleteTable_Floorplan();
void DeleteTable_FloorplanObjectType();
void DeleteTable_FloorplanObjectType_Color();
void DeleteTable_FloorplanType();
void DeleteTable_HorizAlignment();
void DeleteTable_HouseMode();
void DeleteTable_Household();
void DeleteTable_Household_Installation();
void DeleteTable_Icon();
void DeleteTable_Image();
void DeleteTable_InfraredGroup();
void DeleteTable_InfraredGroup_Command();
void DeleteTable_InfraredGroup_Command_Preferred();
void DeleteTable_InstallWizard();
void DeleteTable_InstallWizard_Distro();
void DeleteTable_Installation();
void DeleteTable_Installation_RepositorySource_URL();
void DeleteTable_Installation_Users();
void DeleteTable_Language();
void DeleteTable_License();
void DeleteTable_Licensing();
void DeleteTable_Manufacturer();
void DeleteTable_MediaType();
void DeleteTable_MediaType_Broadcast();
void DeleteTable_MediaType_DesignObj();
void DeleteTable_News();
void DeleteTable_OperatingSystem();
void DeleteTable_Orbiter();
void DeleteTable_Orbiter_Users_PasswordReq();
void DeleteTable_Orbiter_Variable();
void DeleteTable_Package();
void DeleteTable_PackageType();
void DeleteTable_Package_Compat();
void DeleteTable_Package_Device();
void DeleteTable_Package_Directory();
void DeleteTable_Package_Directory_File();
void DeleteTable_Package_Package();
void DeleteTable_Package_Source();
void DeleteTable_Package_Source_Compat();
void DeleteTable_Package_Users();
void DeleteTable_Package_Version();
void DeleteTable_PageSetup();
void DeleteTable_PaidLicense();
void DeleteTable_ParameterType();
void DeleteTable_PhoneLineType();
void DeleteTable_Pipe();
void DeleteTable_RemoteControl();
void DeleteTable_RepositorySource();
void DeleteTable_RepositorySource_URL();
void DeleteTable_RepositoryType();
void DeleteTable_Room();
void DeleteTable_RoomType();
void DeleteTable_Room_Users();
void DeleteTable_Schema();
void DeleteTable_SetupStep();
void DeleteTable_Size();
void DeleteTable_Skin();
void DeleteTable_StabilityStatus();
void DeleteTable_StartupScript();
void DeleteTable_Style();
void DeleteTable_StyleVariation();
void DeleteTable_System();
void DeleteTable_Template();
void DeleteTable_Text();
void DeleteTable_TextCategory();
void DeleteTable_Text_LS();
void DeleteTable_Text_LS_AltVersions();
void DeleteTable_UnknownDevices();
void DeleteTable_UserMode();
void DeleteTable_Users();
void DeleteTable_Variable();
void DeleteTable_Version();
void DeleteTable_VertAlignment();
void DeleteTable_groupaddresses();
void DeleteTable_psc_constants_batdet();
void DeleteTable_psc_constants_bathdr();
void DeleteTable_psc_constants_batuser();
void DeleteTable_psc_constants_repset();
void DeleteTable_psc_constants_schema();
void DeleteTable_psc_constants_tables();
void DeleteTable_psc_dce_batdet();
void DeleteTable_psc_dce_bathdr();
void DeleteTable_psc_dce_batuser();
void DeleteTable_psc_dce_repset();
void DeleteTable_psc_dce_schema();
void DeleteTable_psc_dce_tables();
void DeleteTable_psc_designer_batdet();
void DeleteTable_psc_designer_bathdr();
void DeleteTable_psc_designer_batuser();
void DeleteTable_psc_designer_repset();
void DeleteTable_psc_designer_schema();
void DeleteTable_psc_designer_tables();
void DeleteTable_psc_document_batdet();
void DeleteTable_psc_document_bathdr();
void DeleteTable_psc_document_batuser();
void DeleteTable_psc_document_repset();
void DeleteTable_psc_document_schema();
void DeleteTable_psc_document_tables();
void DeleteTable_psc_ir_batdet();
void DeleteTable_psc_ir_bathdr();
void DeleteTable_psc_ir_batuser();
void DeleteTable_psc_ir_repset();
void DeleteTable_psc_ir_schema();
void DeleteTable_psc_ir_tables();
void DeleteTable_psc_local_batdet();
void DeleteTable_psc_local_bathdr();
void DeleteTable_psc_local_batuser();
void DeleteTable_psc_local_repset();
void DeleteTable_psc_local_schema();
void DeleteTable_psc_local_tables();
void DeleteTable_psc_website_batdet();
void DeleteTable_psc_website_bathdr();
void DeleteTable_psc_website_batuser();
void DeleteTable_psc_website_repset();
void DeleteTable_psc_website_schema();
void DeleteTable_psc_website_tables();
};
#endif
