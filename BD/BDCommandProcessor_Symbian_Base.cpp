#include "BD/BDCommandProcessor.h"
#include "MyString.h"

#include "PlutoBTApp.h"
#include "PlutoBTAppUi.h"

//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::BDCommandProcessor_Symbian_Base
	(
		const string& sMacAddressPhone,
		MBluetoothListener* aListener
	) 
	: 
		BDCommandProcessor(sMacAddressPhone), 
		CActive(EPriorityStandard), 
		iListener(aListener),
		iVmcBuf(NULL)
{ 
	CActiveScheduler::Add(this);
}
//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::~BDCommandProcessor_Symbian_Base()
{
	Cancel();

	CloseAllResources();

	delete iCommandTimer;
	delete iWaitDialog;
	delete iProgressDialog;

	iSocketServ.Close();
	iNotifier.Close();
}
//----------------------------------------------------------------------------------------------
bool  BDCommandProcessor_Symbian_Base::SendData(int size, const char *data) 
{
	if (iConnected) 
	{
		HBufC8* pStr = HBufC8::NewL(size);
		TPtr8 str = pStr->Des();
		
		for (int i = 0; i < size; i++)
			str.Append(TUint8(data[i]));

		iSocket.Write(str, iStatus);
		User::WaitForRequest(iStatus);

		delete pStr;
		pStr = NULL;

		if(KErrNone != iStatus.Int())
			return EFalse;

		return ETrue;
	}
	else
		return EFalse;
}
//----------------------------------------------------------------------------------------------
char *BDCommandProcessor_Symbian_Base::ReceiveData(int size) 
{
	if (iConnected) 
	{
		char *buffer = (char *)malloc(size);

		pReceivedStr = HBufC8::NewL(size);// < 6000 ? size : 6000);
		TPtr8 str = pReceivedStr->Des();

		TSockXfrLength aLen;

		int bytes_received = 0;

		while(bytes_received < size)
		{
			//iSocket.Recv(str, 0, iStatus, aLen);  
			iSocket.RecvOneOrMore(str, 0, iStatus, aLen);  //it works with BT_Dongle under windows
			User::WaitForRequest(iStatus);

			int len = str.Length();
			for (int i = 0; i < len; i++) 
				buffer[bytes_received + i] = (char)str[i];

			bytes_received += len;

			str.Zero();
		}

		iState = EAccepting; //all ok
 
		delete pReceivedStr;
		pReceivedStr = NULL;

		return buffer;
	}
	else
		return NULL;

}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::Listen() 
{
	iPendingKey = 0;
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
	TBTServiceSecurity serviceSecurity( KUidPlutoBT, KSolBtRFCOMM, 0 );

	//Define security requirements
	serviceSecurity.SetAuthentication(false);    
	serviceSecurity.SetEncryption(false); 
	serviceSecurity.SetAuthorisation(false);

	serviceSecurity.SetChannelID(KPlutoBTPort );
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
	 portNumber[0] = (TUint8)KPlutoBTPort ;
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
void BDCommandProcessor_Symbian_Base::RequestVmcFile()
{
	if (!iConnected)
	{
		iState = EIdle;
		return;
	}
	iPacketBuf.Copy(_L8("VMC"));
	iSocket.Write(iPacketBuf, iStatus);
	SetActive();
	iState = EWritingVmcRequest; 
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::PressedKey(TInt aKey)
{
	if (!iConnected)
		return;
	if (iState != EIdle)
	{
		iPendingKey = aKey;
		return;
	}
	iPacketBuf.Copy(_L8("KY  "));
	iPacketBuf[2] = (TInt8)( aKey % 256 );
	iPacketBuf[3] = (TInt8)( aKey / 256 );

	iSocket.Write(iPacketBuf, iStatus);
	SetActive();
	iState = EWritingKey; 
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::RunL() 
{
	if (iStatus != KErrNone)
	{
		iConnected = false;
		iSocket.Close();
		iListener->Hide();
		User::LeaveIfError( iSocket.Open( iSocketServ ) );
		iListeningSocket.Accept( iSocket, iStatus );
		iState = EAccepting;
		SetActive();
		return;
	}

	switch (iState)
	{
	case EIdle:
		SetActive();
		break;
	case EAccepting:
		iConnected = true;
		
		if (iCommandTimer->IsActive())
		{
			iCommandTimer->Cancel();

			delete iCommandTimer;
			iCommandTimer = NULL;

			iCommandTimer = CPeriodic::NewL(EPriorityStandard);
		}

		iCommandTimer->Start(100000, 1 * 10000000,
			TCallBack(CommandTimerCallBack, this)); 

		break;


	default:
		{
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
void BDCommandProcessor_Symbian_Base::ProcessCommands(BDCommandProcessor_Symbian_Base* pProcessor)
{
	if(pProcessor->iConnected && pProcessor->iState == EAccepting)
	{
		pProcessor->iState = EIdle;
		
		do
		{
			bool bImmediateCallback;
			while( pProcessor->SendCommand(bImmediateCallback) && bImmediateCallback);
		}
		while(pProcessor->m_listCommands.Count());

		pProcessor->iState = EAccepting;
	}
}
//----------------------------------------------------------------------------------------------
TInt BDCommandProcessor_Symbian_Base::CommandTimerCallBack(TAny* aBluetoothHandler)
{
	BDCommandProcessor_Symbian_Base* pProcessor = 
		(BDCommandProcessor_Symbian_Base*)aBluetoothHandler;

	pProcessor->ProcessCommands(pProcessor);

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
	switch (iState)
	{
		case EAccepting:
			iListeningSocket.CancelAccept();
			break;
		case EReadingVmcSize:
		case EReadingVmc:
			iSocket.CancelRead();
			break;
		case EWritingVmcRequest:
		case EWritingKey:
			iSocket.CancelWrite();
			break;
		default:
			break;
	}
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::CloseAllResources()
{
	iState = EIdle;
	iConnected = EFalse;

	if (iCommandTimer)
		iCommandTimer->Cancel();

	if (iWaitDialog)
		iWaitDialog->ProcessFinishedL();

	if (iProgressDialog)
		iProgressDialog->ProcessFinishedL();

    iSdpDatabase.DeleteRecordL(iRecord);
    iRecord = 0;

	iSdpDatabase.Close();
    iSdpSession.Close();
	iSocket.Close();
}
//----------------------------------------------------------------------------------------------
