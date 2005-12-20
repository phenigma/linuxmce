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
	map<int,int> mapDeviceTree;
	GetAllDevicesInTree(pMySqlHelper,PK_Device,mapDeviceTree);
	string sDeviceList;
	for(map<int,int>::iterator it=mapDeviceTree.begin();it!=mapDeviceTree.end();++it)
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

void DatabaseUtils::GetAllDevicesInTree(MySqlHelper *pMySqlHelper,int PK_Device,map<int,int> &mapDeviceTree,bool bCheckParent,int PK_Device_ChildExclude)
{
	{
		string sSQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
			mapDeviceTree[PK_Device]=atoi(row[0]);
	}
	if( bCheckParent )
	{
		string sSQL = "SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=" + StringUtils::itos(PK_Device);
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) && row[0] )
			GetAllDevicesInTree(pMySqlHelper,atoi(row[0]),mapDeviceTree,true,PK_Device);
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
					GetAllDevicesInTree(pMySqlHelper,PK_Device_Child,mapDeviceTree,false,0);
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
	string sSQL = "SELECT FK_CommMethod FROM DeviceTemplate JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup WHERE PK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=pMySqlHelper->mysql_query_result( sSQL ) )!=NULL && (row=mysql_fetch_row(result.r))!=NULL && row[0] )
		return atoi(row[0]);
	return 0;
}

