#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_main.h"


#include "DCEConfig.h"


namespace DCE
{
	Logger *g_pPlutoLogger; //dummy
}
using namespace DCE;
Database_pluto_main::Database_pluto_main()
{
tblArray=NULL;
tblBroadcast=NULL;
tblButton=NULL;
tblCachedScreens=NULL;
tblCannedEvents=NULL;
tblCannedEvents_CriteriaParmList=NULL;
tblCity=NULL;
tblCommMethod=NULL;
tblCommand=NULL;
tblCommandCategory=NULL;
tblCommandGroup=NULL;
tblCommandGroup_Command=NULL;
tblCommandGroup_Command_CommandParameter=NULL;
tblCommandGroup_D=NULL;
tblCommandGroup_D_Command=NULL;
tblCommandGroup_D_Command_CommandParameter=NULL;
tblCommandGroup_EntertainArea=NULL;
tblCommandGroup_Room=NULL;
tblCommandParameter=NULL;
tblCommand_CommandParameter=NULL;
tblCommand_Pipe=NULL;
tblConfigType=NULL;
tblConfigType_File=NULL;
tblConfigType_Setting=NULL;
tblConfigType_Token=NULL;
tblConnectorType=NULL;
tblCountry=NULL;
tblCriteria=NULL;
tblCriteriaList=NULL;
tblCriteriaList_CriteriaParmList=NULL;
tblCriteriaParm=NULL;
tblCriteriaParmList=NULL;
tblCriteriaParmNesting=NULL;
tblDHCPDevice=NULL;
tblDHCPDevice_DeviceData=NULL;
tblDataGrid=NULL;
tblDesignObj=NULL;
tblDesignObjCategory=NULL;
tblDesignObjParameter=NULL;
tblDesignObjType=NULL;
tblDesignObjType_DesignObjParameter=NULL;
tblDesignObjVariation=NULL;
tblDesignObjVariation_DesignObj=NULL;
tblDesignObjVariation_DesignObjParameter=NULL;
tblDesignObjVariation_Text=NULL;
tblDesignObjVariation_Text_Skin_Language=NULL;
tblDesignObjVariation_Zone=NULL;
tblDevice=NULL;
tblDeviceCategory=NULL;
tblDeviceCategory_DeviceData=NULL;
tblDeviceCategory_Event=NULL;
tblDeviceCommandGroup=NULL;
tblDeviceCommandGroup_Command=NULL;
tblDeviceCommandGroup_DeviceCommandGroup_Parent=NULL;
tblDeviceData=NULL;
tblDeviceGroup=NULL;
tblDeviceTemplate=NULL;
tblDeviceTemplate_AV=NULL;
tblDeviceTemplate_DSPMode=NULL;
tblDeviceTemplate_DesignObj=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceCommandGroup=NULL;
tblDeviceTemplate_DeviceData=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceTemplate_Related=NULL;
tblDeviceTemplate_Event=NULL;
tblDeviceTemplate_InfraredGroup=NULL;
tblDeviceTemplate_Input=NULL;
tblDeviceTemplate_MediaType=NULL;
tblDeviceTemplate_MediaType_DesignObj=NULL;
tblDeviceTemplate_Output=NULL;
tblDeviceTemplate_PageSetup=NULL;
tblDevice_Command=NULL;
tblDevice_CommandGroup=NULL;
tblDevice_DeviceData=NULL;
tblDevice_DeviceGroup=NULL;
tblDevice_Device_Pipe=NULL;
tblDevice_Device_Related=NULL;
tblDevice_EntertainArea=NULL;
tblDevice_HouseMode=NULL;
tblDevice_MRU=NULL;
tblDevice_Orbiter=NULL;
tblDevice_QuickStart=NULL;
tblDevice_StartupScript=NULL;
tblDevice_Users=NULL;
tblDirection=NULL;
tblDirectory=NULL;
tblDistro=NULL;
tblDocument=NULL;
tblDocument_Comment=NULL;
tblEntertainArea=NULL;
tblEvent=NULL;
tblEventCategory=NULL;
tblEventHandler=NULL;
tblEventParameter=NULL;
tblEvent_EventParameter=NULL;
tblFAQ=NULL;
tblFirewall=NULL;
tblFloorplan=NULL;
tblFloorplanObjectType=NULL;
tblFloorplanObjectType_Color=NULL;
tblFloorplanType=NULL;
tblHorizAlignment=NULL;
tblHouseMode=NULL;
tblHousehold=NULL;
tblHousehold_Installation=NULL;
tblIcon=NULL;
tblImage=NULL;
tblInfraredGroup=NULL;
tblInfraredGroup_Command=NULL;
tblInfraredGroup_Command_Preferred=NULL;
tblInstallWizard=NULL;
tblInstallWizard_Distro=NULL;
tblInstallation=NULL;
tblInstallation_RepositorySource_URL=NULL;
tblInstallation_Users=NULL;
tblLanguage=NULL;
tblLicense=NULL;
tblLicensing=NULL;
tblManufacturer=NULL;
tblMediaType=NULL;
tblMediaType_Broadcast=NULL;
tblMediaType_DesignObj=NULL;
tblNews=NULL;
tblOperatingSystem=NULL;
tblOrbiter=NULL;
tblOrbiter_Users_PasswordReq=NULL;
tblOrbiter_Variable=NULL;
tblPackage=NULL;
tblPackageType=NULL;
tblPackage_Compat=NULL;
tblPackage_Device=NULL;
tblPackage_Directory=NULL;
tblPackage_Directory_File=NULL;
tblPackage_Package=NULL;
tblPackage_Source=NULL;
tblPackage_Source_Compat=NULL;
tblPackage_Users=NULL;
tblPackage_Version=NULL;
tblPageSetup=NULL;
tblPaidLicense=NULL;
tblParameterType=NULL;
tblPhoneLineType=NULL;
tblPipe=NULL;
tblQuickStartCategory=NULL;
tblQuickStartTemplate=NULL;
tblRegion=NULL;
tblRemoteControl=NULL;
tblRemoteMapping=NULL;
tblRepositorySource=NULL;
tblRepositorySource_URL=NULL;
tblRepositoryType=NULL;
tblRoom=NULL;
tblRoomType=NULL;
tblRoom_Users=NULL;
tblSchema=NULL;
tblSetupStep=NULL;
tblSize=NULL;
tblSkin=NULL;
tblStabilityStatus=NULL;
tblStartupScript=NULL;
tblStyle=NULL;
tblStyleVariation=NULL;
tblSystem=NULL;
tblTemplate=NULL;
tblText=NULL;
tblTextCategory=NULL;
tblText_LS=NULL;
tblText_LS_AltVersions=NULL;
tblUI=NULL;
tblUnknownDevices=NULL;
tblUserMode=NULL;
tblUsers=NULL;
tblVariable=NULL;
tblVersion=NULL;
tblVertAlignment=NULL;
tblpsc_constants_batdet=NULL;
tblpsc_constants_bathdr=NULL;
tblpsc_constants_batuser=NULL;
tblpsc_constants_repset=NULL;
tblpsc_constants_schema=NULL;
tblpsc_constants_tables=NULL;
tblpsc_dce_batdet=NULL;
tblpsc_dce_bathdr=NULL;
tblpsc_dce_batuser=NULL;
tblpsc_dce_repset=NULL;
tblpsc_dce_schema=NULL;
tblpsc_dce_tables=NULL;
tblpsc_designer_batdet=NULL;
tblpsc_designer_bathdr=NULL;
tblpsc_designer_batuser=NULL;
tblpsc_designer_repset=NULL;
tblpsc_designer_schema=NULL;
tblpsc_designer_tables=NULL;
tblpsc_document_batdet=NULL;
tblpsc_document_bathdr=NULL;
tblpsc_document_batuser=NULL;
tblpsc_document_repset=NULL;
tblpsc_document_schema=NULL;
tblpsc_document_tables=NULL;
tblpsc_ir_batdet=NULL;
tblpsc_ir_bathdr=NULL;
tblpsc_ir_batuser=NULL;
tblpsc_ir_repset=NULL;
tblpsc_ir_schema=NULL;
tblpsc_ir_tables=NULL;
tblpsc_local_batdet=NULL;
tblpsc_local_bathdr=NULL;
tblpsc_local_batuser=NULL;
tblpsc_local_repset=NULL;
tblpsc_local_schema=NULL;
tblpsc_local_tables=NULL;
tblpsc_website_batdet=NULL;
tblpsc_website_bathdr=NULL;
tblpsc_website_batuser=NULL;
tblpsc_website_repset=NULL;
tblpsc_website_schema=NULL;
tblpsc_website_tables=NULL;
}

Database_pluto_main::~Database_pluto_main()
{
	DeleteAllTables();
}

bool Database_pluto_main::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
if( tblArray!=NULL )
Commit_Array(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblBroadcast!=NULL )
Commit_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblButton!=NULL )
Commit_Button(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCachedScreens!=NULL )
Commit_CachedScreens(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCannedEvents!=NULL )
Commit_CannedEvents(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCannedEvents_CriteriaParmList!=NULL )
Commit_CannedEvents_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCity!=NULL )
Commit_City(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommMethod!=NULL )
Commit_CommMethod(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand!=NULL )
Commit_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandCategory!=NULL )
Commit_CommandCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup!=NULL )
Commit_CommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Command!=NULL )
Commit_CommandGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Command_CommandParameter!=NULL )
Commit_CommandGroup_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D!=NULL )
Commit_CommandGroup_D(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D_Command!=NULL )
Commit_CommandGroup_D_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D_Command_CommandParameter!=NULL )
Commit_CommandGroup_D_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_EntertainArea!=NULL )
Commit_CommandGroup_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Room!=NULL )
Commit_CommandGroup_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandParameter!=NULL )
Commit_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand_CommandParameter!=NULL )
Commit_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand_Pipe!=NULL )
Commit_Command_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType!=NULL )
Commit_ConfigType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_File!=NULL )
Commit_ConfigType_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_Setting!=NULL )
Commit_ConfigType_Setting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_Token!=NULL )
Commit_ConfigType_Token(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConnectorType!=NULL )
Commit_ConnectorType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCountry!=NULL )
Commit_Country(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteria!=NULL )
Commit_Criteria(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaList!=NULL )
Commit_CriteriaList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaList_CriteriaParmList!=NULL )
Commit_CriteriaList_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParm!=NULL )
Commit_CriteriaParm(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParmList!=NULL )
Commit_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParmNesting!=NULL )
Commit_CriteriaParmNesting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDHCPDevice!=NULL )
Commit_DHCPDevice(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDHCPDevice_DeviceData!=NULL )
Commit_DHCPDevice_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDataGrid!=NULL )
Commit_DataGrid(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObj!=NULL )
Commit_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjCategory!=NULL )
Commit_DesignObjCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjParameter!=NULL )
Commit_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjType!=NULL )
Commit_DesignObjType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjType_DesignObjParameter!=NULL )
Commit_DesignObjType_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation!=NULL )
Commit_DesignObjVariation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_DesignObj!=NULL )
Commit_DesignObjVariation_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_DesignObjParameter!=NULL )
Commit_DesignObjVariation_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Text!=NULL )
Commit_DesignObjVariation_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Text_Skin_Language!=NULL )
Commit_DesignObjVariation_Text_Skin_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Zone!=NULL )
Commit_DesignObjVariation_Zone(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice!=NULL )
Commit_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory!=NULL )
Commit_DeviceCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory_DeviceData!=NULL )
Commit_DeviceCategory_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory_Event!=NULL )
Commit_DeviceCategory_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup!=NULL )
Commit_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup_Command!=NULL )
Commit_DeviceCommandGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent!=NULL )
Commit_DeviceCommandGroup_DeviceCommandGroup_Parent(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceData!=NULL )
Commit_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceGroup!=NULL )
Commit_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate!=NULL )
Commit_DeviceTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_AV!=NULL )
Commit_DeviceTemplate_AV(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DSPMode!=NULL )
Commit_DeviceTemplate_DSPMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DesignObj!=NULL )
Commit_DeviceTemplate_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCategory_ControlledVia!=NULL )
Commit_DeviceTemplate_DeviceCategory_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe!=NULL )
Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCommandGroup!=NULL )
Commit_DeviceTemplate_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceData!=NULL )
Commit_DeviceTemplate_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_ControlledVia!=NULL )
Commit_DeviceTemplate_DeviceTemplate_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe!=NULL )
Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_Related!=NULL )
Commit_DeviceTemplate_DeviceTemplate_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Event!=NULL )
Commit_DeviceTemplate_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_InfraredGroup!=NULL )
Commit_DeviceTemplate_InfraredGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Input!=NULL )
Commit_DeviceTemplate_Input(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_MediaType!=NULL )
Commit_DeviceTemplate_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_MediaType_DesignObj!=NULL )
Commit_DeviceTemplate_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Output!=NULL )
Commit_DeviceTemplate_Output(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_PageSetup!=NULL )
Commit_DeviceTemplate_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Command!=NULL )
Commit_Device_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_CommandGroup!=NULL )
Commit_Device_CommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_DeviceData!=NULL )
Commit_Device_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_DeviceGroup!=NULL )
Commit_Device_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Device_Pipe!=NULL )
Commit_Device_Device_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Device_Related!=NULL )
Commit_Device_Device_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_EntertainArea!=NULL )
Commit_Device_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_HouseMode!=NULL )
Commit_Device_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_MRU!=NULL )
Commit_Device_MRU(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Orbiter!=NULL )
Commit_Device_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_QuickStart!=NULL )
Commit_Device_QuickStart(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_StartupScript!=NULL )
Commit_Device_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Users!=NULL )
Commit_Device_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDirection!=NULL )
Commit_Direction(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDirectory!=NULL )
Commit_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDistro!=NULL )
Commit_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDocument!=NULL )
Commit_Document(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDocument_Comment!=NULL )
Commit_Document_Comment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEntertainArea!=NULL )
Commit_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEvent!=NULL )
Commit_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventCategory!=NULL )
Commit_EventCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventHandler!=NULL )
Commit_EventHandler(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventParameter!=NULL )
Commit_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEvent_EventParameter!=NULL )
Commit_Event_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFAQ!=NULL )
Commit_FAQ(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFirewall!=NULL )
Commit_Firewall(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplan!=NULL )
Commit_Floorplan(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanObjectType!=NULL )
Commit_FloorplanObjectType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanObjectType_Color!=NULL )
Commit_FloorplanObjectType_Color(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanType!=NULL )
Commit_FloorplanType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHorizAlignment!=NULL )
Commit_HorizAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHouseMode!=NULL )
Commit_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHousehold!=NULL )
Commit_Household(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHousehold_Installation!=NULL )
Commit_Household_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblIcon!=NULL )
Commit_Icon(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblImage!=NULL )
Commit_Image(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup!=NULL )
Commit_InfraredGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup_Command!=NULL )
Commit_InfraredGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup_Command_Preferred!=NULL )
Commit_InfraredGroup_Command_Preferred(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallWizard!=NULL )
Commit_InstallWizard(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallWizard_Distro!=NULL )
Commit_InstallWizard_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation!=NULL )
Commit_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation_RepositorySource_URL!=NULL )
Commit_Installation_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation_Users!=NULL )
Commit_Installation_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLanguage!=NULL )
Commit_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLicense!=NULL )
Commit_License(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLicensing!=NULL )
Commit_Licensing(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblManufacturer!=NULL )
Commit_Manufacturer(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType!=NULL )
Commit_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType_Broadcast!=NULL )
Commit_MediaType_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType_DesignObj!=NULL )
Commit_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblNews!=NULL )
Commit_News(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOperatingSystem!=NULL )
Commit_OperatingSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter!=NULL )
Commit_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter_Users_PasswordReq!=NULL )
Commit_Orbiter_Users_PasswordReq(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter_Variable!=NULL )
Commit_Orbiter_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage!=NULL )
Commit_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackageType!=NULL )
Commit_PackageType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Compat!=NULL )
Commit_Package_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Device!=NULL )
Commit_Package_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Directory!=NULL )
Commit_Package_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Directory_File!=NULL )
Commit_Package_Directory_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Package!=NULL )
Commit_Package_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Source!=NULL )
Commit_Package_Source(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Source_Compat!=NULL )
Commit_Package_Source_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Users!=NULL )
Commit_Package_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Version!=NULL )
Commit_Package_Version(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPageSetup!=NULL )
Commit_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPaidLicense!=NULL )
Commit_PaidLicense(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblParameterType!=NULL )
Commit_ParameterType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPhoneLineType!=NULL )
Commit_PhoneLineType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPipe!=NULL )
Commit_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblQuickStartCategory!=NULL )
Commit_QuickStartCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblQuickStartTemplate!=NULL )
Commit_QuickStartTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRegion!=NULL )
Commit_Region(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRemoteControl!=NULL )
Commit_RemoteControl(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRemoteMapping!=NULL )
Commit_RemoteMapping(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositorySource!=NULL )
Commit_RepositorySource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositorySource_URL!=NULL )
Commit_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositoryType!=NULL )
Commit_RepositoryType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoom!=NULL )
Commit_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoomType!=NULL )
Commit_RoomType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoom_Users!=NULL )
Commit_Room_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSchema!=NULL )
Commit_Schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSetupStep!=NULL )
Commit_SetupStep(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSize!=NULL )
Commit_Size(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSkin!=NULL )
Commit_Skin(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStabilityStatus!=NULL )
Commit_StabilityStatus(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStartupScript!=NULL )
Commit_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStyle!=NULL )
Commit_Style(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStyleVariation!=NULL )
Commit_StyleVariation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSystem!=NULL )
Commit_System(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblTemplate!=NULL )
Commit_Template(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText!=NULL )
Commit_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblTextCategory!=NULL )
Commit_TextCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText_LS!=NULL )
Commit_Text_LS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText_LS_AltVersions!=NULL )
Commit_Text_LS_AltVersions(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUI!=NULL )
Commit_UI(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUnknownDevices!=NULL )
Commit_UnknownDevices(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUserMode!=NULL )
Commit_UserMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUsers!=NULL )
Commit_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVariable!=NULL )
Commit_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVersion!=NULL )
Commit_Version(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVertAlignment!=NULL )
Commit_VertAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_batdet!=NULL )
Commit_psc_constants_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_bathdr!=NULL )
Commit_psc_constants_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_batuser!=NULL )
Commit_psc_constants_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_repset!=NULL )
Commit_psc_constants_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_schema!=NULL )
Commit_psc_constants_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_tables!=NULL )
Commit_psc_constants_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_batdet!=NULL )
Commit_psc_dce_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_bathdr!=NULL )
Commit_psc_dce_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_batuser!=NULL )
Commit_psc_dce_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_repset!=NULL )
Commit_psc_dce_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_schema!=NULL )
Commit_psc_dce_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_tables!=NULL )
Commit_psc_dce_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_batdet!=NULL )
Commit_psc_designer_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_bathdr!=NULL )
Commit_psc_designer_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_batuser!=NULL )
Commit_psc_designer_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_repset!=NULL )
Commit_psc_designer_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_schema!=NULL )
Commit_psc_designer_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_tables!=NULL )
Commit_psc_designer_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_batdet!=NULL )
Commit_psc_document_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_bathdr!=NULL )
Commit_psc_document_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_batuser!=NULL )
Commit_psc_document_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_repset!=NULL )
Commit_psc_document_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_schema!=NULL )
Commit_psc_document_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_tables!=NULL )
Commit_psc_document_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_batdet!=NULL )
Commit_psc_ir_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_bathdr!=NULL )
Commit_psc_ir_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_batuser!=NULL )
Commit_psc_ir_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_repset!=NULL )
Commit_psc_ir_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_schema!=NULL )
Commit_psc_ir_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_tables!=NULL )
Commit_psc_ir_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_batdet!=NULL )
Commit_psc_local_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_bathdr!=NULL )
Commit_psc_local_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_batuser!=NULL )
Commit_psc_local_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_repset!=NULL )
Commit_psc_local_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_schema!=NULL )
Commit_psc_local_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_tables!=NULL )
Commit_psc_local_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_batdet!=NULL )
Commit_psc_website_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_bathdr!=NULL )
Commit_psc_website_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_batuser!=NULL )
Commit_psc_website_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_repset!=NULL )
Commit_psc_website_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_schema!=NULL )
Commit_psc_website_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_tables!=NULL )
Commit_psc_website_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

void Database_pluto_main::DeleteAllTables()
{
DeleteTable_Array();
DeleteTable_Broadcast();
DeleteTable_Button();
DeleteTable_CachedScreens();
DeleteTable_CannedEvents();
DeleteTable_CannedEvents_CriteriaParmList();
DeleteTable_City();
DeleteTable_CommMethod();
DeleteTable_Command();
DeleteTable_CommandCategory();
DeleteTable_CommandGroup();
DeleteTable_CommandGroup_Command();
DeleteTable_CommandGroup_Command_CommandParameter();
DeleteTable_CommandGroup_D();
DeleteTable_CommandGroup_D_Command();
DeleteTable_CommandGroup_D_Command_CommandParameter();
DeleteTable_CommandGroup_EntertainArea();
DeleteTable_CommandGroup_Room();
DeleteTable_CommandParameter();
DeleteTable_Command_CommandParameter();
DeleteTable_Command_Pipe();
DeleteTable_ConfigType();
DeleteTable_ConfigType_File();
DeleteTable_ConfigType_Setting();
DeleteTable_ConfigType_Token();
DeleteTable_ConnectorType();
DeleteTable_Country();
DeleteTable_Criteria();
DeleteTable_CriteriaList();
DeleteTable_CriteriaList_CriteriaParmList();
DeleteTable_CriteriaParm();
DeleteTable_CriteriaParmList();
DeleteTable_CriteriaParmNesting();
DeleteTable_DHCPDevice();
DeleteTable_DHCPDevice_DeviceData();
DeleteTable_DataGrid();
DeleteTable_DesignObj();
DeleteTable_DesignObjCategory();
DeleteTable_DesignObjParameter();
DeleteTable_DesignObjType();
DeleteTable_DesignObjType_DesignObjParameter();
DeleteTable_DesignObjVariation();
DeleteTable_DesignObjVariation_DesignObj();
DeleteTable_DesignObjVariation_DesignObjParameter();
DeleteTable_DesignObjVariation_Text();
DeleteTable_DesignObjVariation_Text_Skin_Language();
DeleteTable_DesignObjVariation_Zone();
DeleteTable_Device();
DeleteTable_DeviceCategory();
DeleteTable_DeviceCategory_DeviceData();
DeleteTable_DeviceCategory_Event();
DeleteTable_DeviceCommandGroup();
DeleteTable_DeviceCommandGroup_Command();
DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
DeleteTable_DeviceData();
DeleteTable_DeviceGroup();
DeleteTable_DeviceTemplate();
DeleteTable_DeviceTemplate_AV();
DeleteTable_DeviceTemplate_DSPMode();
DeleteTable_DeviceTemplate_DesignObj();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceCommandGroup();
DeleteTable_DeviceTemplate_DeviceData();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceTemplate_Related();
DeleteTable_DeviceTemplate_Event();
DeleteTable_DeviceTemplate_InfraredGroup();
DeleteTable_DeviceTemplate_Input();
DeleteTable_DeviceTemplate_MediaType();
DeleteTable_DeviceTemplate_MediaType_DesignObj();
DeleteTable_DeviceTemplate_Output();
DeleteTable_DeviceTemplate_PageSetup();
DeleteTable_Device_Command();
DeleteTable_Device_CommandGroup();
DeleteTable_Device_DeviceData();
DeleteTable_Device_DeviceGroup();
DeleteTable_Device_Device_Pipe();
DeleteTable_Device_Device_Related();
DeleteTable_Device_EntertainArea();
DeleteTable_Device_HouseMode();
DeleteTable_Device_MRU();
DeleteTable_Device_Orbiter();
DeleteTable_Device_QuickStart();
DeleteTable_Device_StartupScript();
DeleteTable_Device_Users();
DeleteTable_Direction();
DeleteTable_Directory();
DeleteTable_Distro();
DeleteTable_Document();
DeleteTable_Document_Comment();
DeleteTable_EntertainArea();
DeleteTable_Event();
DeleteTable_EventCategory();
DeleteTable_EventHandler();
DeleteTable_EventParameter();
DeleteTable_Event_EventParameter();
DeleteTable_FAQ();
DeleteTable_Firewall();
DeleteTable_Floorplan();
DeleteTable_FloorplanObjectType();
DeleteTable_FloorplanObjectType_Color();
DeleteTable_FloorplanType();
DeleteTable_HorizAlignment();
DeleteTable_HouseMode();
DeleteTable_Household();
DeleteTable_Household_Installation();
DeleteTable_Icon();
DeleteTable_Image();
DeleteTable_InfraredGroup();
DeleteTable_InfraredGroup_Command();
DeleteTable_InfraredGroup_Command_Preferred();
DeleteTable_InstallWizard();
DeleteTable_InstallWizard_Distro();
DeleteTable_Installation();
DeleteTable_Installation_RepositorySource_URL();
DeleteTable_Installation_Users();
DeleteTable_Language();
DeleteTable_License();
DeleteTable_Licensing();
DeleteTable_Manufacturer();
DeleteTable_MediaType();
DeleteTable_MediaType_Broadcast();
DeleteTable_MediaType_DesignObj();
DeleteTable_News();
DeleteTable_OperatingSystem();
DeleteTable_Orbiter();
DeleteTable_Orbiter_Users_PasswordReq();
DeleteTable_Orbiter_Variable();
DeleteTable_Package();
DeleteTable_PackageType();
DeleteTable_Package_Compat();
DeleteTable_Package_Device();
DeleteTable_Package_Directory();
DeleteTable_Package_Directory_File();
DeleteTable_Package_Package();
DeleteTable_Package_Source();
DeleteTable_Package_Source_Compat();
DeleteTable_Package_Users();
DeleteTable_Package_Version();
DeleteTable_PageSetup();
DeleteTable_PaidLicense();
DeleteTable_ParameterType();
DeleteTable_PhoneLineType();
DeleteTable_Pipe();
DeleteTable_QuickStartCategory();
DeleteTable_QuickStartTemplate();
DeleteTable_Region();
DeleteTable_RemoteControl();
DeleteTable_RemoteMapping();
DeleteTable_RepositorySource();
DeleteTable_RepositorySource_URL();
DeleteTable_RepositoryType();
DeleteTable_Room();
DeleteTable_RoomType();
DeleteTable_Room_Users();
DeleteTable_Schema();
DeleteTable_SetupStep();
DeleteTable_Size();
DeleteTable_Skin();
DeleteTable_StabilityStatus();
DeleteTable_StartupScript();
DeleteTable_Style();
DeleteTable_StyleVariation();
DeleteTable_System();
DeleteTable_Template();
DeleteTable_Text();
DeleteTable_TextCategory();
DeleteTable_Text_LS();
DeleteTable_Text_LS_AltVersions();
DeleteTable_UI();
DeleteTable_UnknownDevices();
DeleteTable_UserMode();
DeleteTable_Users();
DeleteTable_Variable();
DeleteTable_Version();
DeleteTable_VertAlignment();
DeleteTable_psc_constants_batdet();
DeleteTable_psc_constants_bathdr();
DeleteTable_psc_constants_batuser();
DeleteTable_psc_constants_repset();
DeleteTable_psc_constants_schema();
DeleteTable_psc_constants_tables();
DeleteTable_psc_dce_batdet();
DeleteTable_psc_dce_bathdr();
DeleteTable_psc_dce_batuser();
DeleteTable_psc_dce_repset();
DeleteTable_psc_dce_schema();
DeleteTable_psc_dce_tables();
DeleteTable_psc_designer_batdet();
DeleteTable_psc_designer_bathdr();
DeleteTable_psc_designer_batuser();
DeleteTable_psc_designer_repset();
DeleteTable_psc_designer_schema();
DeleteTable_psc_designer_tables();
DeleteTable_psc_document_batdet();
DeleteTable_psc_document_bathdr();
DeleteTable_psc_document_batuser();
DeleteTable_psc_document_repset();
DeleteTable_psc_document_schema();
DeleteTable_psc_document_tables();
DeleteTable_psc_ir_batdet();
DeleteTable_psc_ir_bathdr();
DeleteTable_psc_ir_batuser();
DeleteTable_psc_ir_repset();
DeleteTable_psc_ir_schema();
DeleteTable_psc_ir_tables();
DeleteTable_psc_local_batdet();
DeleteTable_psc_local_bathdr();
DeleteTable_psc_local_batuser();
DeleteTable_psc_local_repset();
DeleteTable_psc_local_schema();
DeleteTable_psc_local_tables();
DeleteTable_psc_website_batdet();
DeleteTable_psc_website_bathdr();
DeleteTable_psc_website_batuser();
DeleteTable_psc_website_repset();
DeleteTable_psc_website_schema();
DeleteTable_psc_website_tables();
}

bool Database_pluto_main::Connect(string host, string user, string pass, string sDBName, int port)
{
return MySQLConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_main::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
