#ifndef VR_RequestPayment_H
#define VR_RequestPayment_H

#include "RA/RA_Request.h"

class InvoiceDetail
{
public:
	string m_sLineID,m_sProductID,m_sDescription;
	long m_iAmount;
	InvoiceDetail() {};
	InvoiceDetail(string LineID,string ProductID,string Description,long Amount) { 
		m_sLineID=LineID; m_sProductID=ProductID; m_sDescription=Description; m_iAmount=Amount; 
	}

};

class VR_RequestPayment : public RA_Request
{
public:
	// Request Variables
	long m_iAmount;
	string m_sInvoiceNumber;
	unsigned long m_FKID_PlutoId_Establishment,m_FKID_PlutoId_User;
	u_int64_t m_iMacAddress;  // The phone that this will go to
	string m_sDescription,m_sCashierName;
	MYSTL_CREATE_LIST(m_listInvoiceDetail,InvoiceDetail);

	// Response Variables
	short m_iPaymentResponse;
	unsigned long m_iTransactionNumber;
	string m_sApprovalCode,m_sCardNumber;

	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_RequestPayment(u_int64_t iMacAddress,long Amount,string InvoiceNumber,
		string Description,string CashierName,unsigned long FKID_PlutoId_Establishment,
		unsigned long FKID_PlutoId_User);

	// The server will call this constructor, then ProcessRequest
	VR_RequestPayment(unsigned long size,const char *data);


	virtual unsigned long ID() { return VRS_REQUEST_PAYMENT; }
	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif

