/*
 MediaHandlerInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "EntertainArea.h"
#include "MediaPosition.h"
#include "Media_Plugin.h"

#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

MediaHandlerInfo::MediaHandlerInfo( class MediaHandlerBase *pMediaHandlerBase, class Command_Impl *pCommand_Impl, int PK_MediaType, int PK_DeviceTemplate, bool bCanJumpPosition, bool bUsesDevices, bool bDontRegister )
{
    m_iPK_DesignObj=0;
    m_pMediaHandlerBase=pMediaHandlerBase;
    m_pCommand_Impl=pCommand_Impl;
    m_PK_MediaType=PK_MediaType;
    m_bCanJumpPosition=bCanJumpPosition;
    m_bUsesDevices=bUsesDevices;
    m_bUsesRemovableMedia=true;

    if( PK_DeviceTemplate )
    {
        ListDeviceData_Router *pListDeviceData_Router=m_pMediaHandlerBase->m_pMedia_Plugin->m_pRouter->m_mapDeviceByTemplate_Find( PK_DeviceTemplate );
		if( !pListDeviceData_Router )
		{
			g_pPlutoLogger->Write(LV_STATUS,"Media Handler Info cannot find any devices for template: %d",PK_DeviceTemplate);
			return;
		}
        for( ListDeviceData_Router::iterator it=pListDeviceData_Router->begin( );it!=pListDeviceData_Router->end( );++it )
        {
            DeviceData_Router *pDeviceData_Router = *it;
            MediaDevice *pMediaDevice = m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaDevice_Find( pDeviceData_Router->m_dwPK_Device );
            if( !pMediaDevice )
                g_pPlutoLogger->Write( LV_CRITICAL, "Device %d (%s) isn't in an entertainment area", pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_sDescription.c_str() );
            else
                m_listMediaDevice.push_back( pMediaDevice );
        }
    }

    Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType=NULL;
    if( PK_DeviceTemplate )
    {
        vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
        pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_main->DeviceTemplate_MediaType_get( )->GetRows(
                DEVICETEMPLATE_MEDIATYPE_FK_DEVICETEMPLATE_FIELD + string( "=" ) + StringUtils::itos( PK_DeviceTemplate ) + " AND " +
                DEVICETEMPLATE_MEDIATYPE_FK_MEDIATYPE_FIELD + "=" + StringUtils::itos( PK_MediaType ), &vectRow_DeviceTemplate_MediaType );

        if( vectRow_DeviceTemplate_MediaType.size( ) ) // there should only be 1 anyway
            pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[0];
    }

    // See if we have information specific to this media type on this device. Otherwise we'll use the defaults for the media type
    if( pRow_DeviceTemplate_MediaType )
    {
        string::size_type pos=0;
        while( true )
        {
            string Extension_list = pRow_DeviceTemplate_MediaType->Extensions_get( );
            string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
            if( !Extension.size( ) )
                break;
            m_listExtensions.push_back( Extension );
        }
    }
    else
    {
        Row_MediaType *pRow_MediaType=m_pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_main->MediaType_get( )->GetRow( m_PK_MediaType );
        if( pRow_MediaType )
        {
    //todo      m_bUsesRemovableMedia = pRow_MediaType->UsesRemoveableMedia_get( )==1;
    //todo      m_bCanStoreOnServer = pRow_MediaType->CanStoreOnServer_get( )==1;
    // todo     m_bCanBroadcastInHouse = pRow_MediaType->CanBroadcastInHouse_get( )==1;
    //todo hack     m_bIsExternalTransmission = pRow_MediaType->IsExternalTransmission_get( )==1;
            string::size_type pos=0;
            while( true )
            {
                string Extension_list = pRow_MediaType->Extensions_get( );
                string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
                if( !Extension.size( ) )
                    break;
                m_listExtensions.push_back( Extension );
            }
        }
    }

    // Now find all the entertainment areas served by those devices, and put ourselves as a media handler there
    for( list<class MediaDevice *>::iterator itListMD=m_listMediaDevice.begin( );itListMD!=m_listMediaDevice.end( );++itListMD )
    {
        MediaDevice *pMediaDevice = *itListMD;
        for( list<class EntertainArea *>::iterator itEntArea=pMediaDevice->m_listEntertainArea.begin( );itEntArea!=pMediaDevice->m_listEntertainArea.end( );++itEntArea )
        {
            EntertainArea *pEntertainArea = *itEntArea;
            List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find( m_PK_MediaType );
            if( !pList_MediaHandlerInfo )
            {
                pList_MediaHandlerInfo = new List_MediaHandlerInfo( );
                pEntertainArea->m_mapMediaHandlerInfo_MediaType[m_PK_MediaType] = pList_MediaHandlerInfo;
            }
            pList_MediaHandlerInfo->push_back( this );

            for( list<string>::iterator itExt=m_listExtensions.begin( );itExt!=m_listExtensions.end( );++itExt )
            {
                string Extension = *itExt;

                pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find( Extension );
                if( !pList_MediaHandlerInfo )
                {
                    pList_MediaHandlerInfo = new List_MediaHandlerInfo( );
                    pEntertainArea->m_mapMediaHandlerInfo_Extension[Extension] = pList_MediaHandlerInfo;
                }
                pList_MediaHandlerInfo->push_back( this );
            }
        }
    }
// todo m_pMediaHandlerBase->m_pMedia_Plugin->RegisterMediaPlugin( this );
}
