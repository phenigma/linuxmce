#ifndef __Database_pluto_main_H_
#define __Database_pluto_main_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
class DLL_EXPORT Database_pluto_main
{
public:
MYSQL *db_handle;
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
class Table_DSPMode* tblDSPMode;
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
class Table_Device_Orbiter* tblDevice_Orbiter;
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
class Table_Input* tblInput;
class Table_InstallWizard* tblInstallWizard;
class Table_InstallWizard_Distro* tblInstallWizard_Distro;
class Table_Installation* tblInstallation;
class Table_Installation_RepositorySource_URL* tblInstallation_RepositorySource_URL;
class Table_Installation_Users* tblInstallation_Users;
class Table_Language* tblLanguage;
class Table_License* tblLicense;
class Table_Manufacturer* tblManufacturer;
class Table_MediaType* tblMediaType;
class Table_MediaType_Broadcast* tblMediaType_Broadcast;
class Table_News* tblNews;
class Table_OperatingSystem* tblOperatingSystem;
class Table_Orbiter* tblOrbiter;
class Table_Orbiter_Users_PasswordReq* tblOrbiter_Users_PasswordReq;
class Table_Orbiter_Variable* tblOrbiter_Variable;
class Table_Output* tblOutput;
class Table_Package* tblPackage;
class Table_PackageType* tblPackageType;
class Table_Package_Compat* tblPackage_Compat;
class Table_Package_Directory* tblPackage_Directory;
class Table_Package_Directory_File* tblPackage_Directory_File;
class Table_Package_Package* tblPackage_Package;
class Table_Package_Source* tblPackage_Source;
class Table_Package_Source_Compat* tblPackage_Source_Compat;
class Table_Package_Users* tblPackage_Users;
class Table_Package_Version* tblPackage_Version;
class Table_PageSetup* tblPageSetup;
class Table_ParameterType* tblParameterType;
class Table_PhoneLineType* tblPhoneLineType;
class Table_Pipe* tblPipe;
class Table_RepositorySource* tblRepositorySource;
class Table_RepositorySource_URL* tblRepositorySource_URL;
class Table_RepositoryType* tblRepositoryType;
class Table_Room* tblRoom;
class Table_RoomType* tblRoomType;
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
class Table_Array* Array_get() { return tblArray; }
class Table_Broadcast* Broadcast_get() { return tblBroadcast; }
class Table_Button* Button_get() { return tblButton; }
class Table_CachedScreens* CachedScreens_get() { return tblCachedScreens; }
class Table_CannedEvents* CannedEvents_get() { return tblCannedEvents; }
class Table_CannedEvents_CriteriaParmList* CannedEvents_CriteriaParmList_get() { return tblCannedEvents_CriteriaParmList; }
class Table_Command* Command_get() { return tblCommand; }
class Table_CommandCategory* CommandCategory_get() { return tblCommandCategory; }
class Table_CommandGroup* CommandGroup_get() { return tblCommandGroup; }
class Table_CommandGroup_Command* CommandGroup_Command_get() { return tblCommandGroup_Command; }
class Table_CommandGroup_Command_CommandParameter* CommandGroup_Command_CommandParameter_get() { return tblCommandGroup_Command_CommandParameter; }
class Table_CommandGroup_D* CommandGroup_D_get() { return tblCommandGroup_D; }
class Table_CommandGroup_D_Command* CommandGroup_D_Command_get() { return tblCommandGroup_D_Command; }
class Table_CommandGroup_D_Command_CommandParameter* CommandGroup_D_Command_CommandParameter_get() { return tblCommandGroup_D_Command_CommandParameter; }
class Table_CommandGroup_EntertainArea* CommandGroup_EntertainArea_get() { return tblCommandGroup_EntertainArea; }
class Table_CommandGroup_Room* CommandGroup_Room_get() { return tblCommandGroup_Room; }
class Table_CommandParameter* CommandParameter_get() { return tblCommandParameter; }
class Table_Command_CommandParameter* Command_CommandParameter_get() { return tblCommand_CommandParameter; }
class Table_Command_Pipe* Command_Pipe_get() { return tblCommand_Pipe; }
class Table_ConfigType* ConfigType_get() { return tblConfigType; }
class Table_ConfigType_File* ConfigType_File_get() { return tblConfigType_File; }
class Table_ConfigType_Setting* ConfigType_Setting_get() { return tblConfigType_Setting; }
class Table_ConfigType_Token* ConfigType_Token_get() { return tblConfigType_Token; }
class Table_ConnectorType* ConnectorType_get() { return tblConnectorType; }
class Table_Country* Country_get() { return tblCountry; }
class Table_Criteria* Criteria_get() { return tblCriteria; }
class Table_CriteriaList* CriteriaList_get() { return tblCriteriaList; }
class Table_CriteriaList_CriteriaParmList* CriteriaList_CriteriaParmList_get() { return tblCriteriaList_CriteriaParmList; }
class Table_CriteriaParm* CriteriaParm_get() { return tblCriteriaParm; }
class Table_CriteriaParmList* CriteriaParmList_get() { return tblCriteriaParmList; }
class Table_CriteriaParmNesting* CriteriaParmNesting_get() { return tblCriteriaParmNesting; }
class Table_CriteriaParmNesting_D* CriteriaParmNesting_D_get() { return tblCriteriaParmNesting_D; }
class Table_CriteriaParm_D* CriteriaParm_D_get() { return tblCriteriaParm_D; }
class Table_Criteria_D* Criteria_D_get() { return tblCriteria_D; }
class Table_DSPMode* DSPMode_get() { return tblDSPMode; }
class Table_DataGrid* DataGrid_get() { return tblDataGrid; }
class Table_DesignObj* DesignObj_get() { return tblDesignObj; }
class Table_DesignObjCategory* DesignObjCategory_get() { return tblDesignObjCategory; }
class Table_DesignObjParameter* DesignObjParameter_get() { return tblDesignObjParameter; }
class Table_DesignObjType* DesignObjType_get() { return tblDesignObjType; }
class Table_DesignObjType_DesignObjParameter* DesignObjType_DesignObjParameter_get() { return tblDesignObjType_DesignObjParameter; }
class Table_DesignObjVariation* DesignObjVariation_get() { return tblDesignObjVariation; }
class Table_DesignObjVariation_DesignObj* DesignObjVariation_DesignObj_get() { return tblDesignObjVariation_DesignObj; }
class Table_DesignObjVariation_DesignObjParameter* DesignObjVariation_DesignObjParameter_get() { return tblDesignObjVariation_DesignObjParameter; }
class Table_DesignObjVariation_Text* DesignObjVariation_Text_get() { return tblDesignObjVariation_Text; }
class Table_DesignObjVariation_Text_Skin_Language* DesignObjVariation_Text_Skin_Language_get() { return tblDesignObjVariation_Text_Skin_Language; }
class Table_DesignObjVariation_Zone* DesignObjVariation_Zone_get() { return tblDesignObjVariation_Zone; }
class Table_Device* Device_get() { return tblDevice; }
class Table_DeviceCategory* DeviceCategory_get() { return tblDeviceCategory; }
class Table_DeviceCategory_DeviceData* DeviceCategory_DeviceData_get() { return tblDeviceCategory_DeviceData; }
class Table_DeviceCategory_Event* DeviceCategory_Event_get() { return tblDeviceCategory_Event; }
class Table_DeviceCommandGroup* DeviceCommandGroup_get() { return tblDeviceCommandGroup; }
class Table_DeviceCommandGroup_Command* DeviceCommandGroup_Command_get() { return tblDeviceCommandGroup_Command; }
class Table_DeviceCommandGroup_DeviceCommandGroup_Parent* DeviceCommandGroup_DeviceCommandGroup_Parent_get() { return tblDeviceCommandGroup_DeviceCommandGroup_Parent; }
class Table_DeviceData* DeviceData_get() { return tblDeviceData; }
class Table_DeviceGroup* DeviceGroup_get() { return tblDeviceGroup; }
class Table_DeviceTemplate* DeviceTemplate_get() { return tblDeviceTemplate; }
class Table_DeviceTemplate_AV* DeviceTemplate_AV_get() { return tblDeviceTemplate_AV; }
class Table_DeviceTemplate_DSPMode* DeviceTemplate_DSPMode_get() { return tblDeviceTemplate_DSPMode; }
class Table_DeviceTemplate_DesignObj* DeviceTemplate_DesignObj_get() { return tblDeviceTemplate_DesignObj; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia* DeviceTemplate_DeviceCategory_ControlledVia_get() { return tblDeviceTemplate_DeviceCategory_ControlledVia; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get() { return tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceCommandGroup* DeviceTemplate_DeviceCommandGroup_get() { return tblDeviceTemplate_DeviceCommandGroup; }
class Table_DeviceTemplate_DeviceData* DeviceTemplate_DeviceData_get() { return tblDeviceTemplate_DeviceData; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia* DeviceTemplate_DeviceTemplate_ControlledVia_get() { return tblDeviceTemplate_DeviceTemplate_ControlledVia; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe* DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_get() { return tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceTemplate_Related* DeviceTemplate_DeviceTemplate_Related_get() { return tblDeviceTemplate_DeviceTemplate_Related; }
class Table_DeviceTemplate_Event* DeviceTemplate_Event_get() { return tblDeviceTemplate_Event; }
class Table_DeviceTemplate_InfraredGroup* DeviceTemplate_InfraredGroup_get() { return tblDeviceTemplate_InfraredGroup; }
class Table_DeviceTemplate_Input* DeviceTemplate_Input_get() { return tblDeviceTemplate_Input; }
class Table_DeviceTemplate_MediaType* DeviceTemplate_MediaType_get() { return tblDeviceTemplate_MediaType; }
class Table_DeviceTemplate_MediaType_DesignObj* DeviceTemplate_MediaType_DesignObj_get() { return tblDeviceTemplate_MediaType_DesignObj; }
class Table_DeviceTemplate_Output* DeviceTemplate_Output_get() { return tblDeviceTemplate_Output; }
class Table_DeviceTemplate_PageSetup* DeviceTemplate_PageSetup_get() { return tblDeviceTemplate_PageSetup; }
class Table_Device_Command* Device_Command_get() { return tblDevice_Command; }
class Table_Device_CommandGroup* Device_CommandGroup_get() { return tblDevice_CommandGroup; }
class Table_Device_DeviceData* Device_DeviceData_get() { return tblDevice_DeviceData; }
class Table_Device_DeviceGroup* Device_DeviceGroup_get() { return tblDevice_DeviceGroup; }
class Table_Device_Device_Pipe* Device_Device_Pipe_get() { return tblDevice_Device_Pipe; }
class Table_Device_Device_Related* Device_Device_Related_get() { return tblDevice_Device_Related; }
class Table_Device_EntertainArea* Device_EntertainArea_get() { return tblDevice_EntertainArea; }
class Table_Device_HouseMode* Device_HouseMode_get() { return tblDevice_HouseMode; }
class Table_Device_Orbiter* Device_Orbiter_get() { return tblDevice_Orbiter; }
class Table_Device_StartupScript* Device_StartupScript_get() { return tblDevice_StartupScript; }
class Table_Device_Users* Device_Users_get() { return tblDevice_Users; }
class Table_Direction* Direction_get() { return tblDirection; }
class Table_Directory* Directory_get() { return tblDirectory; }
class Table_Distro* Distro_get() { return tblDistro; }
class Table_Document* Document_get() { return tblDocument; }
class Table_Document_Comment* Document_Comment_get() { return tblDocument_Comment; }
class Table_EntertainArea* EntertainArea_get() { return tblEntertainArea; }
class Table_Event* Event_get() { return tblEvent; }
class Table_EventCategory* EventCategory_get() { return tblEventCategory; }
class Table_EventHandler* EventHandler_get() { return tblEventHandler; }
class Table_EventParameter* EventParameter_get() { return tblEventParameter; }
class Table_Event_EventParameter* Event_EventParameter_get() { return tblEvent_EventParameter; }
class Table_Firewall* Firewall_get() { return tblFirewall; }
class Table_Floorplan* Floorplan_get() { return tblFloorplan; }
class Table_FloorplanObjectType* FloorplanObjectType_get() { return tblFloorplanObjectType; }
class Table_FloorplanObjectType_Color* FloorplanObjectType_Color_get() { return tblFloorplanObjectType_Color; }
class Table_FloorplanType* FloorplanType_get() { return tblFloorplanType; }
class Table_HorizAlignment* HorizAlignment_get() { return tblHorizAlignment; }
class Table_HouseMode* HouseMode_get() { return tblHouseMode; }
class Table_Household* Household_get() { return tblHousehold; }
class Table_Household_Installation* Household_Installation_get() { return tblHousehold_Installation; }
class Table_Icon* Icon_get() { return tblIcon; }
class Table_Image* Image_get() { return tblImage; }
class Table_InfraredGroup* InfraredGroup_get() { return tblInfraredGroup; }
class Table_InfraredGroup_Command* InfraredGroup_Command_get() { return tblInfraredGroup_Command; }
class Table_Input* Input_get() { return tblInput; }
class Table_InstallWizard* InstallWizard_get() { return tblInstallWizard; }
class Table_InstallWizard_Distro* InstallWizard_Distro_get() { return tblInstallWizard_Distro; }
class Table_Installation* Installation_get() { return tblInstallation; }
class Table_Installation_RepositorySource_URL* Installation_RepositorySource_URL_get() { return tblInstallation_RepositorySource_URL; }
class Table_Installation_Users* Installation_Users_get() { return tblInstallation_Users; }
class Table_Language* Language_get() { return tblLanguage; }
class Table_License* License_get() { return tblLicense; }
class Table_Manufacturer* Manufacturer_get() { return tblManufacturer; }
class Table_MediaType* MediaType_get() { return tblMediaType; }
class Table_MediaType_Broadcast* MediaType_Broadcast_get() { return tblMediaType_Broadcast; }
class Table_News* News_get() { return tblNews; }
class Table_OperatingSystem* OperatingSystem_get() { return tblOperatingSystem; }
class Table_Orbiter* Orbiter_get() { return tblOrbiter; }
class Table_Orbiter_Users_PasswordReq* Orbiter_Users_PasswordReq_get() { return tblOrbiter_Users_PasswordReq; }
class Table_Orbiter_Variable* Orbiter_Variable_get() { return tblOrbiter_Variable; }
class Table_Output* Output_get() { return tblOutput; }
class Table_Package* Package_get() { return tblPackage; }
class Table_PackageType* PackageType_get() { return tblPackageType; }
class Table_Package_Compat* Package_Compat_get() { return tblPackage_Compat; }
class Table_Package_Directory* Package_Directory_get() { return tblPackage_Directory; }
class Table_Package_Directory_File* Package_Directory_File_get() { return tblPackage_Directory_File; }
class Table_Package_Package* Package_Package_get() { return tblPackage_Package; }
class Table_Package_Source* Package_Source_get() { return tblPackage_Source; }
class Table_Package_Source_Compat* Package_Source_Compat_get() { return tblPackage_Source_Compat; }
class Table_Package_Users* Package_Users_get() { return tblPackage_Users; }
class Table_Package_Version* Package_Version_get() { return tblPackage_Version; }
class Table_PageSetup* PageSetup_get() { return tblPageSetup; }
class Table_ParameterType* ParameterType_get() { return tblParameterType; }
class Table_PhoneLineType* PhoneLineType_get() { return tblPhoneLineType; }
class Table_Pipe* Pipe_get() { return tblPipe; }
class Table_RepositorySource* RepositorySource_get() { return tblRepositorySource; }
class Table_RepositorySource_URL* RepositorySource_URL_get() { return tblRepositorySource_URL; }
class Table_RepositoryType* RepositoryType_get() { return tblRepositoryType; }
class Table_Room* Room_get() { return tblRoom; }
class Table_RoomType* RoomType_get() { return tblRoomType; }
class Table_Schema* Schema_get() { return tblSchema; }
class Table_SetupStep* SetupStep_get() { return tblSetupStep; }
class Table_Size* Size_get() { return tblSize; }
class Table_Skin* Skin_get() { return tblSkin; }
class Table_StabilityStatus* StabilityStatus_get() { return tblStabilityStatus; }
class Table_StartupScript* StartupScript_get() { return tblStartupScript; }
class Table_Style* Style_get() { return tblStyle; }
class Table_StyleVariation* StyleVariation_get() { return tblStyleVariation; }
class Table_System* System_get() { return tblSystem; }
class Table_Template* Template_get() { return tblTemplate; }
class Table_Text* Text_get() { return tblText; }
class Table_TextCategory* TextCategory_get() { return tblTextCategory; }
class Table_Text_LS* Text_LS_get() { return tblText_LS; }
class Table_Text_LS_AltVersions* Text_LS_AltVersions_get() { return tblText_LS_AltVersions; }
class Table_UnknownDevices* UnknownDevices_get() { return tblUnknownDevices; }
class Table_UserMode* UserMode_get() { return tblUserMode; }
class Table_Users* Users_get() { return tblUsers; }
class Table_Variable* Variable_get() { return tblVariable; }
class Table_Version* Version_get() { return tblVersion; }
class Table_VertAlignment* VertAlignment_get() { return tblVertAlignment; }
class Table_psc_constants_batdet* psc_constants_batdet_get() { return tblpsc_constants_batdet; }
class Table_psc_constants_bathdr* psc_constants_bathdr_get() { return tblpsc_constants_bathdr; }
class Table_psc_constants_batuser* psc_constants_batuser_get() { return tblpsc_constants_batuser; }
class Table_psc_constants_repset* psc_constants_repset_get() { return tblpsc_constants_repset; }
class Table_psc_constants_schema* psc_constants_schema_get() { return tblpsc_constants_schema; }
class Table_psc_constants_tables* psc_constants_tables_get() { return tblpsc_constants_tables; }
class Table_psc_dce_batdet* psc_dce_batdet_get() { return tblpsc_dce_batdet; }
class Table_psc_dce_bathdr* psc_dce_bathdr_get() { return tblpsc_dce_bathdr; }
class Table_psc_dce_batuser* psc_dce_batuser_get() { return tblpsc_dce_batuser; }
class Table_psc_dce_repset* psc_dce_repset_get() { return tblpsc_dce_repset; }
class Table_psc_dce_schema* psc_dce_schema_get() { return tblpsc_dce_schema; }
class Table_psc_dce_tables* psc_dce_tables_get() { return tblpsc_dce_tables; }
class Table_psc_designer_batdet* psc_designer_batdet_get() { return tblpsc_designer_batdet; }
class Table_psc_designer_bathdr* psc_designer_bathdr_get() { return tblpsc_designer_bathdr; }
class Table_psc_designer_batuser* psc_designer_batuser_get() { return tblpsc_designer_batuser; }
class Table_psc_designer_repset* psc_designer_repset_get() { return tblpsc_designer_repset; }
class Table_psc_designer_schema* psc_designer_schema_get() { return tblpsc_designer_schema; }
class Table_psc_designer_tables* psc_designer_tables_get() { return tblpsc_designer_tables; }
class Table_psc_document_batdet* psc_document_batdet_get() { return tblpsc_document_batdet; }
class Table_psc_document_bathdr* psc_document_bathdr_get() { return tblpsc_document_bathdr; }
class Table_psc_document_batuser* psc_document_batuser_get() { return tblpsc_document_batuser; }
class Table_psc_document_repset* psc_document_repset_get() { return tblpsc_document_repset; }
class Table_psc_document_schema* psc_document_schema_get() { return tblpsc_document_schema; }
class Table_psc_document_tables* psc_document_tables_get() { return tblpsc_document_tables; }
class Table_psc_ir_batdet* psc_ir_batdet_get() { return tblpsc_ir_batdet; }
class Table_psc_ir_bathdr* psc_ir_bathdr_get() { return tblpsc_ir_bathdr; }
class Table_psc_ir_batuser* psc_ir_batuser_get() { return tblpsc_ir_batuser; }
class Table_psc_ir_repset* psc_ir_repset_get() { return tblpsc_ir_repset; }
class Table_psc_ir_schema* psc_ir_schema_get() { return tblpsc_ir_schema; }
class Table_psc_ir_tables* psc_ir_tables_get() { return tblpsc_ir_tables; }
class Table_psc_local_batdet* psc_local_batdet_get() { return tblpsc_local_batdet; }
class Table_psc_local_bathdr* psc_local_bathdr_get() { return tblpsc_local_bathdr; }
class Table_psc_local_batuser* psc_local_batuser_get() { return tblpsc_local_batuser; }
class Table_psc_local_repset* psc_local_repset_get() { return tblpsc_local_repset; }
class Table_psc_local_schema* psc_local_schema_get() { return tblpsc_local_schema; }
class Table_psc_local_tables* psc_local_tables_get() { return tblpsc_local_tables; }
class Table_psc_website_batdet* psc_website_batdet_get() { return tblpsc_website_batdet; }
class Table_psc_website_bathdr* psc_website_bathdr_get() { return tblpsc_website_bathdr; }
class Table_psc_website_batuser* psc_website_batuser_get() { return tblpsc_website_batuser; }
class Table_psc_website_repset* psc_website_repset_get() { return tblpsc_website_repset; }
class Table_psc_website_schema* psc_website_schema_get() { return tblpsc_website_schema; }
class Table_psc_website_tables* psc_website_tables_get() { return tblpsc_website_tables; }
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
void CreateTable_DSPMode();
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
void CreateTable_Device_Orbiter();
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
void CreateTable_Input();
void CreateTable_InstallWizard();
void CreateTable_InstallWizard_Distro();
void CreateTable_Installation();
void CreateTable_Installation_RepositorySource_URL();
void CreateTable_Installation_Users();
void CreateTable_Language();
void CreateTable_License();
void CreateTable_Manufacturer();
void CreateTable_MediaType();
void CreateTable_MediaType_Broadcast();
void CreateTable_News();
void CreateTable_OperatingSystem();
void CreateTable_Orbiter();
void CreateTable_Orbiter_Users_PasswordReq();
void CreateTable_Orbiter_Variable();
void CreateTable_Output();
void CreateTable_Package();
void CreateTable_PackageType();
void CreateTable_Package_Compat();
void CreateTable_Package_Directory();
void CreateTable_Package_Directory_File();
void CreateTable_Package_Package();
void CreateTable_Package_Source();
void CreateTable_Package_Source_Compat();
void CreateTable_Package_Users();
void CreateTable_Package_Version();
void CreateTable_PageSetup();
void CreateTable_ParameterType();
void CreateTable_PhoneLineType();
void CreateTable_Pipe();
void CreateTable_RepositorySource();
void CreateTable_RepositorySource_URL();
void CreateTable_RepositoryType();
void CreateTable_Room();
void CreateTable_RoomType();
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
void DeleteTable_DSPMode();
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
void DeleteTable_Device_Orbiter();
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
void DeleteTable_Input();
void DeleteTable_InstallWizard();
void DeleteTable_InstallWizard_Distro();
void DeleteTable_Installation();
void DeleteTable_Installation_RepositorySource_URL();
void DeleteTable_Installation_Users();
void DeleteTable_Language();
void DeleteTable_License();
void DeleteTable_Manufacturer();
void DeleteTable_MediaType();
void DeleteTable_MediaType_Broadcast();
void DeleteTable_News();
void DeleteTable_OperatingSystem();
void DeleteTable_Orbiter();
void DeleteTable_Orbiter_Users_PasswordReq();
void DeleteTable_Orbiter_Variable();
void DeleteTable_Output();
void DeleteTable_Package();
void DeleteTable_PackageType();
void DeleteTable_Package_Compat();
void DeleteTable_Package_Directory();
void DeleteTable_Package_Directory_File();
void DeleteTable_Package_Package();
void DeleteTable_Package_Source();
void DeleteTable_Package_Source_Compat();
void DeleteTable_Package_Users();
void DeleteTable_Package_Version();
void DeleteTable_PageSetup();
void DeleteTable_ParameterType();
void DeleteTable_PhoneLineType();
void DeleteTable_Pipe();
void DeleteTable_RepositorySource();
void DeleteTable_RepositorySource_URL();
void DeleteTable_RepositoryType();
void DeleteTable_Room();
void DeleteTable_RoomType();
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
