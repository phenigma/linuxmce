#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include <iostream>
#include <fstream>
#else
#include "VIPShared/VIPIncludes.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD_PC_KeyWasPressed.h"

#ifdef BLUETOOTH_DONGLE
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

#ifdef APPSERVER
#include "../level2/AppServer/MODCommandImpl.h"
#include "BD/BDCommandProcessor.h"
#include "../level1/Controller/AppServer/ControllerAppServer.h"
#include "../pluto_dce/Table_Command.h"
#endif

BD_PC_KeyWasPressed::BD_PC_KeyWasPressed(int Key) 
	
{
	m_Key=Key;

#ifdef SYMBIAN
	LOG("#	Sending 'KeyWasPressed' command  #\n");
#endif 
}

void BD_PC_KeyWasPressed::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_Key);
}

void BD_PC_KeyWasPressed::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_Key = Read_long();
}


#ifdef BLUETOOTH_DONGLE
bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_OrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter)
	{
		//TODO: log this
		return false;
	}

	pOrbiter->m_pOrbiter->ButtonDown(m_Key);

	return true;
}
//#endif //BLUETOOTH_DONGLE
/*

#ifdef APPSERVER
bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_AppServer *pBTCPAS = (BDCommandProcessor_AppServer *) pProcessor;

	pBTCPAS->m_pControllerAppServer->ButtonDown(m_Key);

	MOBILE_ORBITER_DETECTORCommandImpl *pCmd = pBTCPAS->m_pMOBILE_ORBITER_DETECTORCommandImpl;

	char Key_string[2]=" ";
	Key_string[0] = m_Key;
printf("m_Key: %c\n",Key_string);
	DCESAFETYLOCK(bm, pCmd->m_BTMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Got key from phone: %s", Key_string);
	map<string, ActiveMenuData *>::iterator iAM;
	iAM = pCmd->m_OrbiterActiveMenu.find(pProcessor->m_sMacAddressPhone);
	if (iAM != pCmd->m_OrbiterActiveMenu.end())
	{
		ActiveMenuData *pActiveData = (*iAM).second;
		
		// Todo - this keystroke may change on other phones.  Shouldn't be hardcoded.  However the logic with the 
		// phones overlay is intended to be in PlutoServer, so I'm not sure where to put this
		if( m_Key=='C' && pActiveData->m_sCurrentInput.length()>0 ) // Clear the user entry when they hit 'c'
		{
			if( pActiveData->m_sCurrentInput.length()==1 )
				pActiveData->m_sCurrentInput="";
			else
				pActiveData->m_sCurrentInput = pActiveData->m_sCurrentInput.substr(0,pActiveData->m_sCurrentInput.length()-1);
			pCmd->ProcessMenu(pActiveData);
			return true;
		}
		bool bNeedsToWaitForImage = true;
		ActiveMenuData *m_OrbiterActiveMenu = pActiveData; 
		map<string,string>::iterator iA = m_OrbiterActiveMenu->m_KeyActions.find(Key_string);
		if (iA == m_OrbiterActiveMenu->m_KeyActions.end())
		{
			bNeedsToWaitForImage = false;
			iA = m_OrbiterActiveMenu->m_KeyActions.find(string("*")+Key_string);
		}   
		if (iA == m_OrbiterActiveMenu->m_KeyActions.end())
		{
			g_pPlutoLogger->Write(LV_WARNING, "No action found for key: %s", Key_string);
		}
		else
		{
			// Clear the existing menu out so the phone will have to wait for the next one.
			delete m_OrbiterActiveMenu->m_CurrentImage;
			m_OrbiterActiveMenu->m_CurrentImage = NULL;
			m_OrbiterActiveMenu->m_bWaitForImage = bNeedsToWaitForImage;
			g_pPlutoLogger->Write(LV_WARNING, "ReqImg-: Got key, %swait for new image.",bNeedsToWaitForImage ? "" :"NO ");

			string msgtokens = (*iA).second;
			string::size_type msgpos = 0;
			string msg;

			while((msg = StringUtils::Tokenize(msgtokens, "&", msgpos))!="")
			{
				g_pPlutoLogger->Write(LV_STATUS, "* Firing message: %s", msg.c_str());
	
				string::size_type pos = 0;
	
				int msgFrom = atoi(StringUtils::Tokenize(msg, " ", pos).c_str());
				int msgTo = atoi(StringUtils::Tokenize(msg, " ", pos).c_str());
				int msgType = atoi(StringUtils::Tokenize(msg, " ", pos).c_str());
				int msgID = atoi(StringUtils::Tokenize(msg, " ", pos).c_str());
				string tok;
				Message *pMsg = new Message(msgFrom, msgTo, PRIORITY_NORMAL, msgType, msgID, 0);
				bool bAbortLoop=false;
				while(!bAbortLoop && (tok = StringUtils::Tokenize(msg, " ", pos))!="")
				{  
					string value;
					if( msg[pos]=='\"' )
					{
						value = StringUtils::Tokenize(msg,"\"",++pos);
						++pos;
					}
					else
						value = StringUtils::Tokenize(msg, " ", pos);
					string::size_type pos2;
printf("checking for hrow %s\n",Key_string);
					while( pActiveData->m_pvectMenuItem && (pos2=value.find("{HROW}"))!=string::npos && pActiveData->m_iCurrentMenuLine<(int) pActiveData->m_pvectMenuItem->size() )
					{
						MOMenuItem *mi = (*pActiveData->m_pvectMenuItem)[pActiveData->m_iCurrentMenuLine];
printf("found hrow %s %d\n",Key_string,pActiveData->m_MenuType);
						if( pActiveData->m_MenuType==MENUITEM_FILELIST )
						{
							pMsg->m_dwID=COMMAND_NAV_GOTO_CONST;
							pMsg->m_dwPK_Device_To=pCmd->m_DeviceID;
							pMsg->m_mapParameters[C_COMMANDPARAMETER_PKID_OBJECT_HEADER_CONST]="-1";
							pMsg->m_mapParameters[C_COMMANDPARAMETER_PATH_CONST]=mi->m_sValue;
							bAbortLoop=true;
							break;
						}
						value.replace(pos2,5,mi->m_sValue);
					}
printf("no hrow\n");
					pMsg->m_mapParameters[atoi(tok.c_str())] = value; 
				}
				if( msgTo==pCmd->m_DeviceID )
				{
					// It's for us anyway
printf("it's for us %d\n",msgID);
					switch(msgID)
					{
					case COMMAND_EXECUTE_CELL_CONST:
printf("execute 1: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						{
							// See if we were passed in the row directly
							string sRowID = pMsg->m_mapParameters[C_COMMANDPARAMETER_DG_ROW_ID_CONST];
							if( sRowID.length()>0 )
								pActiveData->m_iCurrentMenuLine = atoi(sRowID.c_str()) + pActiveData->m_iFirstMenuLine;
						}
printf("execute 2: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_iCurrentMenuLine>=0 && pActiveData->m_iCurrentMenuLine+pActiveData->m_iFirstMenuLine<(int) pActiveData->m_pvectMenuItem->size() )
						{
							MOMenuItem *mi =(*pActiveData->m_pvectMenuItem)[pActiveData->m_iCurrentMenuLine+pActiveData->m_iFirstMenuLine];
							if( !mi->m_pMessage )
								return true; // Nothing to do??

							// See if there's special handling required
							int ID = atoi(pMsg->m_mapParameters[C_COMMANDPARAMETER_ID_CONST].c_str());
							if( ID==1 )
							{
								// It's an 'add to the queue' button
								mi->m_pMessage->m_mapParameters[C_COMMANDPARAMETER_FILE_NAME_CONST] = "+" + mi->m_pMessage->m_mapParameters[C_COMMANDPARAMETER_FILE_NAME_CONST];
							}
							else if( ID==2 )
							{
								// It's a 'preview' button
								mi->m_pMessage->m_mapParameters[C_COMMANDPARAMETER_FILE_NAME_CONST] = "*" + mi->m_pMessage->m_mapParameters[C_COMMANDPARAMETER_FILE_NAME_CONST];
							}

							Message *pSendMessage = new Message(mi->m_pMessage);
							pCmd->GetEvents()->SendOCMessage(pSendMessage);
						}
						break;
					case COMMAND_SEEK_DATA_GRID_CONST:
printf("before seek: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						pCmd->HandleScrollSeek(pActiveData,atoi(pMsg->m_mapParameters[C_COMMANDPARAMETER_TEXT_CONST].c_str()));
printf("after seek: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_pvectMenuItem->size()>0 )
							pActiveData->m_iCurrentMenuLine = 0;
						else
							pActiveData->m_iCurrentMenuLine = -1;
						pActiveData->m_TokenMap["CI"]=pActiveData->m_sCurrentInput;
						break;
					case COMMAND_GEN_MOVE_UP_CONST:
					case COMMAND_MOVE_GRID_UP_CONST:
						--pActiveData->m_iCurrentMenuLine;
printf("before move up - first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( msgID==COMMAND_MOVE_GRID_UP_CONST || pActiveData->m_iCurrentMenuLine<0 )
						{
							pActiveData->m_iCurrentMenuLine=0;
							pActiveData->m_iFirstMenuLine = pActiveData->m_iFirstMenuLine - (pActiveData->m_iLastMenuLine-pActiveData->m_iFirstMenuLine);
							if( pActiveData->m_iFirstMenuLine<0 )
								pActiveData->m_iFirstMenuLine=0;
printf("after move up first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						}
						break;
					case COMMAND_GEN_MOVE_DOWN_CONST:
					case COMMAND_MOVE_GRID_DOWN_CONST:
printf("before down first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_iFirstMenuLine + pActiveData->m_iCurrentMenuLine < (int) pActiveData->m_pvectMenuItem->size() )
						{
							++pActiveData->m_iCurrentMenuLine;
printf("during down menu type: %d last: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iLastMenuLine );
							if( msgID==COMMAND_MOVE_GRID_DOWN_CONST || pActiveData->m_iCurrentMenuLine>pActiveData->m_iLastMenuLine )
							{
								pActiveData->m_iCurrentMenuLine=0;
								pActiveData->m_iFirstMenuLine = pActiveData->m_iLastMenuLine+1;
								if( pActiveData->m_iFirstMenuLine>=(int) pActiveData->m_pvectMenuItem->size() )
								{
									pActiveData->m_iFirstMenuLine = (int) pActiveData->m_pvectMenuItem->size()-3;
									if( pActiveData->m_iFirstMenuLine<0 )
										pActiveData->m_iFirstMenuLine = 0;
								}
printf("after down first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
							}
						}
						break;
					default:
printf("unhandled %d\n",msgID);
						break;
					}
					pCmd->ProcessMenu(pActiveData);
				}
				else
					pCmd->GetEvents()->SendOCMessage(pMsg);
			}
		}	
	}		
	return true;
}
*/


#else
bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
#endif

