#include "BDCommandProcessor_Symbian_Base.h"
#include "PlutoUtils/MyString.h"

#include "BDCommand.h"
#include "PlutoPhoneCommands.h"
#include "BD_HaveNothing.h"
#include "BD_WhatDoYouHave.h"

#include "PlutoMOApp.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::GotoStage(TBluetoothState state)
{
	iState = state;

	GotoNextStage();
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::GotoNextStage()
{
	LOG("@GotoNext@:\n");
	TRequestStatus* s = &iStatus; 
	User::RequestComplete(s, KErrNone); 
}
//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::BDCommandProcessor_Symbian_Base
	(
		const string& sMacAddressPhone,
		MBluetoothListener* aListener
	) 
	: 
		CActive(EPriorityStandard), 
		iListener(aListener)
{ 
	//iTimer.CreateLocal(); // created for this thread
	iPendingKey = false;

	m_bDead = false;
//	m_bExit = false;

	m_ReceiveCmdData=NULL;
	m_ReceiveAckData=NULL;
	m_ReceiveCmdHeader=NULL;
	m_ReceiveAckHeader=NULL;

	m_HBuf_ReceiveCmdData=NULL;
	m_HBuf_ReceiveAckData=NULL;
	m_HBuf_ReceiveCmdHeader=NULL;
	m_HBuf_ReceiveAckHeader=NULL;

	m_pCommand = NULL;
	m_pCommand_Sent=NULL;
	m_sMacAddressPhone=sMacAddressPhone;

	m_Recv_iBuf = NULL;

	CActiveScheduler::Add(this);
}
//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::~BDCommandProcessor_Symbian_Base()
{
	LOG("Canceling timer...\n");

	if (iCommandTimer)
		iCommandTimer->Cancel();

	LOG("Deleting timer...\n");
	delete iCommandTimer;
	iCommandTimer = NULL;

	LOG("Canceling active object request...\n");
	Cancel();

	LOG("Canceling socket requests...\n");
	iSocket.CancelAll();

	iState = EIdle;
	iConnected = EFalse;

	LOG("Closing sdp stuffs...\n");
	iSdpDatabase.Close();
    iSdpSession.Close();

	LOG("Closing socket server...\n");
	iSocketServ.Close();

	LOG("Ok all... exiting destruction... \n");
}
//----------------------------------------------------------------------------------------------
bool  BDCommandProcessor_Symbian_Base::SendData(int size, const char *data) 
{
	if(size)
	{
		iSocket.CancelWrite(); //flush the write internal buffer. symbian RSocket bug ?

		HBufC8* pStr = HBufC8::NewL(size);
		TPtr8 str = pStr->Des();
		
		for (int i = 0; i < size; i++)
			str.Append(TUint8(data[i]));

		iSocket.Write(str, iStatus);

		LOG(">>> Sending: ");
		LOGN(size);
		LOGN(" bytes\n");

		delete pStr;
		pStr = NULL;
	}
	else //nothing to send ... just go to the next stage
	{
		GotoNextStage();
	}

	return true;
}
//----------------------------------------------------------------------------------------------
bool BDCommandProcessor_Symbian_Base::SendLong(long l)
{
	SendData(4,(const char *)&l);

	return true;
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::Listen() 
{
//	iPendingKey = 0;
	iCommandTimer = CPeriodic::NewL(EPriorityStandard);

	User::LeaveIfError( iListeningSocket.Listen( KListeningQueSize ) );
	iListeningSocket.Accept( iSocket, iStatus );
	iState = EAccepting;

	SetActive();
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::SetupSecurityManager()
{
	// a connection to the security manager
	RBTMan secManager;

	// a security session
	RBTSecuritySettings secSettingsSession;

	// define the security on this port
	User::LeaveIfError(secManager.Connect());
	CleanupClosePushL(secManager);
	User::LeaveIfError(secSettingsSession.Open(secManager));
	CleanupClosePushL(secSettingsSession);

	// the security settings 
	TBTServiceSecurity serviceSecurity( KUidPlutoMO, KSolBtRFCOMM, 0 );

	//Define security requirements
	serviceSecurity.SetAuthentication(false);    
	serviceSecurity.SetEncryption(false); 
	serviceSecurity.SetAuthorisation(false);

	serviceSecurity.SetChannelID(KPlutoMOPort );
	TRequestStatus status;
	secSettingsSession.RegisterService(serviceSecurity, status);

	User::WaitForRequest( status ); // wait until the security settings are set
	User::LeaveIfError( status.Int() );

	CleanupStack::PopAndDestroy();  //  secManager
	CleanupStack::PopAndDestroy();  //  secSettingsSession

}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::AdvertiseThePlutoService()
{
	 TSdpServRecordHandle recHandle;
	 CSdpAttrValue* attrVal = 0;
	 CSdpAttrValueDES* attrValDES = 0;
 
	 // Set Attr 1 (service class list) to list with UUID = 0x1101 (serial port)
	 TUUID sppUid = TUUID(TUint32(0x1101));
	 iSdpDatabase.CreateServiceRecordL(sppUid, recHandle);
 
	 // Set Attr 2 (service record state) to 0
	 TBuf8<4> recordState;
	 recordState.FillZ(4);
	 attrVal = CSdpAttrValueUint::NewUintL(recordState);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceRecordState, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;


	 // the port number in the protocol should be a TUint8, not a 4 byte UInt 
	 TBuf8<1> portNumber;
	 portNumber.SetLength(1);
	 portNumber[0] = (TUint8)KPlutoMOPort ;
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildDESL()
	   ->StartListL()
		->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
	   ->EndListL()
	   ->BuildDESL()
	   ->StartListL()
		->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
		->BuildUintL(portNumber) // the port number
	   ->EndListL()
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 
	 // Set Attr 5 (browse group list) to list with one UUID 0x1002 (public browse group)
	 //
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildUUIDL(TUUID(TUint32(0x1002))) // Public browse group
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 

	 // Set Attr 0x006 (language base)
	 TBuf8<2> langId;
	 langId.FillZ(2);
	 langId[0] = 0x65;
	 langId[1] = 0x6e;
 
	 TBuf8<2> charEnc;
	 charEnc.FillZ(2);
	 charEnc[1] = 0x6a;
 
	 TBuf8<2> baseAttrId;
	 baseAttrId.FillZ(2);
	 baseAttrId[0] = 0x01;
 
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildUintL(langId) // english
	   ->BuildUintL(charEnc) // UTF-8
	   ->BuildUintL(baseAttrId) // language base
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 
	 // Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	 TBuf8<4> timeToLive;
	 timeToLive.FillZ(4);
	 timeToLive[2]=4;
	 timeToLive[3]=0xb0;
	 attrVal = CSdpAttrValueUint::NewUintL(timeToLive);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceInfoTimeToLive, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x008 (availability) to 0xff - fully available
	 TBuf8<1> avail;
	 avail.FillZ(1);
	 avail[0]=0xff;
	 attrVal = CSdpAttrValueUint::NewUintL(avail);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceAvailability, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x100 (default Name) to string
	 attrVal = CSdpAttrValueString::NewStringL(KServiceName);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceName, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x101 (def. description) to string
	 attrVal = CSdpAttrValueString::NewStringL(KServiceName);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceDescription, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;

	 
	 // Set Attr 0x102 (def. provider) to Intuwave
	 attrVal = CSdpAttrValueString::NewStringL(KServiceDescription);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, 0x0102, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::RunL() 
{
	if (iStatus != KErrNone || iState == EConnectionLost)
	{
		LOG("Disconnected! [iStatus != KErrNone || iState == EConnectionLost]\n");

		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bMakeVisibleAllowed = false;
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->ResetViewer();

		iConnected = false;
		//iSocket.CancelAll();
		iSocket.Close();

		User::LeaveIfError( iSocket.Open( iSocketServ ) );
		iListeningSocket.Accept( iSocket, iStatus );
		iState = EAccepting;
		SetActive();

		iListener->Hide();

		LOG("Waiting for new connections...\n");
		return;
	}

	if(iPendingKey)
	{
		iPendingKey = false;
		iState = ESendingCommand;
	}

	m_bStatusOk = true;

	switch (iState)
	{
	case EAccepting:
		{
			LOG("EAccepting\n");
			iConnected = true;

			if (iCommandTimer->IsActive())
			{
				iCommandTimer->Cancel();

				delete iCommandTimer;
				iCommandTimer = NULL;

				iCommandTimer = CPeriodic::NewL(EPriorityIdle);
			}

			iCommandTimer->Start(100000, 5 * 1000000,
				TCallBack(CommandTimerCallBack, this)); 

			iState = EIdle;
		}
		break;

	case ESendingCommand:
		{
			LOG("ESendingCommand\n");
			m_bImmediateCallback = false;

			if( MYSTL_SIZEOF_LIST(m_listCommands)==0 )
				m_pCommand_Sent = new BD_WhatDoYouHave();
			else
			{
				MYSTL_EXTRACT_FIRST(m_listCommands, BDCommand, pC);
				m_pCommand_Sent = pC;
			}

			m_pCommand_Sent->ConvertCommandToBinary();
			SendLong(m_pCommand_Sent->ID());

			iState = ESendingCommandId;
			SetActive();
		}
		break;

	case ESendingCommandId:
		{
			LOG("ESendingCommandId\n");
			SendLong(m_pCommand_Sent->GetCommandOrAckSize());
			iState = ESendingCommandOrAckSize;
			SetActive();
		}
		break;

	case ESendingCommandOrAckSize:
		{
			LOG("ESendingCommandOrAckSize\n");
			SendData(m_pCommand_Sent->GetCommandOrAckSize(),m_pCommand_Sent->GetCommandOrAckData());
			iState = ESendingCommandOrAckData;
			SetActive();
		}
		break;

	case ESendingCommandOrAckData:
		{
			LOG("ESendingCommandOrAckData\n");
			int AckHeaderSize;
			if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
				AckHeaderSize = 8; 
			else
				AckHeaderSize = 4;

			if(NULL != m_HBuf_ReceiveAckHeader)
			{
				delete m_HBuf_ReceiveAckHeader;
				m_HBuf_ReceiveAckHeader = NULL;
			}

			if(NULL != m_ReceiveAckHeader)
			{
				delete m_ReceiveAckHeader;
				m_ReceiveAckHeader = NULL;
			}

			if(NULL != m_Recv_iBuf)
			{
				delete m_Recv_iBuf;
				m_Recv_iBuf = NULL;
			}

			m_iRecvSize = AckHeaderSize;
			m_HBuf_ReceiveAckHeader = HBufC8::NewL(m_iRecvSize);
			
			m_Recv_iBuf = new TPtr8(NULL, 0); 
			m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveAckHeader->Ptr(), 0, m_iRecvSize); 
			iSocket.Read(*m_Recv_iBuf, iStatus);

			iState = EReceivingAckHeader;
			m_pCommand_Sent->FreeSerializeMemory();

			SetActive();
		}
		break;

	case EReceivingAckHeader:
		{
			LOG("EReceivingAckHeader\n");
			
			m_ReceiveAckHeader = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveAckHeader[i] = (char)(*m_Recv_iBuf)[i];

			LOG("<<< Received: ");
			LOGN(m_iRecvSize);
			LOGN(" bytes\n");

			long *lSize;
			if( m_pCommand_Sent->ID()==BD_PC_WHAT_DO_YOU_HAVE )
				lSize = (long *) (m_ReceiveAckHeader + 4);
			else
				lSize = (long *) (m_ReceiveAckHeader);

			if( *lSize )
			{
				if(NULL != m_HBuf_ReceiveAckData)
				{
					delete m_HBuf_ReceiveAckData;
					m_HBuf_ReceiveAckData = NULL;
				}

				if(NULL != m_ReceiveAckData)
				{
					delete m_ReceiveAckData;
					m_ReceiveAckData = NULL;
				}

				m_iRecvSize = *lSize;
				m_bStartRecv = true;

				GotoStage(EReceivingAckData_Loop);
			}
			else
				GotoStage(EReceivingAckData);

			SetActive();
		}
		break;


	case EReceivingAckData_Loop:
		{
			LOG("EReceivingAckData_Loop\n");
			if(m_bStartRecv)
			{
				m_bStartRecv = false;

				if(m_HBuf_ReceiveAckData)
				{
					delete m_HBuf_ReceiveAckData;
					m_HBuf_ReceiveAckData = NULL;
				}

				if(m_ReceiveAckData)
				{
					delete m_ReceiveAckData;
					m_ReceiveAckData = NULL;
				}

				if(NULL != m_Recv_iBuf)
				{
					delete m_Recv_iBuf;
					m_Recv_iBuf = NULL;
				}

				m_HBuf_ReceiveAckData = HBufC8::NewL(m_iRecvSize);
				m_ReceiveAckData = new char[m_iRecvSize];
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveAckData->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingAckData_Loop;
			}
			else
			{
				for (int i = 0; i < m_iRecvSize; i++) 
					m_ReceiveAckData[i] = (char)(*m_Recv_iBuf)[i];

				LOG("<<< Received: ");
				LOGN(m_iRecvSize);
				LOGN(" bytes\n");

				GotoStage(EReceivingAckData);
			}

			SetActive();
		}
		break;

	case EReceivingAckData:
		{
			LOG("EReceivingAckData\n");
			if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
			{
				iStatus = KErrNone;
				iState = EReceivingCommand;

				//no break, continue
			}
			else
			{
				long *lSize = (long *) (m_ReceiveAckHeader);
				m_pCommand_Sent->ParseAck(*lSize, m_ReceiveAckData);

				delete m_pCommand_Sent;
				m_pCommand_Sent=NULL;

				iState = EIdle;
				break;
			}
		}

	case EReceivingCommand:
		{
			LOG("EReceivingCommand\n");
			// The Ack will be a command that's parsed directly
			long *lType = (long *) m_ReceiveAckHeader;
			long *lSize = (long *) (m_ReceiveAckHeader + 4);

			if(*lType)
			{
				m_pCommand = BuildCommandFromData(*lType);
				m_pCommand->ParseCommand(*lSize, m_ReceiveAckData);
				m_pCommand->ProcessCommand(NULL);
				m_pCommand->ConvertAckToBinary();

				iState = ERecvCommand_ReadyToSendAck;
				//no break, continue
			}
			else
			{
				if(NULL != m_HBuf_ReceiveCmdHeader)
				{
					delete m_HBuf_ReceiveCmdHeader;
					m_HBuf_ReceiveCmdHeader = NULL;
				}
				
				if(NULL != m_ReceiveCmdHeader)
				{
					delete m_ReceiveCmdHeader;
					m_ReceiveCmdHeader = NULL;
				}

				if(NULL != m_Recv_iBuf)
				{
					delete m_Recv_iBuf;
					m_Recv_iBuf = NULL;
				}

				m_iRecvSize = 8;
				m_HBuf_ReceiveCmdHeader = HBufC8::NewL(m_iRecvSize);
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveCmdHeader->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingCmdHeader;
				SetActive();
				break;
			}
		}

	case ERecvCommand_ReadyToSendAck:
		{
			LOG("ERecvCommand_ReadyToSendAck\n");
			SendLong(m_pCommand->GetCommandOrAckSize());
			iState = ERecvCommand_SendingCommandOrAckSize;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckSize:
		{
			LOG("ERecvCommand_SendingCommandOrAckSize\n");
			SendData(m_pCommand->GetCommandOrAckSize(),m_pCommand->GetCommandOrAckData());
			iState = ERecvCommand_SendingCommandOrAckData;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckData:
		{
			LOG("ERecvCommand_SendingCommandOrAckData\n");
			m_pCommand->FreeSerializeMemory();

			delete m_pCommand;
			m_pCommand = NULL;

			iState = ERecvCommand_End;

			//no break, continue
		}

	case ERecvCommand_End:
		{
			LOG("ERecvCommand_End\n");
			long *lType = (long *) m_ReceiveAckHeader;

			if( *lType != BD_CP_HAVE_NOTHING )
			{
				GotoStage(ESendingCommand);
				SetActive();
			}
			else
				iState = EIdle;
		}
		break;

	case EReceivingCmdHeader:
		{
			LOG("EReceivingCmdHeader\n");

			m_ReceiveCmdHeader = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveCmdHeader[i] = (char)(*m_Recv_iBuf)[i];

			LOG("<<< Received: ");
			LOGN(m_iRecvSize);
			LOGN(" bytes\n");
			
			long *lSize = (long *) (m_ReceiveCmdHeader + 4);

			if( *lSize )
			{
				if(NULL != m_HBuf_ReceiveCmdData)
				{
					delete m_HBuf_ReceiveCmdData;
					m_HBuf_ReceiveCmdData = NULL;
				}

				if(NULL != m_ReceiveCmdData)
				{
					delete m_ReceiveCmdData;
					m_ReceiveCmdData = NULL;
				}

				if(NULL != m_Recv_iBuf)
				{
					delete m_Recv_iBuf;
					m_Recv_iBuf = NULL;
				}

				m_iRecvSize = 8;
				m_HBuf_ReceiveCmdData = HBufC8::NewL(m_iRecvSize);
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveCmdData->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingCmdData;
			}
			else
				GotoStage(EReceivingCommand_BuildCommand);

			SetActive();
		}
		break;

	case EReceivingCmdData:
		{
			LOG("EReceivingCmdData\n");

			m_ReceiveCmdData = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveCmdData[i] = (char)(*m_Recv_iBuf)[i];

			LOG("<<< Received: ");
			LOGN(m_iRecvSize);
			LOGN(" bytes\n");

			GotoStage(EReceivingCommand_BuildCommand);
			SetActive();
		}
		break;

	case EReceivingCommand_BuildCommand:
		{
			LOG("EReceivingCommand_BuildCommand\n");
			long *lType = (long *) m_ReceiveCmdHeader;
			long *lSize = (long *) (m_ReceiveCmdHeader+4);
			m_pCommand = BuildCommandFromData(*lType);
			if( m_pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE )
			{
				iState = ESendingCommand;
			}
			else
			{
				m_pCommand->ParseCommand(*lSize,m_ReceiveCmdData);
				m_pCommand->ProcessCommand(NULL);
				m_pCommand->ConvertAckToBinary();

				SendLong(m_pCommand->GetCommandOrAckSize());
				iState = ERecvCommand_SendingCommandOrAckSize_Step2;
			}
			
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckSize_Step2:
		{
			LOG("ERecvCommand_SendingCommandOrAckSize_Step2\n");
			SendData(m_pCommand->GetCommandOrAckSize(), m_pCommand->GetCommandOrAckData());
			iState = ERecvCommand_SendingCommandOrAckData_Step2;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckData_Step2:
		{
			LOG("ERecvCommand_SendingCommandOrAckData_Step2\n");
			m_pCommand->FreeSerializeMemory();
			iState = ERecvCommand_End;

			long *lType = (long *) m_ReceiveCmdHeader;

			if( *lType != BD_CP_HAVE_NOTHING )
			{
				GotoStage(ESendingCommand);
				SetActive();
			}
			else
				iState = EIdle;
		}

	default:
		{
		LOG("Unexpected Bluetooth state\n");
		TBuf<256> formatBuf;
		formatBuf.Format(_L("Unexpected Bluetooth state %d"), iState);
		CEikonEnv::Static()->InfoWinL(KNullDesC, formatBuf);
		}
		break;
		}

}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::Close() 
{
/*
	iTransferSocket.Close();
	iListenSocket.Close();
	iSocketServ.Close();
*/
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::ProcessCommands(bool bCriticalRequest /*=true*/)
{
	if(iConnected)
	{
		if(!m_bStatusOk)
		{
			//it's seems that in last x seconds, we had only EIdle state
			//connection lost!
			iSocket.CancelAll();
			iState = EConnectionLost;
			return;
		}

		if(bCriticalRequest && IsActive()) //the object is already active.
		{
			//GotoStage(ESendingCommand); //don't force object activation
			iPendingKey = true;
			return;
		}

		if(iState == EIdle || bCriticalRequest)
		{
			GotoStage(ESendingCommand);
			SetActive();
			return;
		}

		//on timer tick, we found an EIdle state. what if the connection was lost?
		m_bStatusOk = false;
	}
}
//----------------------------------------------------------------------------------------------
TInt BDCommandProcessor_Symbian_Base::CommandTimerCallBack(TAny* aBluetoothHandler)
{
	BDCommandProcessor_Symbian_Base* pProcessor = 
		(BDCommandProcessor_Symbian_Base*)aBluetoothHandler;

	pProcessor->ProcessCommands(false);

	return 0;
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::Connect()
{
	SetActive();
}
//----------------------------------------------------------------------------------------------
TInt BDCommandProcessor_Symbian_Base::RunError(TInt aError)
{
	TBuf<256> formatBuf;
	if (aError != KErrNone)
	{
		formatBuf.Format(_L("Unexpected error %d in state %d"), aError, iState);
		CEikonEnv::Static()->InfoWinL(formatBuf, KNullDesC);
	}
	return KErrNone;
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::DoCancel()
{
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::AddCommand(class BDCommand *pCommand)
{
	MYSTL_ADDTO_LIST(m_listCommands,pCommand);
}
//----------------------------------------------------------------------------------------------