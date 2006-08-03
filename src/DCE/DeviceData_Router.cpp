/* 
	DeviceData_Router
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

/**
 *
 * @file DeviceData_Router.cpp
 * @brief source file for the AllDevices class
 * @author
 * @todo notcommented
 *
 */


#include "Logger.h"
#include "Command_Impl.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "DeviceData_Router.h"

using namespace DCE;

// This will return the first device within the given category that is in any way
// related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
DeviceData_Base *DeviceData_Router::FindFirstRelatedDeviceOfCategory(int PK_DeviceCategory,Command_Impl *pCommand_Impl_Confirm_Registration,int TimeoutToWait,bool bScanParent,int PK_Device_ExcludeChild)
{
	if( WithinCategory(PK_DeviceCategory) )
	{
		if( pCommand_Impl_Confirm_Registration )
		{
			time_t tTimeout = time(NULL) + TimeoutToWait;
			char Status = 'N';
			while (Status != 'Y')
			{
				Status = pCommand_Impl_Confirm_Registration->DeviceIsRegistered(m_dwPK_Device);
				switch (Status)
				{
					case 'Y': // is treated in the while loop
						return this;
					case 'N': // not registered
						if( tTimeout < time(NULL) )
						{
							if( g_pPlutoLogger )
								g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d never registered",m_dwPK_Device);
							return NULL;
						}
						Sleep(1000);
						break;
					case 'D':
						if( g_pPlutoLogger )
							g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d is disabled",m_dwPK_Device);
						return NULL;
					case 'E':
						if( g_pPlutoLogger )
							g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d comm error",m_dwPK_Device);
						return NULL;
				}
			}
		}
		return this;
	}
	for(size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s)
	{
		DeviceData_Base *pDeviceData_Base = m_vectDeviceData_Impl_Children[s];
		if( pDeviceData_Base->m_dwPK_Device==PK_Device_ExcludeChild )
			continue;
		DeviceData_Base *pDeviceData_Base_Result = pDeviceData_Base->FindFirstRelatedDeviceOfCategory(PK_DeviceCategory,pCommand_Impl_Confirm_Registration,TimeoutToWait,false);
		if( pDeviceData_Base_Result )
			return pDeviceData_Base_Result;
	}
	if( bScanParent && m_pDevice_ControlledVia )
		return m_pDevice_ControlledVia->FindFirstRelatedDeviceOfCategory(PK_DeviceCategory,pCommand_Impl_Confirm_Registration,TimeoutToWait,true,m_dwPK_Device);
	return NULL;
}

// This will return the first device of the given template that is in any way
// related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
DeviceData_Base *DeviceData_Router::FindFirstRelatedDeviceOfTemplate(int PK_DeviceTemplate,Command_Impl *pCommand_Impl_Confirm_Registration,int TimeoutToWait,bool bScanParent, int PK_Device_ExcludeChild)
{
	if( m_dwPK_DeviceTemplate==PK_DeviceTemplate )
	{
		if( pCommand_Impl_Confirm_Registration )
		{
			time_t tTimeout = time(NULL) + TimeoutToWait;
			char Status = 'N';
			while (Status != 'Y')
			{
				Status = pCommand_Impl_Confirm_Registration->DeviceIsRegistered(m_dwPK_Device);
				switch (Status)
				{
					case 'Y': // is treated in the while loop
						return this;
					case 'N': // not registered
						if( tTimeout < time(NULL) )
						{
							if( g_pPlutoLogger )
								g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d never registered",m_dwPK_Device);
							return NULL;
						}
						Sleep(1000);
						break;
					case 'D':
						if( g_pPlutoLogger )
							g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d is disabled",m_dwPK_Device);
						return NULL;
					case 'E':
						if( g_pPlutoLogger )
							g_pPlutoLogger->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d comm error",m_dwPK_Device);
						return NULL;
				}
			}
		}

		return this;
	}
	for(size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s)
	{
		DeviceData_Base *pDeviceData_Base = m_vectDeviceData_Impl_Children[s];
		if( pDeviceData_Base->m_dwPK_Device==PK_Device_ExcludeChild )
			continue;
		DeviceData_Base *pDeviceData_Base_Result = pDeviceData_Base->FindFirstRelatedDeviceOfTemplate(PK_DeviceTemplate,pCommand_Impl_Confirm_Registration,TimeoutToWait,false);
		if( pDeviceData_Base_Result )
			return pDeviceData_Base_Result;
	}
	if( bScanParent && m_pDevice_ControlledVia )
		return m_pDevice_ControlledVia->FindFirstRelatedDeviceOfTemplate(PK_DeviceTemplate,pCommand_Impl_Confirm_Registration,TimeoutToWait,true,m_dwPK_Device);
	return NULL;
}
