//#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "Logger.h"
#include "PhoneDevice.h"
#include "BDCommandProcessor.h"
#include "BDCommandProcessor_Linux_Bluetooth.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/socket.h>

using namespace DCE;

void DummySignalHandler(int) {
g_pPlutoLogger->Write(LV_CRITICAL, "Need to cancel bd comm");
}

BDCommandProcessor_Linux_Bluetooth::BDCommandProcessor_Linux_Bluetooth(string sMacAddressPhone,string sMacAddressDongle,class PhoneDevice *pDevice)
: BDCommandProcessor(sMacAddressPhone)
	
{
printf("start of constructor %p\n",g_pPlutoLogger);
g_pPlutoLogger->Write(LV_STATUS,"start of const %s",sMacAddressPhone.c_str());

	m_bQuit = false;
	m_bRunning = false;

	m_pDevice=pDevice;
	if( !m_pDevice )
		cerr << "temporary hack -- m_pDevice is NULL. " << sMacAddressPhone << endl;
	// Assert: ON_OFF is 1 and the map has no connection.  Try to connect
	
	struct sockaddr_rc laddr, raddr;
	struct rfcomm_dev_req req;
	socklen_t alen;

	laddr.rc_family = AF_BLUETOOTH;
	bacpy(&laddr.rc_bdaddr, &m_pDevice->m_bdaddrDongle);
	laddr.rc_channel = 0;

	//if( NULL != m_pDevice->m_bdaddrDongle ) 
	//printf("m_pDevice->m_bdaddrDongle is null !!\n");

	printf("@mac address: %s\n", pDevice->m_sMacAddress.c_str());
	
	raddr.rc_family = AF_BLUETOOTH;
	str2ba(pDevice->m_sMacAddress.c_str(), &raddr.rc_bdaddr);
	raddr.rc_channel = 19;
	
	if ((m_CommHandle = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) 
	{
		g_pPlutoLogger->Write(LV_WARNING,"Can't create RFCOMM socket");
		m_bDead=true;
		return;
	}

	if (bind(m_CommHandle, (struct sockaddr *)&laddr, sizeof(laddr)) < 0) 
	{
		g_pPlutoLogger->Write(LV_WARNING,"Can't bind RFCOMM socket %s",sMacAddressPhone.c_str());
		close(m_CommHandle);
		m_bDead=true;
		return;
	}
	
	int iRetries = 10;
    int err_code = 0;
	while(iRetries--)
	{
		err_code = 0;
	    printf("# try to connect\n");
		if((err_code = connect(m_CommHandle, (struct sockaddr *)&raddr, sizeof(raddr))) < 0) 
		{
			g_pPlutoLogger->Write(LV_WARNING,"Can't connect RFCOMM socket %s. (%d)",sMacAddressPhone.c_str(), 10 - iRetries);
			printf("# error code: %d\n", err_code);
			Sleep(1000);
		}
		else
			break; //connected
	}

	if(err_code < 0) 
	{
		g_pPlutoLogger->Write(LV_WARNING,"Failed to connect to PlutoMO, phone mac %s", sMacAddressPhone.c_str());
		close(m_CommHandle);
	    m_bDead=true;
		return;
	}

	alen = sizeof(laddr);
	if (getsockname(m_CommHandle, (struct sockaddr *)&laddr, &alen) < 0) 
	{
		g_pPlutoLogger->Write(LV_WARNING,"Can't get RFCOMM socket name %s",sMacAddressPhone.c_str());
		close(m_CommHandle);
		m_bDead=true;
		return;
	}

	g_pPlutoLogger->Write(LV_WARNING,"Successfully connected to the symbian application %s",sMacAddressPhone.c_str());
}

BDCommandProcessor_Linux_Bluetooth::~BDCommandProcessor_Linux_Bluetooth()
{
	if (m_CommHandle)
	{
		m_bQuit = true;
		signal(SIGUSR1 ,DummySignalHandler);
		raise(SIGUSR1);
		
		time_t start = time(NULL);
		while(m_bRunning)
		{
			if(time(NULL) - start > 5)
				break;
			
			g_pPlutoLogger->Write(LV_STATUS, "Waiting any operation with the socket to finish...");
			Sleep(50);
		}
		
		close(m_CommHandle);
		m_CommHandle = 0;
	}
}

bool BDCommandProcessor_Linux_Bluetooth::SendData(int size, const char *data)
{
//	g_pPlutoLogger->Write(LV_STATUS, "Ready to send %d bytes of data", size);
	
	m_bRunning = true;
	
	int br;
	int bytes_to_send = size;
	int bytes_sent = 0;

	while(bytes_to_send > 0 && !m_bQuit)
	{
		br = write(m_CommHandle, data + bytes_sent, bytes_to_send);

		if(br == -1) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Failed to send %d bytes of data", size);
			return false;
		}
		else 
		{
			bytes_sent += br;
			bytes_to_send -= br;

//			g_pPlutoLogger->Write(LV_STATUS, "Sent %d bytes [total %d/%d]", br, bytes_sent, size);
		}

		if(bytes_to_send > 0)
		{
//			printf("@need to send more data... \n");
		}
	}

	m_bRunning = false;
}

char *BDCommandProcessor_Linux_Bluetooth::ReceiveData(int size)
{
//	g_pPlutoLogger->Write(LV_STATUS, "Ready to receive %d bytes of data", size);
//
	m_bRunning = true;
		
	char *buffer = (char *)malloc(size);
	int br = 0;
	int bytes_to_receive = size;
	int bytes_received = 0;

	while(bytes_to_receive > 0 && !m_bQuit)
	{
		br = read(m_CommHandle, buffer + bytes_received, bytes_to_receive);

		if (br==-1) 
		{
			g_pPlutoLogger->Write(LV_STATUS, "Failed to receive %d bytes of data", size);
			return NULL;
		}
		else 
		{
			bytes_received += br;
			bytes_to_receive -= br;
			
//			g_pPlutoLogger->Write(LV_STATUS, "Received %d bytes [total %d/%d]", br, bytes_received, size);
		}

		if(bytes_to_receive > 0)
		{
//			printf("@need to receive more data.. \n");
		}
	}

	m_bRunning = false;
	
	return buffer;
}


/*
void MobileOrbiterSocket::ProcessSocket()
{
//	while( m_bdCommandProcessor->ReceiveCommand() );
//	return;

printf("in read bt\n");
	m_bExit = false;
	do 
	{
		char buffer[5];
		unsigned long bytes_read = 0;
	
		memset(buffer, 0, 5);
		if (!ReadFile(m_CommHandle, buffer, 4, &bytes_read))
		{
			g_pPlutoLogger->Write(LV_STATUS,"command: %s\r\n", GetLastError());
			m_bExit = true;
		}
		
		
		else
		{
printf("before if\n");
			if (!strcmp(buffer, "IMGR"))
			{
printf("after if\n");
				g_pPlutoLogger->Write(LV_STATUS,"Phone requests current image");

				bool bGotImage;
				time_t timeout = time(NULL)+2;
				
				do
				{
					DCESAFETYLOCK(bm, m_pCmd->m_BTMutex);
					map<string, ActiveMenuData *>::iterator iAM;
					iAM = m_pCmd->m_OrbiterActiveMenu.find(m_ID);
					if (iAM != m_pCmd->m_OrbiterActiveMenu.end())
					{
						if ((*iAM).second->m_CurrentImage)
						{
							if (!WriteImageToPhone((*iAM).second->m_CurrentImage, (*iAM).second->m_CurrentImageSize))
							{
								m_bExit = true;
								return;
							}
							delete (*iAM).second->m_CurrentImage;
							(*iAM).second->m_CurrentImage = NULL;
							(*iAM).second->m_bWaitForImage = false;
							g_pPlutoLogger->Write(LV_STATUS,"Wrote image");
							break;
						}
						else
						{
							if (!((*iAM).second->m_bWaitForImage) && WriteImageToPhone(NULL, 0))
							{
								break;
							}
						}
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS,"Not in map");
					}

						
				} while(time(NULL) < timeout);
				if (time(NULL) >= timeout)
				{
					g_pPlutoLogger->Write(LV_WARNING, "Phone is requesting image, but its image wasn't populated in over 2 seconds.");
					WriteImageToPhone(NULL, 0);
				}					
			}
			
			else if (!strncmp(buffer, "KY", 2))
				PressedKey(StringUtils::Format("%d", (int)((unsigned char)buffer[2]+256*(unsigned char)buffer[3])));
			else if (!strncmp(buffer, "KEY", 3))
				PressedKey(StringUtils::Format("%d", (int)((unsigned char)buffer[3])));
			else
				g_pPlutoLogger->Write(LV_STATUS,"Got unknown command \"%s\"\r\n", buffer);


			{
				DCESAFETYLOCK(bm, m_pCmd->m_BTMutex);
			
				map<string, ActiveMenuData *>::iterator itAD = m_pCmd->m_OrbiterActiveMenu.find(m_ID);
	printf("before check: %s\n",m_ID.c_str());
				if( itAD!=m_pCmd->m_OrbiterActiveMenu.end() )
				{
	printf("checking video.  streaming\n");
					ActiveMenuData *pActiveData = (*itAD).second;
					if( pActiveData && pActiveData->m_bContainsStreamingVideo )
					{
	printf("Contains video.  streaming\n");
						m_pCmd->ProcessMenu(pActiveData);
					}
				}
			}
		}
	} while (!m_bExit);
	m_bDead = true;
}

void MobileOrbiterSocket::PressedKey(string Key)
{
printf("Key: %s\n",Key.c_str());
	DCESAFETYLOCK(bm, m_pCmd->m_BTMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Got key from phone: %s", Key.c_str());
	map<string, ActiveMenuData *>::iterator iAM;
	iAM = m_pCmd->m_OrbiterActiveMenu.find(m_ID);
	if (iAM != m_pCmd->m_OrbiterActiveMenu.end())
	{
		ActiveMenuData *pActiveData = (*iAM).second;
		
		// Todo - this keystroke may change on other phones.  Shouldn't be hardcoded.  However the logic with the 
		// phones overlay is intended to be in PlutoServer, so I'm not sure where to put this
		if( Key=="1" && pActiveData->m_sCurrentInput.length()>0 ) // Clear the user entry when they hit 'c'
		{
			if( pActiveData->m_sCurrentInput.length()==1 )
				pActiveData->m_sCurrentInput="";
			else
				pActiveData->m_sCurrentInput = pActiveData->m_sCurrentInput.substr(0,pActiveData->m_sCurrentInput.length()-1);
			m_pCmd->ProcessMenu(pActiveData);
			return;
		}
		bool bNeedsToWaitForImage = true;
		ActiveMenuData *m_OrbiterActiveMenu = pActiveData; 
		map<string,string>::iterator iA = m_OrbiterActiveMenu->m_KeyActions.find(Key);
		if (iA == m_OrbiterActiveMenu->m_KeyActions.end())
		{
			bNeedsToWaitForImage = false;
			iA = m_OrbiterActiveMenu->m_KeyActions.find("*"+Key);
		}   
		if (iA == m_OrbiterActiveMenu->m_KeyActions.end())
		{
			g_pPlutoLogger->Write(LV_WARNING, "No action found for key: %s", Key.c_str());
		}
		else
		{
			// Clear the existing menu out so the phone will have to wait for the next one.
			delete m_OrbiterActiveMenu->m_CurrentImage;
			m_OrbiterActiveMenu->m_CurrentImage = NULL;
			m_OrbiterActiveMenu->m_bWaitForImage = bNeedsToWaitForImage;
			g_pPlutoLogger->Write(LV_WARNING, "ReqImg-%s: Got key, %swait for new image.",m_ID.c_str(), bNeedsToWaitForImage ? "" :"NO ");

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
printf("checking for hrow %s\n",Key.c_str());
					while( pActiveData->m_pvectMenuItem && (pos2=value.find("{HROW}"))!=string::npos && pActiveData->m_iCurrentMenuLine<(int) pActiveData->m_pvectMenuItem->size() )
					{
						MOMenuItem *mi = (*pActiveData->m_pvectMenuItem)[pActiveData->m_iCurrentMenuLine];
printf("found hrow %s %d\n",Key.c_str(),pActiveData->m_MenuType);
						if( pActiveData->m_MenuType==MENUITEM_FILELIST )
						{
							pMsg->m_ID=ACTION_NAV_GOTO_CONST;
							pMsg->m_dwPK_Device_To=m_pCmd->m_dwPK_Device;
							pMsg->m_mapParameters[C_ACTIONPARAMETER_PKID_OBJECT_HEADER_CONST]="-1";
							pMsg->m_mapParameters[C_ACTIONPARAMETER_PATH_CONST]=mi->m_sValue;
							bAbortLoop=true;
							break;
						}
						value.replace(pos2,5,mi->m_sValue);
					}
printf("no hrow\n");
					pMsg->m_mapParameters[atoi(tok.c_str())] = value; 
				}
				if( msgTo==m_pCmd->m_dwPK_Device )
				{
					// It's for us anyway
printf("it's for us %d\n",msgID);
					switch(msgID)
					{
					case ACTION_EXECUTE_CELL_CONST:
printf("execute 1: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						{
							// See if we were passed in the row directly
							string sRowID = pMsg->m_mapParameters[C_ACTIONPARAMETER_DG_ROW_ID_CONST];
							if( sRowID.length()>0 )
								pActiveData->m_iCurrentMenuLine = atoi(sRowID.c_str()) + pActiveData->m_iFirstMenuLine;
						}
printf("execute 2: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_iCurrentMenuLine>=0 && pActiveData->m_iCurrentMenuLine+pActiveData->m_iFirstMenuLine<(int) pActiveData->m_pvectMenuItem->size() )
						{
							MOMenuItem *mi =(*pActiveData->m_pvectMenuItem)[pActiveData->m_iCurrentMenuLine+pActiveData->m_iFirstMenuLine];
							if( !mi->m_pMessage )
								return; // Nothing to do??

							// See if there's special handling required
							int ID = atoi(pMsg->m_mapParameters[C_ACTIONPARAMETER_ID_CONST].c_str());
							if( ID==1 )
							{
								// It's an 'add to the queue' button
								mi->m_pMessage->m_mapParameters[C_ACTIONPARAMETER_FILE_NAME_CONST] = "+" + mi->m_pMessage->m_mapParameters[C_ACTIONPARAMETER_FILE_NAME_CONST];
							}
							else if( ID==2 )
							{
								// It's a 'preview' button
								mi->m_pMessage->m_mapParameters[C_ACTIONPARAMETER_FILE_NAME_CONST] = "*" + mi->m_pMessage->m_mapParameters[C_ACTIONPARAMETER_FILE_NAME_CONST];
							}

							Message *pSendMessage = new Message(mi->m_pMessage);
							m_pCmd->GetEvents()->SendMessage(pSendMessage);
						}
						break;
					case ACTION_SEEK_DATA_GRID_CONST:
printf("before seek: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						m_pCmd->HandleScrollSeek(pActiveData,atoi(pMsg->m_mapParameters[C_ACTIONPARAMETER_TEXT_CONST].c_str()));
printf("after seek: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_pvectMenuItem->size()>0 )
							pActiveData->m_iCurrentMenuLine = 0;
						else
							pActiveData->m_iCurrentMenuLine = -1;
						pActiveData->m_TokenMap["CI"]=pActiveData->m_sCurrentInput;
						break;
					case ACTION_GEN_MOVE_UP_CONST:
					case ACTION_MOVE_GRID_UP_CONST:
						--pActiveData->m_iCurrentMenuLine;
printf("before move up - first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( msgID==ACTION_MOVE_GRID_UP_CONST || pActiveData->m_iCurrentMenuLine<0 )
						{
							pActiveData->m_iCurrentMenuLine=0;
							pActiveData->m_iFirstMenuLine = pActiveData->m_iFirstMenuLine - (pActiveData->m_iLastMenuLine-pActiveData->m_iFirstMenuLine);
							if( pActiveData->m_iFirstMenuLine<0 )
								pActiveData->m_iFirstMenuLine=0;
printf("after move up first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						}
						break;
					case ACTION_GEN_MOVE_DOWN_CONST:
					case ACTION_MOVE_GRID_DOWN_CONST:
printf("before down first menu line: %d current line: %d size: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iCurrentMenuLine,(int) pActiveData->m_pvectMenuItem->size());
						if( pActiveData->m_iFirstMenuLine + pActiveData->m_iCurrentMenuLine < (int) pActiveData->m_pvectMenuItem->size() )
						{
							++pActiveData->m_iCurrentMenuLine;
printf("during down menu type: %d last: %d\n",pActiveData->m_iFirstMenuLine,pActiveData->m_iLastMenuLine );
							if( msgID==ACTION_MOVE_GRID_DOWN_CONST || pActiveData->m_iCurrentMenuLine>pActiveData->m_iLastMenuLine )
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
					m_pCmd->ProcessMenu(pActiveData);
				}
				else
					m_pCmd->GetEvents()->SendMessage(pMsg);
			}
		}	
	}		
}


bool MobileOrbiterSocket::WriteImageToPhone(char *Image, int ImageSize)
{
	unsigned long bytesWritten = 0;

	if (Image == NULL)
	{
		int nil = 0;
		WriteFile(m_CommHandle, (char *)&nil, 2, &bytesWritten);
		if (bytesWritten < 2)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Phone lost connection trying to write empty image result.");
			return false;
		}
		return true;	
	}
	int PaddedSize = (ImageSize / 4 + 3) * 4;
	char *Buf = new char[PaddedSize];
	memset(Buf, 0, PaddedSize);
	memcpy(Buf, Image, ImageSize);
	
	printf("Img Size %d, Padded Size %d", ImageSize, PaddedSize);
	
	WriteFile(m_CommHandle, (char *)&PaddedSize, 2, &bytesWritten); 
	if (bytesWritten < 2)
	{
		g_pPlutoLogger->Write(LV_STATUS,"Couldn't write size to phone (errno=%d)\r\n", GetLastError());
		return false;
	} 
	WriteFile(m_CommHandle, Buf, PaddedSize, &bytesWritten);
	if ((int)bytesWritten < PaddedSize)
	{
		g_pPlutoLogger->Write(LV_STATUS,"Couldn't write image to phone (errno=%d)\r\n", GetLastError());
		return false;
	}
	return true;
}
*/
