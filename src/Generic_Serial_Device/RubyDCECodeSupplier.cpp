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
#include "PlutoUtils/StringUtils.h"
#include "pluto_main/Database_pluto_main.h"

using namespace std;

namespace DCE {

void 
RubyDCECodeSupplier::addCode(Database_pluto_main* pdb, DeviceData_Base* pdevicedata) {
	char tmpbuff[20];
	
	if(rcode_.length() == 0) {
		rcode_ = "require 'Ruby_Generic_Serial_Device'""\n";
	}

	unsigned long devtemplid = pdevicedata->m_dwPK_DeviceTemplate;
	unsigned long devid = pdevicedata->m_dwPK_Device;
	
	sprintf(tmpbuff, "%lu", devtemplid);
	string sdevtemplid = tmpbuff;
	
	sprintf(tmpbuff, "%lu", devid);
	string sdevid = tmpbuff;

	/* 
		header:
			in header we define type names for wrapper classes in order to shorten the names */
	
	rcode_ += "class Command < Ruby_Generic_Serial_Device::RubyCommandWrapper\nend\n";
	/* header end*/	
	
	rcode_ += "class Device_"; rcode_ += sdevid; rcode_ += " < Ruby_Generic_Serial_Device::RubySerialIOWrapper""\n";
	
	string sql = "select FK_Command, IRData "
					"from InfraredGroup_Command "
						"inner join InfraredGroup_Command_Preferred on FK_InfraredGroup_Command=PK_InfraredGroup_Command "
 					"where FK_DeviceTemplate="; sql += sdevtemplid; sql += " order by FK_Command asc; ";

	g_pPlutoLogger->Write(LV_STATUS, "Running query to get Ruby code: \n%s", sql.c_str())	;
						
	PlutoSqlResult cmds;
	if((cmds.r = pdb->mysql_query_result(sql.c_str()))) {
		MYSQL_ROW rowcmd;
		while((rowcmd = mysql_fetch_row(cmds.r))) {
			int cmdid = atoi(rowcmd[0]);
			sprintf(tmpbuff, "%d", cmdid);
			string scmdid = tmpbuff;
			
			/*for each command*/
			rcode_ += "def cmd_"; rcode_ += scmdid; rcode_ += "(";

			/*add command parameters*/
			sql = "select PK_CommandParameter, CommandParameter.Description "
								"from CommandParameter "
									"inner join Command_CommandParameter on FK_CommandParameter=PK_CommandParameter "
									"inner join Command on FK_Command=PK_Command "
								"where PK_Command = "; sql += scmdid; sql += " order by PK_CommandParameter asc";
									
			g_pPlutoLogger->Write(LV_STATUS, "Running query to get params for Command %s: \n%s", scmdid.c_str(), sql.c_str());
			
			PARAMLIST& paramlist = cmdparammap_[cmdid];
			PlutoSqlResult params;
			if((params.r = pdb->mysql_query_result(sql.c_str()))) {
				MYSQL_ROW rowparam;
				while((rowparam = mysql_fetch_row(params.r))) {
					if(paramlist.size() > 0) {
						rcode_ += ", ";
					}
					string rbparam = StringUtils::ToLower(FileUtils::ValidCPPName(rowparam[1]).c_str());
					
					PARAMPAIR parampair(atoi(rowparam[0]), rbparam);
					paramlist.push_back(parampair);
					
					rcode_ += rbparam;
				}
			}
			
			g_pPlutoLogger->Write(LV_STATUS, "Added %d parameeters for Command %s: ", paramlist.size(), scmdid.c_str());
			
			rcode_ += ")""\n"; // SetLevel(
			//rcode_ += "conn_ = getConn()""\n";
			rcode_ += rowcmd[1];
			
			/*insert ruby code for the method*/
			rcode_ += "\n""end""\n"; // def
			
		}
	}
	
	rcode_ += "end""\n"; // class
}

void 
RubyDCECodeSupplier::clearCode() {
	rcode_.clear();
	cmdparammap_.clear();
}


int 
RubyDCECodeSupplier::getParamsOrderForCmd(/*in*/int cmd, /*out*/std::list<int>& params) {
	params.clear();
	
	PARAMLIST& paramlist = cmdparammap_[cmd];
	PARAMLIST::iterator it = paramlist.begin();
	while(it != paramlist.end()) {
		params.push_back((*it).first);
		it++;
	}
	
	return params.size();
}

int 
RubyDCECodeSupplier::getParamsNamesForCmd(/*in*/int cmd, /*out*/std::list<string>& params) {
	params.clear();
	
	PARAMLIST& paramlist = cmdparammap_[cmd];
	PARAMLIST::iterator it = paramlist.begin();
	while(it != paramlist.end()) {
		params.push_back((*it).second);
		it++;
	}
	
	return params.size();
}


RubyDCECodeSupplier::RubyDCECodeSupplier() {
}


RubyDCECodeSupplier::~RubyDCECodeSupplier() {
}

};
