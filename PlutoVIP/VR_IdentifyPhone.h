#ifndef VR_IdentifyPhone_H
#define VR_IdentifyPhone_H

#include "RA/RA_Request.h"
#include "VIPIncludes.h"

class VR_IdentifyPhone : public RA_Request
{
public:
	// Request Variables
	string m_sBluetoothID,m_sIdentifiedPlutoIdPin;
	u_int64_t m_iMacAddress;
	unsigned long m_iCachedRecordVersion,m_iCachedPlutoId,m_iIdentifiedPlutoId;
	unsigned long m_iEstablishmentID;

	// Response Variables
	unsigned char m_iUseCache;
	unsigned long m_iRecordVersion,m_iPlutoId;
	unsigned char m_iImageType;
	unsigned long m_iImageSize;
	char *m_pImage;
	string m_sEmail,m_sNickname,m_sFirstName,m_sLastName;
	string m_sAddress,m_sCity,m_sState,m_sZip,m_sCountry;
	string m_sGender,m_sBirthdate;
	string m_sComments,m_sEstablishmentCustomerId;
	MYSTL_CREATE_LIST(m_listPhoneNumbers,VIPPhoneNumber);
	MYSTL_CREATE_LIST(m_listAttributes,MiscVIPAttribute);

	// Other
	class Customer *m_pCustomer; // Used if we get back actions to send to the customer

	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_IdentifyPhone(
		unsigned long EstablishmentID,
		string sBluetooth,
		u_int64_t MacAddress,
		unsigned long CachedRecordVersion,
		unsigned long CachedPlutoId,
		unsigned long IdentifiedPlutoId,
		string IdentiedPlutoIdPin,
		class Customer *pCustomer
	);

	// The server will call this constructor, then ProcessRequest
	VR_IdentifyPhone(unsigned long size,const char *data);

	virtual void LogVisit();

	virtual unsigned long ID() { return VRS_IDENFITY_PHONE; }

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif

