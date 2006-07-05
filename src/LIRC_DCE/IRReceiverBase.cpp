#include "IRReceiverBase.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_DeviceTemplate.h"

#include <iostream>
using namespace std;
using namespace DCE;

IRReceiverBase::IRReceiverBase(Command_Impl *pCommand_Impl)
: m_Virtual_Device_Translator()
{
	m_pCommand_Impl=pCommand_Impl;
	m_cCurrentScreen='M';
	
	m_mapAVWCommands["ok"] = "return";
	m_mapAVWCommands["volup"] = "plus";
	m_mapAVWCommands["voldn"] = "minus";
	m_mapAVWCommands["chup"] = "plus";
	m_mapAVWCommands["chdown"] = "minus";
	m_mapAVWCommands["back"] = "escape";
	m_mapAVWCommands["stop"] = "escape";
	m_mapAVWCommands["power"] = "escape";
}

void IRReceiverBase::GetConfig(DeviceData_Impl *pData)
{
	string sMapping;
	DCE::CMD_Get_Remote_Control_Mapping_DT CMD_Get_Remote_Control_Mapping_DT(pData->m_dwPK_Device, DEVICETEMPLATE_Infrared_Plugin_CONST,
		BL_SameHouse, &sMapping);
	m_pCommand_Impl->SendCommand(CMD_Get_Remote_Control_Mapping_DT);  // Get the codes from I/R Plugin

	string::size_type posTab=0;
	while(posTab<sMapping.size())
	{
		char cScreenType = StringUtils::Tokenize(sMapping,"\t",posTab)[0];
		char cRemoteLayout = StringUtils::Tokenize(sMapping,"\t",posTab)[0];
		string sThisMapping = StringUtils::Tokenize(sMapping,"\t",posTab);
		vector<string> vectButtons;
		StringUtils::Tokenize(sThisMapping,"\r\n",vectButtons);
		for(size_t s=0;s<vectButtons.size();++s)
		{
			string::size_type pos=0;
			// We got ourselves a valid one
			string::size_type posSlash=vectButtons[s].find('|');
			while( pos<vectButtons[s].size() )
			{
				string sToken = StringUtils::Tokenize(vectButtons[s],";|",pos);
				if( posSlash!=string::npos )  // Should always be true unless data is malformed
				{
					int iNumberOfArguments;
					char **pArgs = StringUtils::ConvertStringToArgs(vectButtons[s].substr(posSlash+1),iNumberOfArguments);
					if( iNumberOfArguments )  // Should always be true
					{
						g_pPlutoLogger->Write(LV_STATUS,"Screen %c, Layout %c, LIRCBtn %s fires: %s",cScreenType ? cScreenType : '*', cRemoteLayout ? cRemoteLayout : '*', sToken.c_str(),vectButtons[s].substr(posSlash+1).c_str());
						Message *pMessage = new Message(iNumberOfArguments,pArgs,pData->m_dwPK_Device);
						pMessage->m_dwPK_Device_To = m_Virtual_Device_Translator.TranslateVirtualDevice(pMessage->m_dwPK_Device_To);
						for(size_t s=0;s<pMessage->m_vectExtraMessages.size();++s)
							pMessage->m_vectExtraMessages[s]->m_dwPK_Device_To = m_Virtual_Device_Translator.TranslateVirtualDevice(pMessage->m_vectExtraMessages[s]->m_dwPK_Device_To);

						MapKeysToMessages *pMapKeysToMessages = m_mapKeyMapping[StringUtils::ToUpper(sToken)];
						if( !pMapKeysToMessages )
						{
							pMapKeysToMessages = new MapKeysToMessages();
							m_mapKeyMapping[StringUtils::ToUpper(sToken)] = pMapKeysToMessages;
						}

						(*pMapKeysToMessages)[ make_pair<char,char> (cRemoteLayout,cScreenType) ] = pMessage;
					}
					if( posSlash<=pos )
						break; // We already parsed the last one
				}
			}
		}
	}
}

IRReceiverBase::~IRReceiverBase()
{
	return; // Todo - this crashes???  not too important since it's just memory cleanup on exit
	for(map<string,MapKeysToMessages *>::iterator it=m_mapKeyMapping.begin();it!=m_mapKeyMapping.end();++it)
	{
		MapKeysToMessages *pMapKeysToMessages = it->second;
		if( pMapKeysToMessages )
		{
			for(MapKeysToMessages::iterator it2=pMapKeysToMessages->begin();it2!=pMapKeysToMessages->end();++it)
				delete it2->second;
		}

		delete (*it).second;
	}
}

void IRReceiverBase::ReceivedCode(int PK_Device_Remote,const char *pCode)
{
	char cRemoteLayout = m_mapRemoteLayout[PK_Device_Remote];
	map<string,MapKeysToMessages *>::iterator it = m_mapKeyMapping.find(StringUtils::ToUpper(pCode));
	if( it!=m_mapKeyMapping.end() )
	{
		MapKeysToMessages *pMapKeysToMessages = it->second;
		MapKeysToMessages::iterator itMessage;
		
		itMessage = pMapKeysToMessages->find(make_pair<char,char> (cRemoteLayout,m_cCurrentScreen) );
		if( itMessage==pMapKeysToMessages->end() )
			itMessage = pMapKeysToMessages->find(make_pair<char,char> (cRemoteLayout,0) );
		if( itMessage==pMapKeysToMessages->end() )
			itMessage = pMapKeysToMessages->find(make_pair<char,char> (0,m_cCurrentScreen) );
		if( itMessage==pMapKeysToMessages->end() )
			itMessage = pMapKeysToMessages->find(make_pair<char,char> (0,0) );

		if( itMessage!=pMapKeysToMessages->end() )
		{
			Message *pm = itMessage->second;
			g_pPlutoLogger->Write(LV_STATUS,"IRReceiverBase::ReceivedCode Sending Message Type %d ID %d by  code: %s device %d (screen %c remote layout %c)",
					pm->m_dwMessage_Type,pm->m_dwID,pCode,PK_Device_Remote,m_cCurrentScreen,cRemoteLayout);
			m_pCommand_Impl->QueueMessageToRouter(new Message(pm));
		}
		else
			g_pPlutoLogger->Write(LV_WARNING,"No mapping for code: %s device %d",pCode,PK_Device_Remote);
	}
	else
		g_pPlutoLogger->Write(LV_WARNING,"Cannot find code %s device %d",pCode,PK_Device_Remote);
}

std::string IRReceiverBase::GetSocketText(int Socket) const
{
	int Len = 0;
	int iRecv = recv(Socket, &Len, sizeof(Len), 0);
	if( iRecv != sizeof(Len) || Len <= 0 )
	{
		return std::string();
	}
	
	std::string Buffer;
	Buffer.reserve(Len);
	Buffer.insert(0, Len, 0);
	iRecv = recv(Socket, const_cast<char*>(Buffer.data()), Len, 0);
	if( iRecv != Len )
	{
		return std::string();
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Buffer %d = %s", Buffer.length(), Buffer.c_str());
	
	return Buffer;
}

bool IRReceiverBase::SendSocketText(int Socket, std::string Text) const
{
	int Len = Text.length();
	
	g_pPlutoLogger->Write(LV_STATUS, "SendSocketText %d = %s", Len, Text.c_str());
	
	if(  0 > write(Socket, &Len, sizeof(Len)) )
	{
		return false;
	}
	
	if( Len != write(Socket, Text.c_str(), Len) )
	{
		return false;
	}
	
	return true;
}

void IRReceiverBase::ForceKeystroke(string sCommand, string sAVWHost, int iAVWPort)
{
	g_pPlutoLogger->Write(LV_STATUS,"IRReceiverBase::ForceKeystroke %s",sCommand.c_str());
	
	map<string, string>::iterator it = m_mapAVWCommands.find(sCommand);
	if(it != m_mapAVWCommands.end())
		sCommand = it->second;
	
	int sockfd = 0;
	struct hostent *he = 0;
	struct sockaddr_in their_addr; // connector's address information 

	if ((he=gethostbyname(sAVWHost.c_str())) == NULL) {  // get the host info 
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed on gethostbyname");
		return;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to open a new socket");
		return;
	}

	bzero(&their_addr, sizeof(their_addr));
	their_addr.sin_family = AF_INET;    // host byte order 
	their_addr.sin_port = htons(iAVWPort);  // short, network byte order 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to connect on AVW");
		close(sockfd);
		return;
	}

	std::string Text = GetSocketText(sockfd);
	if( Text == "OPEN" )
	{
		if( !SendSocketText(sockfd, sCommand) )
		{
			g_pPlutoLogger->Write(LV_WARNING, "Failed to send to AVW: %s", sCommand.c_str());
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "Received from AVW : %s", Text.c_str());
	}
	
	close(sockfd);
}

