//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "RubyDCECodeSupplier.h"

#include "DCE/Message.h"
#include "DCE/Logger.h"
#include "DCE/DeviceData_Base.h"
#include "PlutoUtils/FileUtils.h"
#include "pluto_main/Database_pluto_main.h"

using namespace std;

namespace DCE {

void 
RubyDCECodeSupplier::addCode(Database_pluto_main* pdb, DeviceData_Base* pdevicedata) {
	char tmpbuff[20];
	
	if(rcode_.length() == 0) {
		rcode_ = "require 'Ruby_Generic_Serial_Device'""\n";
	}

	sprintf(tmpbuff, "%lu", pdevicedata->m_dwPK_DeviceTemplate);
	string devtemplid = tmpbuff;
	
	sprintf(tmpbuff, "%lu", pdevicedata->m_dwPK_Device);
	string devid = tmpbuff;
	
	rcode_ += "class Device_"; rcode_ += devid; rcode_ += " < Ruby_Generic_Serial_Device::RubySerialIOWrapper""\n";
	
	string sql = "select FK_Command, IRData "
					"from InfraredGroup_Command "
						"inner join InfraredGroup_Command_Preferred on FK_InfraredGroup_Command=PK_InfraredGroup_Command "
 					"where FK_DeviceTemplate="; sql += devtemplid; sql += " order by FK_Command asc; ";

	g_pPlutoLogger->Write(LV_STATUS, "Running query to get Ruby code: \n%s", sql.c_str())	;
						
	PlutoSqlResult cmds;
	if((cmds.r = pdb->mysql_query_result(sql.c_str()))) {
		MYSQL_ROW rowcmd;
		while((rowcmd = mysql_fetch_row(cmds.r))) {
			sprintf(tmpbuff, "cmd_%d", atoi(rowcmd[0]));
			string cmdid = tmpbuff;
			
			/*for each command*/
			rcode_ += "def "; rcode_ += cmdid; rcode_ += "(";

			int nparams = 0;
			/*add command parameters*/
			sql = "select PK_CommandParameter, CommandParameter.Description "
								"from CommandParameter "
									"inner join Command_CommandParameter on FK_CommandParameter=PK_CommandParameter "
									"inner join Command on FK_Command=PK_Command "
								"where PK_Command = "; sql += rowcmd[0]; sql += " order by PK_CommandParameter asc";
								
			PlutoSqlResult params;
			if(!(params.r = pdb->mysql_query_result(sql.c_str()))) {
				MYSQL_ROW rowparam;
				while((rowparam = mysql_fetch_row(params.r))) {
					rcode_ += ", ";
					rcode_ += FileUtils::ValidCPPName(rowparam[1]);
					nparams++;
				}
			}
			
			rcode_ += ")""\n"; // SetLevel(
			//rcode_ += "conn_ = getConn()""\n";
			rcode_ += rowcmd[1];
			
			/*insert ruby code for the method*/
			rcode_ += "end""\n"; // def
			
		}
	}
	
	rcode_ += "end""\n"; // class
}

RubyDCECodeSupplier::RubyDCECodeSupplier() {
}


RubyDCECodeSupplier::~RubyDCECodeSupplier() {
}

};
