#ifndef __BDCommandProcessor_Symbian_Base_h
#define __BDCommandProcessor_Symbian_Base_h

#include <es_sock.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <btmanclient.h>
#include <btsdp.h>
#include <eikprogi.h>
#include <AknWaitDialog.h>
#include <AknProgressDialog.h>


#include "BDCommandProcessor.h"
#include "VipMenu.h"
//----------------------------------------------------------------------------------------------
#define KServiceName _L8("Pluto")
#define KServiceDescription _L8("PlutoInc")

const TInt KPlutoBTPort = 19;
const TInt KSerialPortService = 0x1101;

_LIT( KServerTransportName, "RFCOMM" );
static const TInt KListeningQueSize = 4;
//----------------------------------------------------------------------------------------------
class MBluetoothListener
{
public:
	virtual void UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
								 VIPMenuCollection *pVMC) = 0;
	virtual void UpdateScreen(VIPMenuCollection *pVMC) = 0;
	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual CCoeEnv* CoeEnv() = 0;
};
//----------------------------------------------------------------------------------------------
class BDCommandProcessor_Symbian_Base:public BDCommandProcessor, public CActive 
{

public:

	enum TBluetoothState 
	{
		EIdle,
		EAccepting,
		EWritingVmcRequest,
		EReadingVmcSize,
		EReadingVmc,
		EWritingKey

	};

	virtual void Connect();

public: // New functions.

	virtual void RequestVmcFile();
	virtual void PressedKey(TInt aKey);

private: // Inherited from CActive.
	
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private: // New functions.

	virtual void CloseAllResources();

	static TInt CommandTimerCallBack(TAny* aBluetoothHandler);

protected:

	TInt iPendingKey;

	RSocketServ iSocketServ;
	RSocket iListeningSocket;
	RSocket iSocket;

	TBluetoothState iState;
	RNotifier iNotifier;

	TBTDeviceResponseParamsPckg iDeviceSelectionPckg;
	TBTDeviceResponseParamsPckg iDeviceFoundPckg;

	TSockXfrLength iXfrLength;
	HBufC8 *iVmcBuf;
	TBuf8<2> iVmcSizeBuf;
	TBuf8<1024> iPacketBuf;
	TBuf8<2048> iTotalPacketBuf;

	TInt iSdpCurrentRecordCount;
	TInt iSdpTotalRecordCount;
	RSdp iSdpSession;
	RSdpDatabase iSdpDatabase;
	TSdpServRecordHandle iRecord;
	TSdpServRecordHandle iRecord2;
	TBool iIsConnected;

	CAknWaitDialog* iWaitDialog;
	CAknProgressDialog* iProgressDialog;

	CEikProgressInfo* iProgressInfo;

	TUUID iServiceClass;
	TUint iRemotePort;
	TBool iConnected;

	TInt iVmcLength;
	CPeriodic* iCommandTimer;
	TNameEntry iNameEntry;

	MBluetoothListener* iListener;

	HBufC8* pReceivedStr;

public:

	BDCommandProcessor_Symbian_Base(const string& sMacAddressPhone, MBluetoothListener* aListener);
	virtual ~BDCommandProcessor_Symbian_Base();

	virtual void Start() = 0;
	virtual void Listen();
	virtual void SetupSecurityManager();
	virtual void AdvertiseThePlutoService();

	virtual bool SendData(int size,const char *data);
	virtual char *ReceiveData(int size);
	virtual void Close();

	void ProcessCommands(BDCommandProcessor_Symbian_Base* pProcessor);
};
//----------------------------------------------------------------------------------------------
#endif
