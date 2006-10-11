/*
    DatabaseUtils

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
 * @file DatabaseUtils.cpp
 * @brief source file for the DatabaseUtils
 * @author
 * @todo notcommented
 *
 */
#include "StringUtils.h"
#include "DatabaseUtils.h"
#include "Other.h"
#include "MySQLHelper.h"
#include "pluto_main/Define_DeviceData.h"

string DatabaseUtils::HumanReadablePort(MySqlHelper *pMySqlHelper,int PK_Device,string sPort)
{
	string sPortMapping = GetDeviceData(pMySqlHelper,PK_Device,DEVICEDATA_Serial_Port_Names_CONST);
	string::size_type pos=0;
	while(pos<sPortMapping.size())
	{
		string sToken = StringUtils::Tokenize(sPortMapping,",",pos);
		string::size_type posEqual = sToken.find('=');
		if( posEqual==string::npos )
			continue;
		if( sToken.substr(0,posEqual)==sPort )
			return sToken.substr(posEqual+1);
	}

	if(sPort == "/dev/ttyS0")
		sPort = "COM1";
	if(sPort == "/dev/ttyS1")
		sPort = "COM2";
	if(sPort == "/dev/ttyS2")
		sPort = "COM3";
	if(sPort == "/dev/ttyS3")
		sPort = "COM4";

	return sPort;
}

int DatabaseUtils::GetTopMostDevice(MySqlHelper *pMySqlHelper,int PK_Device)
{
	string sSQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] && atoi(row[0]) )
		return GetTopMostDevice(pMySqlHelper,atoi(row[0]));
	else
		return PK_Device;
}

int DatabaseUtils::GetNumberOfChildDevices(MySqlHelper *pMySqlHelper,int PK_Device)
{
	string sSQL = "SELECT count(PK_Device) FROM Device WHERE FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
		return atoi(row[0]);
	else
		return 0;
}

int DatabaseUtils::GetDeviceTemplateForDevice(MySqlHelper *pMySqlHelper,int PK_Device)
{
	string sSQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] && atoi(row[0]) )
		return atoi(row[0]);
	else
		return 0;
}

void DatabaseUtils::SetDeviceTemplateForDevice(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceTemplate)
{
	string sSQL = "UPDATE Device SET FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
	pMySqlHelper->threaded_mysql_query(sSQL);	
}

void DatabaseUtils::GetUnusedPortsOnAllPCs(MySqlHelper *pMySqlHelper,vector< pair<int,string> > &vectAllPorts, 
										   long nFK_Installation/*=0*/)
{
	string sSQL = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia IS NULL";
	if(nFK_Installation)
		sSQL += " AND FK_Installation = " + StringUtils::ltos(nFK_Installation);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) )
	{
		while ( row=mysql_fetch_row( result.r ) )
		{
			int PK_Device = atoi(row[0]);
			vector<string> vectPorts;
			GetUnusedPortsOnPC(pMySqlHelper,PK_Device,vectPorts);
			for(size_t s=0;s<vectPorts.size();++s)
				vectAllPorts.push_back( make_pair<int,string> (PK_Device,vectPorts[s]));
		}
	}
}

void DatabaseUtils::GetUnusedPortsOnPC(MySqlHelper *pMySqlHelper,int PK_Device,vector<string> &vectPorts)
{
	map<string,bool> mapPorts;
	map<int, pair<int,int> > mapDeviceTree;
	GetAllDevicesInTree(pMySqlHelper,PK_Device,mapDeviceTree);
	string sDeviceList;
	for(map<int, pair<int,int> >::iterator it=mapDeviceTree.begin();it!=mapDeviceTree.end();++it)
		sDeviceList += (it!=mapDeviceTree.begin() ? "," : "") + StringUtils::itos(it->first);

	// First get all the available ports
	{
		string sSQL = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Available_Serial_Ports_CONST)
			+ " AND FK_Device IN (" + sDeviceList + ")";

		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) )
		{
			while ( row=mysql_fetch_row( result.r ) )
			{
				string sPorts = row[0];
				string::size_type pos=0;
				while(pos<sPorts.size())
					mapPorts[StringUtils::Tokenize(sPorts,",",pos)]=false;
			}
		}
	}

	// Now which ones are in use
	{
		string sSQL = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=" + StringUtils::itos(DEVICEDATA_COM_Port_on_PC_CONST)
			+ " AND FK_Device IN (" + sDeviceList + ")";

		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) )
		{
			while ( row=mysql_fetch_row( result.r ) )
			{
				string sPort = row[0];
				if( mapPorts.find(sPort)!=mapPorts.end() )
					mapPorts[sPort]=true;
			}
		}
	}

	// Now put the unused ones in a vect so we can scan them
	for(map<string,bool>::iterator it=mapPorts.begin();it!=mapPorts.end();++it)
		if( !it->second )
			vectPorts.push_back(it->first);
}

void DatabaseUtils::SetDeviceData(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceData,string IK_DeviceData)
{
	string sSQL = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + StringUtils::itos(PK_Device) 
		+ " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);

	PlutoSqlResult result;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && result.r->row_count>0 )
		sSQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + StringUtils::SQLEscape(IK_DeviceData) + "' WHERE "
			"FK_Device=" + StringUtils::itos(PK_Device) +  " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);
	else
		sSQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES("
			+ StringUtils::itos(PK_Device) + "," + StringUtils::itos(PK_DeviceData) + ",'"
			+ StringUtils::SQLEscape(IK_DeviceData) + "');";

	pMySqlHelper->threaded_mysql_query(sSQL);	
}

string DatabaseUtils::GetDeviceData(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceData)
{
	string sSQL = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + StringUtils::itos(PK_Device) 
		+ " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
		return row[0];
	else
		return "";
}

void DatabaseUtils::GetAllDevicesInTree(MySqlHelper *pMySqlHelper,int PK_Device,map<int,pair<int,int> > &mapDeviceTree,bool bCheckParent,int PK_Device_ChildExclude,int Generation)
{
	// pair<int,int> is a pair of PK_Device to generation, where 0 is the starting generation
	{
		string sSQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
			mapDeviceTree[PK_Device]=make_pair<int,int> (atoi(row[0]),Generation);
	}
	if( bCheckParent )
	{
		string sSQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
			GetAllDevicesInTree(pMySqlHelper,atoi(row[0]),mapDeviceTree,true,PK_Device,Generation+1);
	}

	string sSQL = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )!=NULL )
	{
		while(  (row=mysql_fetch_row(result.r)) )
		{
			if( row[0] )
			{
				int PK_Device_Child = atoi(row[0]);
				if( PK_Device_Child && PK_Device_Child!=PK_Device_ChildExclude )
					GetAllDevicesInTree(pMySqlHelper,PK_Device_Child,mapDeviceTree,false,0,Generation+1);
			}
		}
	}
}

string DatabaseUtils::GetDescriptionForDevice(MySqlHelper *pMySqlHelper,int PK_Device)
{
	string sSQL = "SELECT Description FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )!=NULL && (row=mysql_fetch_row(result.r))!=NULL && row[0] )
		return row[0];
	return "";
}

string DatabaseUtils::GetDescriptionFromTable(MySqlHelper *pMySqlHelper,string sTable,int PK)
{
	string sSQL = "SELECT Description FROM " + sTable + " WHERE PK_" + sTable + "=" + StringUtils::itos(PK);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )!=NULL && (row=mysql_fetch_row(result.r))!=NULL && row[0] )
		return row[0];
	return "";
}

void DatabaseUtils::SetDescriptionForDevice(MySqlHelper *pMySqlHelper,int PK_Device,string sDescription)
{
	string sSQL = "UPDATE Device set Description='" + StringUtils::SQLEscape(sDescription) + "' WHERE PK_Device=" + StringUtils::itos(PK_Device);
	pMySqlHelper->threaded_mysql_query(sSQL);	
}

bool DatabaseUtils::SetDeviceInRoom(MySqlHelper *pMySqlHelper,int PK_Device,int PK_Room)
{
	string sSQL = "SELECT PK_Room FROM Room WHERE PK_Room=" + StringUtils::itos(PK_Room);
	PlutoSqlResult result_room;
	if( ( result_room.r=pMySqlHelper->mysql_query_result( sSQL ) )==NULL || result_room.r->row_count!=1 )
		return false;

	sSQL = "UPDATE Device SET FK_Room=" + StringUtils::itos(PK_Room) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
	pMySqlHelper->threaded_mysql_query(sSQL);
	return true;
}

bool DatabaseUtils::SetDeviceControlledVia(MySqlHelper *pMySqlHelper,int PK_Device,int PK_Device_ControlledVia)
{
	string sSQL = "SELECT PK_Device FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device_ControlledVia);
	PlutoSqlResult result_cv;
	if( ( result_cv.r=pMySqlHelper->mysql_query_result( sSQL ) )==NULL || result_cv.r->row_count!=1 )
		return false;

	sSQL = "UPDATE Device SET FK_Device_ControlledVia=" + StringUtils::itos(PK_Device_ControlledVia) + " WHERE PK_Device=" + StringUtils::itos(PK_Device);
	pMySqlHelper->threaded_mysql_query(sSQL);
	return true;
}

int DatabaseUtils::GetCommMethodForDeviceTemplate(MySqlHelper *pMySqlHelper,int PK_DeviceTemplate)
{
	string sSQL = "SELECT FK_CommMethod FROM DeviceTemplate WHERE PK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )!=NULL && (row=mysql_fetch_row(result.r))!=NULL && row[0] )
		return atoi(row[0]);
	return 0;
}

int DatabaseUtils::FindControlledViaCandidate(MySqlHelper *pMySqlHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,int iPK_Installation)
{
	PlutoSqlResult result_cv1,result_cv2;
	string SQL = "SELECT PK_Device FROM DeviceTemplate_DeviceTemplate_ControlledVia JOIN Device ON Device.FK_DeviceTemplate=FK_DeviceTemplate_ControlledVia WHERE DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate) + " AND FK_Installation=" + StringUtils::itos(iPK_Installation);
	if( (result_cv1.r=pMySqlHelper->mysql_query_result(SQL)) && result_cv1.r->row_count )
	{
		int PK_Device = FindControlledViaCandidate(pMySqlHelper,iPK_Device,iPK_DeviceTemplate,iPK_Device_RelatedTo,result_cv1);
		if( PK_Device )
			return PK_Device;
	}

	string sPK_DeviceCategory;
	SQL = "select DC1.PK_DeviceCategory,DC2.PK_DeviceCategory,DC2.FK_DeviceCategory_Parent "
		"FROM DeviceTemplate_DeviceCategory_ControlledVia "
		"JOIN DeviceCategory AS DC1 ON DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory=DC1.PK_DeviceCategory "
		"LEFT JOIN DeviceCategory AS DC2 ON DC2.FK_DeviceCategory_Parent=DC1.PK_DeviceCategory "
		"WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);

	if( (result_cv2.r=pMySqlHelper->mysql_query_result(SQL)) && result_cv2.r->row_count )
	{
		MYSQL_ROW row;
		while (row=mysql_fetch_row(result_cv2.r))
		{
			if(row[0] && atoi(row[0]))
				sPK_DeviceCategory += (sPK_DeviceCategory.size() ? "," : "") + string(row[0]);
			if(row[1] && atoi(row[1]))
				sPK_DeviceCategory += (sPK_DeviceCategory.size() ? "," : "") + string(row[1]);
			if(row[2] && atoi(row[2]))
				sPK_DeviceCategory += (sPK_DeviceCategory.size() ? "," : "") + string(row[2]);
		}
		PlutoSqlResult result_cv3;
		SQL = "SELECT PK_Device "
			"FROM Device "
			"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE FK_DeviceCategory IN (" + sPK_DeviceCategory + ") AND PK_Device<>" + StringUtils::itos(iPK_Device) + " AND FK_Installation=" + StringUtils::itos(iPK_Installation) + " ORDER BY PK_Device";

		if( (result_cv3.r=pMySqlHelper->mysql_query_result(SQL)) && result_cv3.r->row_count>0 )
			return FindControlledViaCandidate(pMySqlHelper,iPK_Device,iPK_DeviceTemplate,iPK_Device_RelatedTo,result_cv3);
	}
	return 0;
}

int DatabaseUtils::FindControlledViaCandidate(MySqlHelper *pMySqlHelper,int iPK_Device,int iPK_DeviceTemplate,int iPK_Device_RelatedTo,PlutoSqlResult &result)
{
	MYSQL_ROW row;
	map<int,pair<int,int> > mapDeviceTree;
	if( iPK_Device_RelatedTo )
		DatabaseUtils::GetAllDevicesInTree(pMySqlHelper,iPK_Device_RelatedTo,mapDeviceTree);

	// If the user wants a device that is related to iPK_Device_RelatedTo, but we can't find one, here is a 
	// un-related fallback device
	int iPK_Device_Fallback = 0;
	int iPK_Device_ControlledVia = 0; // The controlled via device that works
	int iGeneration = 0;  // What generation it is; get the smallest
	while (row=mysql_fetch_row(result.r))
	{
		int iPK_Device_Candidate = atoi(row[0]);
		if( !iPK_Device_RelatedTo )
			return iPK_Device_Candidate;

		map<int,pair<int,int> >::iterator it=mapDeviceTree.find(iPK_Device_Candidate);
		if( it!=mapDeviceTree.end() )
		{
			// If we haven't picked one yet, or this is a better match, use it
			if( !iPK_Device_ControlledVia || it->second.second<iGeneration )
			{
				iPK_Device_ControlledVia = iPK_Device_Candidate;
				iGeneration = it->second.second;
			}
		}
		iPK_Device_Fallback=iPK_Device_Candidate;
	}
	if( iPK_Device_ControlledVia )
		return iPK_Device_ControlledVia;
	return iPK_Device_Fallback;
}

int DatabaseUtils::ViolatesDuplicateRules(MySqlHelper *pMySqlHelper,int PK_Device_ControlledVia_temp,int iPK_DeviceTemplate)
{
	PlutoSqlResult result,result2;

	string sSQL = "SELECT IK_DeviceData FROM DeviceTemplate_DeviceData where FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Only_One_Per_PC_CONST) + " AND FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	DatabaseUtils::GetTopMostDevice(pMySqlHelper,PK_Device_ControlledVia_temp);
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )==NULL || (row=mysql_fetch_row(result.r))==NULL || !atoi(row[0]) )
		return 0;  // Only one per pc isn't set anyway

	int PK_Device_TopMost = DatabaseUtils::GetTopMostDevice(pMySqlHelper,PK_Device_ControlledVia_temp);
	sSQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(iPK_DeviceTemplate);
	if( (result2.r=pMySqlHelper->mysql_query_result(sSQL)) )
	{
		MYSQL_ROW row;
		while (row=mysql_fetch_row(result2.r))
		{
			int PK_Device_Test = atoi(row[0]);
			int PK_Device_TopMost_Test = DatabaseUtils::GetTopMostDevice(pMySqlHelper,PK_Device_Test);
			if( PK_Device_TopMost_Test==PK_Device_TopMost )
				return PK_Device_Test;
		}
	}
	return 0;
}

string DatabaseUtils::GetNameForRoom(MySqlHelper *pMySqlHelper,int PK_Room)
{
	string sSQL = "SELECT Description FROM Room WHERE PK_Room=" + StringUtils::itos(PK_Room);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
		return row[0];
	else
		return "";
}

bool DatabaseUtils::AlreadyHasUsers(MySqlHelper *pMySqlHelper,int PK_Installation)
{
	string sSQL = "SELECT PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		+ StringUtils::itos(PK_Installation) + " LIMIT 1";

	PlutoSqlResult result_set_room;
	if( (result_set_room.r=pMySqlHelper->mysql_query_result(sSQL)) && result_set_room.r->row_count )
		return true;
	return false;
}

bool DatabaseUtils::AlreadyHasRooms(MySqlHelper *pMySqlHelper,int PK_Installation)
{
	string sSQL = "SELECT PK_Room FROM Room WHERE FK_Installation="
		+ StringUtils::itos(PK_Installation) + " LIMIT 1";

	PlutoSqlResult result_set_room;
	if( (result_set_room.r=pMySqlHelper->mysql_query_result(sSQL)) && result_set_room.r->row_count )
		return true;
	return false;
}

long DatabaseUtils::GetRoomForDevice(MySqlHelper *pMySqlHelper, int nPK_Device)
{
	string sSQL = "SELECT PK_Room FROM Device LEFT JOIN Room "
		"ON FK_Room=PK_Room AND Device.FK_Installation=Room.FK_Installation "
		"WHERE PK_Device = " + StringUtils::ltos(nPK_Device);  

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if((result_set.r = pMySqlHelper->mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set.r)) && row[0])
		return row[0] != NULL ? atoi(row[0]) : 0;

	return 0;
}

long DatabaseUtils::GetRoomByName(MySqlHelper *pMySqlHelper, string sDescription, int PK_RoomType)
{
	string sSQL = "SELECT PK_Room FROM Room where Description = '" + StringUtils::SQLEscape(sDescription) + "'";
	if( PK_RoomType )
		sSQL += " AND FK_RoomType = " + StringUtils::itos(PK_RoomType);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if((result_set.r = pMySqlHelper->mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set.r)) && row[0])
		return atoi(row[0]);

	sSQL = "INSERT INTO Room(Description,FK_RoomType) VALUES('"  + StringUtils::SQLEscape(sDescription) + "'," + StringUtils::itos(PK_RoomType);

	return pMySqlHelper->threaded_mysql_query_withID(sSQL);
}

string DatabaseUtils::GetDeviceCategoryForDeviceTemplate(MySqlHelper *pMySqlHelper,int PK_DeviceTemplate,bool IncludeParents)
{
	string sResult;
	string sSQL = "SELECT FK_DeviceCategory FROM DeviceTemplate where PK_DeviceTemplate = " + StringUtils::itos(PK_DeviceTemplate);

	PlutoSqlResult result_set;
	MYSQL_ROW row;
	if((result_set.r = pMySqlHelper->mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set.r)) && row[0])
	{
		sResult = row[0];
		if( IncludeParents )
		{
			int PK_DeviceCategory = atoi(sResult.c_str());
			while(PK_DeviceCategory)
			{
				string sSQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory where PK_DeviceCategory = " + StringUtils::itos(PK_DeviceCategory);
				PlutoSqlResult result_set;
				MYSQL_ROW row;
				if((result_set.r = pMySqlHelper->mysql_query_result(sSQL)) && (row = mysql_fetch_row(result_set.r)) && row[0])
				{
					sResult += "," + string(row[0]);
					PK_DeviceCategory = atoi(row[0]);
				}
				else
					PK_DeviceCategory = 0;
			}
		}
	}
	
	return sResult;
}

bool DatabaseUtils::IsValidControlledVia(MySqlHelper *pMySqlHelper,int PK_DeviceTemplate,int PK_Device_ControlledVia)
{
	int PK_DeviceTemplate_ControlledVia = DatabaseUtils::GetDeviceTemplateForDevice(pMySqlHelper,PK_Device_ControlledVia);
	string sSQL = "SELECT PK_DeviceTemplate_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia "
		"WHERE FK_DeviceTemplate_ControlledVia=" + StringUtils::itos(PK_DeviceTemplate_ControlledVia) + " AND FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate);

	PlutoSqlResult result1;
	result1.r=pMySqlHelper->mysql_query_result( sSQL );
	if( result1.r && result1.r->row_count>0 )
		return true; // It's a match. 

	sSQL = "SELECT PK_DeviceTemplate_DeviceCategory_ControlledVia FROM DeviceTemplate_DeviceCategory_ControlledVia "
		"WHERE FK_DeviceCategory IN (" + DatabaseUtils::GetDeviceCategoryForDeviceTemplate(pMySqlHelper,PK_DeviceTemplate_ControlledVia) + ") AND FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate);

	PlutoSqlResult result2;
	result2.r=pMySqlHelper->mysql_query_result( sSQL );
	if( result2.r && result2.r->row_count>0 )
		return true; // It's a match.

	return false;
}

bool DatabaseUtils::DeviceIsWithinCategory(MySqlHelper *pMySqlHelper,int PK_Device,int PK_DeviceCategory)
{
	string sSQL = "SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=" + StringUtils::itos(PK_Device);
	PlutoSqlResult result;

	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )==NULL || ( row=mysql_fetch_row( result.r ) )==NULL || row[0]==NULL )
		return false;

	// Loop through all generations of parents
	int PK_DeviceCategory_Test = atoi(row[0]);  // The Device's category
	while(true)
	{
		if( PK_DeviceCategory_Test==PK_DeviceCategory )
			return true;

		string sSQL = "SELECT FK_DeviceCategory_Parent FROM DeviceCategory WHERE PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory_Test);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )==NULL || ( row=mysql_fetch_row( result.r ) )==NULL || row[0]==NULL || (PK_DeviceCategory_Test = atoi(row[0]))==0 )
			return false;
	}
}

