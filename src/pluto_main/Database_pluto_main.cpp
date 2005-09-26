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
tblArray->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblBroadcast!=NULL )
tblBroadcast->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblButton!=NULL )
tblButton->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCachedScreens!=NULL )
tblCachedScreens->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCannedEvents!=NULL )
tblCannedEvents->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCannedEvents_CriteriaParmList!=NULL )
tblCannedEvents_CriteriaParmList->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCity!=NULL )
tblCity->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommMethod!=NULL )
tblCommMethod->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand!=NULL )
tblCommand->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandCategory!=NULL )
tblCommandCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup!=NULL )
tblCommandGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Command!=NULL )
tblCommandGroup_Command->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Command_CommandParameter!=NULL )
tblCommandGroup_Command_CommandParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D!=NULL )
tblCommandGroup_D->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D_Command!=NULL )
tblCommandGroup_D_Command->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_D_Command_CommandParameter!=NULL )
tblCommandGroup_D_Command_CommandParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_EntertainArea!=NULL )
tblCommandGroup_EntertainArea->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandGroup_Room!=NULL )
tblCommandGroup_Room->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommandParameter!=NULL )
tblCommandParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand_CommandParameter!=NULL )
tblCommand_CommandParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCommand_Pipe!=NULL )
tblCommand_Pipe->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType!=NULL )
tblConfigType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_File!=NULL )
tblConfigType_File->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_Setting!=NULL )
tblConfigType_Setting->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConfigType_Token!=NULL )
tblConfigType_Token->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblConnectorType!=NULL )
tblConnectorType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCountry!=NULL )
tblCountry->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteria!=NULL )
tblCriteria->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaList!=NULL )
tblCriteriaList->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaList_CriteriaParmList!=NULL )
tblCriteriaList_CriteriaParmList->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParm!=NULL )
tblCriteriaParm->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParmList!=NULL )
tblCriteriaParmList->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblCriteriaParmNesting!=NULL )
tblCriteriaParmNesting->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDHCPDevice!=NULL )
tblDHCPDevice->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDHCPDevice_DeviceData!=NULL )
tblDHCPDevice_DeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDataGrid!=NULL )
tblDataGrid->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObj!=NULL )
tblDesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjCategory!=NULL )
tblDesignObjCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjParameter!=NULL )
tblDesignObjParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjType!=NULL )
tblDesignObjType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjType_DesignObjParameter!=NULL )
tblDesignObjType_DesignObjParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation!=NULL )
tblDesignObjVariation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_DesignObj!=NULL )
tblDesignObjVariation_DesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_DesignObjParameter!=NULL )
tblDesignObjVariation_DesignObjParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Text!=NULL )
tblDesignObjVariation_Text->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Text_Skin_Language!=NULL )
tblDesignObjVariation_Text_Skin_Language->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDesignObjVariation_Zone!=NULL )
tblDesignObjVariation_Zone->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice!=NULL )
tblDevice->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory!=NULL )
tblDeviceCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory_DeviceData!=NULL )
tblDeviceCategory_DeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCategory_Event!=NULL )
tblDeviceCategory_Event->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup!=NULL )
tblDeviceCommandGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup_Command!=NULL )
tblDeviceCommandGroup_Command->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent!=NULL )
tblDeviceCommandGroup_DeviceCommandGroup_Parent->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceData!=NULL )
tblDeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceGroup!=NULL )
tblDeviceGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate!=NULL )
tblDeviceTemplate->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_AV!=NULL )
tblDeviceTemplate_AV->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DSPMode!=NULL )
tblDeviceTemplate_DSPMode->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DesignObj!=NULL )
tblDeviceTemplate_DesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCategory_ControlledVia!=NULL )
tblDeviceTemplate_DeviceCategory_ControlledVia->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe!=NULL )
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceCommandGroup!=NULL )
tblDeviceTemplate_DeviceCommandGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceData!=NULL )
tblDeviceTemplate_DeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_ControlledVia!=NULL )
tblDeviceTemplate_DeviceTemplate_ControlledVia->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe!=NULL )
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_DeviceTemplate_Related!=NULL )
tblDeviceTemplate_DeviceTemplate_Related->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Event!=NULL )
tblDeviceTemplate_Event->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_InfraredGroup!=NULL )
tblDeviceTemplate_InfraredGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Input!=NULL )
tblDeviceTemplate_Input->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_MediaType!=NULL )
tblDeviceTemplate_MediaType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_MediaType_DesignObj!=NULL )
tblDeviceTemplate_MediaType_DesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_Output!=NULL )
tblDeviceTemplate_Output->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDeviceTemplate_PageSetup!=NULL )
tblDeviceTemplate_PageSetup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Command!=NULL )
tblDevice_Command->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_CommandGroup!=NULL )
tblDevice_CommandGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_DeviceData!=NULL )
tblDevice_DeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_DeviceGroup!=NULL )
tblDevice_DeviceGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Device_Pipe!=NULL )
tblDevice_Device_Pipe->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Device_Related!=NULL )
tblDevice_Device_Related->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_EntertainArea!=NULL )
tblDevice_EntertainArea->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_HouseMode!=NULL )
tblDevice_HouseMode->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_MRU!=NULL )
tblDevice_MRU->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Orbiter!=NULL )
tblDevice_Orbiter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_QuickStart!=NULL )
tblDevice_QuickStart->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_StartupScript!=NULL )
tblDevice_StartupScript->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDevice_Users!=NULL )
tblDevice_Users->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDirection!=NULL )
tblDirection->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDirectory!=NULL )
tblDirectory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDistro!=NULL )
tblDistro->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDocument!=NULL )
tblDocument->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblDocument_Comment!=NULL )
tblDocument_Comment->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEntertainArea!=NULL )
tblEntertainArea->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEvent!=NULL )
tblEvent->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventCategory!=NULL )
tblEventCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventHandler!=NULL )
tblEventHandler->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEventParameter!=NULL )
tblEventParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblEvent_EventParameter!=NULL )
tblEvent_EventParameter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFAQ!=NULL )
tblFAQ->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFirewall!=NULL )
tblFirewall->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplan!=NULL )
tblFloorplan->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanObjectType!=NULL )
tblFloorplanObjectType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanObjectType_Color!=NULL )
tblFloorplanObjectType_Color->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblFloorplanType!=NULL )
tblFloorplanType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHorizAlignment!=NULL )
tblHorizAlignment->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHouseMode!=NULL )
tblHouseMode->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHousehold!=NULL )
tblHousehold->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblHousehold_Installation!=NULL )
tblHousehold_Installation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblIcon!=NULL )
tblIcon->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblImage!=NULL )
tblImage->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup!=NULL )
tblInfraredGroup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup_Command!=NULL )
tblInfraredGroup_Command->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInfraredGroup_Command_Preferred!=NULL )
tblInfraredGroup_Command_Preferred->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallWizard!=NULL )
tblInstallWizard->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallWizard_Distro!=NULL )
tblInstallWizard_Distro->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation!=NULL )
tblInstallation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation_RepositorySource_URL!=NULL )
tblInstallation_RepositorySource_URL->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblInstallation_Users!=NULL )
tblInstallation_Users->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLanguage!=NULL )
tblLanguage->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLicense!=NULL )
tblLicense->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblLicensing!=NULL )
tblLicensing->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblManufacturer!=NULL )
tblManufacturer->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType!=NULL )
tblMediaType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType_Broadcast!=NULL )
tblMediaType_Broadcast->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblMediaType_DesignObj!=NULL )
tblMediaType_DesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblNews!=NULL )
tblNews->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOperatingSystem!=NULL )
tblOperatingSystem->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter!=NULL )
tblOrbiter->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter_Users_PasswordReq!=NULL )
tblOrbiter_Users_PasswordReq->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblOrbiter_Variable!=NULL )
tblOrbiter_Variable->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage!=NULL )
tblPackage->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackageType!=NULL )
tblPackageType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Compat!=NULL )
tblPackage_Compat->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Device!=NULL )
tblPackage_Device->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Directory!=NULL )
tblPackage_Directory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Directory_File!=NULL )
tblPackage_Directory_File->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Package!=NULL )
tblPackage_Package->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Source!=NULL )
tblPackage_Source->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Source_Compat!=NULL )
tblPackage_Source_Compat->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Users!=NULL )
tblPackage_Users->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPackage_Version!=NULL )
tblPackage_Version->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPageSetup!=NULL )
tblPageSetup->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPaidLicense!=NULL )
tblPaidLicense->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblParameterType!=NULL )
tblParameterType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPhoneLineType!=NULL )
tblPhoneLineType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblPipe!=NULL )
tblPipe->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblQuickStartCategory!=NULL )
tblQuickStartCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblQuickStartTemplate!=NULL )
tblQuickStartTemplate->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRegion!=NULL )
tblRegion->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRemoteControl!=NULL )
tblRemoteControl->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRemoteMapping!=NULL )
tblRemoteMapping->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositorySource!=NULL )
tblRepositorySource->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositorySource_URL!=NULL )
tblRepositorySource_URL->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRepositoryType!=NULL )
tblRepositoryType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoom!=NULL )
tblRoom->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoomType!=NULL )
tblRoomType->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblRoom_Users!=NULL )
tblRoom_Users->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSchema!=NULL )
tblSchema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSetupStep!=NULL )
tblSetupStep->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSize!=NULL )
tblSize->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSkin!=NULL )
tblSkin->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStabilityStatus!=NULL )
tblStabilityStatus->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStartupScript!=NULL )
tblStartupScript->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStyle!=NULL )
tblStyle->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblStyleVariation!=NULL )
tblStyleVariation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblSystem!=NULL )
tblSystem->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblTemplate!=NULL )
tblTemplate->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText!=NULL )
tblText->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblTextCategory!=NULL )
tblTextCategory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText_LS!=NULL )
tblText_LS->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblText_LS_AltVersions!=NULL )
tblText_LS_AltVersions->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUI!=NULL )
tblUI->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUnknownDevices!=NULL )
tblUnknownDevices->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUserMode!=NULL )
tblUserMode->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblUsers!=NULL )
tblUsers->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVariable!=NULL )
tblVariable->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVersion!=NULL )
tblVersion->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblVertAlignment!=NULL )
tblVertAlignment->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_batdet!=NULL )
tblpsc_constants_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_bathdr!=NULL )
tblpsc_constants_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_batuser!=NULL )
tblpsc_constants_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_repset!=NULL )
tblpsc_constants_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_schema!=NULL )
tblpsc_constants_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_constants_tables!=NULL )
tblpsc_constants_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_batdet!=NULL )
tblpsc_dce_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_bathdr!=NULL )
tblpsc_dce_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_batuser!=NULL )
tblpsc_dce_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_repset!=NULL )
tblpsc_dce_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_schema!=NULL )
tblpsc_dce_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_dce_tables!=NULL )
tblpsc_dce_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_batdet!=NULL )
tblpsc_designer_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_bathdr!=NULL )
tblpsc_designer_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_batuser!=NULL )
tblpsc_designer_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_repset!=NULL )
tblpsc_designer_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_schema!=NULL )
tblpsc_designer_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_designer_tables!=NULL )
tblpsc_designer_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_batdet!=NULL )
tblpsc_document_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_bathdr!=NULL )
tblpsc_document_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_batuser!=NULL )
tblpsc_document_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_repset!=NULL )
tblpsc_document_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_schema!=NULL )
tblpsc_document_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_document_tables!=NULL )
tblpsc_document_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_batdet!=NULL )
tblpsc_ir_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_bathdr!=NULL )
tblpsc_ir_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_batuser!=NULL )
tblpsc_ir_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_repset!=NULL )
tblpsc_ir_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_schema!=NULL )
tblpsc_ir_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_ir_tables!=NULL )
tblpsc_ir_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_batdet!=NULL )
tblpsc_local_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_bathdr!=NULL )
tblpsc_local_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_batuser!=NULL )
tblpsc_local_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_repset!=NULL )
tblpsc_local_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_schema!=NULL )
tblpsc_local_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_local_tables!=NULL )
tblpsc_local_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_batdet!=NULL )
tblpsc_website_batdet->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_bathdr!=NULL )
tblpsc_website_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_batuser!=NULL )
tblpsc_website_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_repset!=NULL )
tblpsc_website_repset->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_schema!=NULL )
tblpsc_website_schema->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
if( tblpsc_website_tables!=NULL )
tblpsc_website_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
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
