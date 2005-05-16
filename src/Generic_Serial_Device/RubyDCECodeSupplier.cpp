/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyDCECodeSupplier.h"

#include "DCE/Message.h"
#include "DCE/Logger.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Command_Impl.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "Gen_Devices/AllCommandsRequests.h"

using namespace std;

namespace DCE {

void 
RubyDCECodeSupplier::addCode(Database_pluto_main* pdb, Command_Impl *pcmdimpl, DeviceData_Impl* pdevicedata) {
	if(rcode_.length() == 0) {
		rcode_ = "require 'Ruby_Generic_Serial_Device'""\n";
	}

	unsigned long devtemplid = pdevicedata->m_dwPK_DeviceTemplate;
	unsigned long devid = pdevicedata->m_dwPK_Device;
	
	devtemplid;
	
	/* 
		header:
			in header we define type names for wrapper classes in order to shorten the names */
	
	rcode_ += "class Command < Ruby_Generic_Serial_Device::RubyCommandWrapper\nend\n";
	/* header end*/	
	
	char tmpbuff[20];
	sprintf(tmpbuff, "%lu", devid);
	rcode_ += "class Device_"; rcode_ += tmpbuff; rcode_ += " < Ruby_Generic_Serial_Device::RubySerialIOWrapper""\n";


	g_pPlutoLogger->Write(LV_STATUS, "Fetching Ruby code from Infrared Plugin");
	
	map<int,string> mapClass;  
	// this will have all the Ruby code, where int is the PK_Command and string is the codeint 
	int iSize = 0; char* pData = NULL; // Place holders for the 'out' parameter
	DCE::CMD_Get_Infrared_Codes_DT CMD_Get_Infrared_Codes_DT(devid, DEVICETEMPLATE_Infrared_Plugin_CONST,
				BL_SameHouse, devid, &pData, &iSize);
	pcmdimpl->SendCommand(CMD_Get_Infrared_Codes_DT);  // Get the codes from I/R Plugin
	SerializeClass mapsc(true);  // A manual serialize class
	mapsc + mapClass;
	mapsc.SerializeRead(iSize, pData); // De-serialize the data
	
	g_pPlutoLogger->Write(LV_STATUS, "Fetched %d commands...", mapClass.size());

	for(map<int,string>::iterator it = mapClass.begin(); it != mapClass.end(); it++ ) {
		int cmdid = (*it).first;
		string scmdid = StringUtils::itos(cmdid);
		if(!isCmdImplemented(cmdid)) {
			if(cmdid == COMMAND_Private_Method_Listing_CONST) {
				if(!privateassigned_) {
					rcode_ += "#### PRIVATE METHODS ####################################################################\n";
					rcode_ += (*it).second;
					rcode_ += "\n";
					privateassigned_ = true;
				} 
			} else
			if(cmdid == COMMAND_Process_Receive_Command_For_Child_CONST) {
				if(!procchildcmdassigned_) {
					rcode_ += "def cmd_ReceiveCommandForChild(cmd)\n";
					rcode_ += (*it).second;
					rcode_ += "\n""end""\n"; // def
					procchildcmdassigned_ = true;
				}
			} else {
				string scmdtext = TranslateCommandToRuby((*it).second);
				if(!scmdtext.empty()) {
					/*for each command*/
					rcode_ += "#### " + scmdid + " ####################################################################\n";
					rcode_ += "def cmd_"; rcode_ += scmdid; rcode_ += "(";
		
					/*add command parameters*/
					std::string sql = "select PK_CommandParameter, CommandParameter.Description "
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
					rcode_ += TranslateCommandToRuby(scmdtext);
					/*insert ruby code for the method*/
					rcode_ += "\n""end""\n"; // def
				}
			}
		}
	}
		
	/*	
	FillClassMembersFromDevice(pdb, devid, false);
	FillClassMembersFromDevice(pdb, devtemplid, true);
	*/
	
	rcode_ += "end""\n"; // class
}

void 
RubyDCECodeSupplier::FillClassMembersFromDevice(Database_pluto_main* pdb, long id, bool isDevTemplate) {
	char tmpbuff[20];
	sprintf(tmpbuff, "%lu", id);
	string sid = tmpbuff;
	
	string sql = "select FK_Command, IRData "
					"from InfraredGroup_Command "
						"inner join InfraredGroup_Command_Preferred on FK_InfraredGroup_Command=PK_InfraredGroup_Command "
 					"where ";
	
	if(isDevTemplate) {
		sql += "FK_DeviceTemplate="; sql += sid;
	} else {
		sql += "FK_Device="; sql += sid;
	}
					
	sql += " group by FK_Command order by FK_Command asc; ";

	g_pPlutoLogger->Write(LV_STATUS, "Running query to get Ruby code: \n%s", sql.c_str())	;

	PlutoSqlResult cmds;
	if((cmds.r = pdb->mysql_query_result(sql.c_str()))) {
		MYSQL_ROW rowcmd;
		while((rowcmd = mysql_fetch_row(cmds.r))) {
			string scmdid = rowcmd[0];
			int cmdid = atoi(rowcmd[0]);
			if(!isCmdImplemented(cmdid) && rowcmd[1] != NULL) {
				if(cmdid == COMMAND_Private_Method_Listing_CONST) {
					if(!privateassigned_ && rowcmd[1] != NULL) {
						rcode_ += "#### PRIVATE METHODS ####################################################################\n";
						rcode_ += rowcmd[1];
						rcode_ += "\n";
						privateassigned_ = true;
					} 
				} else {
					string scmdtext = TranslateCommandToRuby(rowcmd[1]);
					if(!scmdtext.empty()) {
						/*for each command*/
						rcode_ += "#### " + scmdid + " ####################################################################\n";
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
						if(rowcmd[1] != NULL) {
							rcode_ += TranslateCommandToRuby(rowcmd[1]);
						}
					
						/*insert ruby code for the method*/
						rcode_ += "\n""end""\n"; // def
					}
				}
			}
		}
	}
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

bool 
RubyDCECodeSupplier::isCmdImplemented(int cmd) {
	return (cmdparammap_.find(cmd) != cmdparammap_.end());
}

RubyDCECodeSupplier::RubyDCECodeSupplier() {
}


RubyDCECodeSupplier::~RubyDCECodeSupplier() {
}

std::string 
RubyDCECodeSupplier::TranslateCommandToRuby(const std::string& cmdtxt) {
	if(cmdtxt.empty()) {
		return cmdtxt;
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Translating Command CODE (length %d): \n%s\n", cmdtxt.length(), cmdtxt.c_str());
	
	string ret;
	int first = -1, last = -1;
	while(last < (int)cmdtxt.length()) {
		first = cmdtxt.find("<$", last + 1);
		if(first < 0) {	
			ret += cmdtxt.substr(last + 1, cmdtxt.length() - last - 1);
			break;
		} else {
			ret += cmdtxt.substr(last + 1, first - last - 1);
			
			first += 2;
			last = cmdtxt.find("$>", first + 1);
			if(last < 0) {
				break;
			} else {
				ret += "conn_.Send(" + cmdtxt.substr(first, last - first) + ")\n";
				last += 2;
			}
		}
	}
	return ret;
}


};

