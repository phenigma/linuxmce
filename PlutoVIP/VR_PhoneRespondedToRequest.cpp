#include "VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "VR_PhoneRespondedToRequest.h"
#include "VA_UpdateTransaction.h"
#include "VR_ShowMenu.h"
#include "VIPMenu.h"
#include <iostream>
#include <sstream>

VR_PhoneRespondedToRequest::VR_PhoneRespondedToRequest(unsigned long RequestID,unsigned long RequestSize,const char *RequestData,unsigned long ResponseSize,const char *ResponseData)
	: RA_Request()
{
	m_iRequestID=RequestID;
	m_iOriginalRequestSize=RequestSize;
	m_pOriginalRequest = new char[m_iOriginalRequestSize];
	memcpy(m_pOriginalRequest,RequestData,m_iOriginalRequestSize);

	m_iOriginalResponseSize=ResponseSize;
	m_pOriginalResponse = new char[m_iOriginalResponseSize];
	memcpy(m_pOriginalResponse,ResponseData,m_iOriginalResponseSize);

}

VR_PhoneRespondedToRequest::VR_PhoneRespondedToRequest(unsigned long size,const char *data) 
	: RA_Request(size,data) 
{
	m_iRequestID = Read_unsigned_long();
	m_iOriginalRequestSize = Read_unsigned_long();
	m_pOriginalRequest = Read_block(m_iOriginalRequestSize);
	m_iOriginalResponseSize = Read_unsigned_long();
	m_pOriginalResponse = Read_block(m_iOriginalResponseSize);

	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
}

void VR_PhoneRespondedToRequest::ConvertRequestToBinary()
{
	RA_Request::ConvertRequestToBinary();

	Write_unsigned_long(m_iRequestID);
	Write_unsigned_long(m_iOriginalRequestSize);
	Write_block(m_pOriginalRequest,m_iOriginalRequestSize);
	Write_unsigned_long(m_iOriginalResponseSize);
	Write_block(m_pOriginalResponse,m_iOriginalResponseSize);

	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcRequest = m_pcDataBlock;
}

void VR_PhoneRespondedToRequest::ConvertResponseToBinary()
{
	RA_Request::ConvertResponseToBinary();
}


bool VR_PhoneRespondedToRequest::ParseResponse(unsigned long size,const char *data)
{
	RA_Request::ParseResponse(size,data);
	return true;
}

bool VR_PhoneRespondedToRequest::ProcessRequest()
{
#ifdef VIPSERVER
	RA_Request *pRequest;
	if( m_iOriginalRequestSize )
		pRequest = RA_Request::BuildRequestFromData(m_iOriginalRequestSize,m_pOriginalRequest);
	else
		pRequest = RA_Request::BuildRequestFromData(0,NULL,m_iRequestID);

	pRequest->ParseResponse(m_iOriginalResponseSize,m_pOriginalResponse);
	if( m_iRequestID==VRP_REQUEST_SHOW_MENU )
	{
		VR_ShowMenu *pShowMenu = (VR_ShowMenu *) pRequest;
		if( pShowMenu->m_iMenuCollectionID==VIPMENUCOLL_SECURE_PAYMENT )
		{
			long Amount=-1;
			long PaymentMethod=-1;
			string PinCode;
			long TransNumber=-1;

			MYSTL_ITERATE_LIST(pShowMenu->m_listReturnedVariables,VIPVariable,pVariable,itListVariables)
			{
				if( pVariable->m_iVariableID==VIPVAR_PAYMENT_AMOUNT_USER )
					Amount=atoi(pVariable->m_sCurrentValue.c_str());
				else if( pVariable->m_iVariableID==VIPVAR_PAYMENT_METHOD )
					PaymentMethod=atoi(pVariable->m_sCurrentValue.c_str());
				else if( pVariable->m_iVariableID==VIPVAR_PIN_CODE )
					PinCode=pVariable->m_sCurrentValue;
				else if( pVariable->m_iVariableID==VIPVAR_VIP_TRANS_NUM )
					TransNumber=atoi(pVariable->m_sCurrentValue.c_str());
			}

			std::ostringstream sql;
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);

			// Confirm we have a record of the transaction
			PlutoSqlResult rsTrans;
			MYSQL_ROW TransRow=NULL;
			sql << "SELECT Amount FROM PmtTrans WHERE PKID_PmtTrans=" << TransNumber;
			if( (rsTrans.r = g_pPlutoConfig->mysql_query_result(sql.str()))==NULL || 
				(TransRow=mysql_fetch_row(rsTrans.r))==NULL )
			{
				ErrorLog << "CANNOT FIND TRANSACTION: " << sql; }
				m_cProcessOutcome = INVALID_REQUEST_DATA;
				return true;
			}

			if( Amount==-1 )
				Amount = atoi(TransRow[0]);

			if( pShowMenu->m_iMenuTermination!=CLOSE_WITH_OK )
			{
				sql.str("");
				sql << "UPDATE PmtTrans SET Result=-3 WHERE PKID_PmtTrans=" << TransNumber;
				g_pPlutoConfig->threaded_mysql_query(sql.str());

				VA_UpdateTransaction *pUP = new VA_UpdateTransaction(TransNumber,-3,"cancelled");
				m_listActions.push_back(pUP);
				m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
			}

			// 0=Not processed, 1=approved, -1=declined, -2=referred, -3=user cancelled
			int Result=1; 
			string ResultStr="28228";

			// Process the transaction
			sql << "INSERT INTO PmtTrans_Auth(FKID_PmtTrans,DateTime,Result,ResultStr) VALUES("
				<< TransNumber << ",'" << StringUtils::SQLDateTime() << "'," << Result << ",'"
				<< StringUtils::SQLEscape(ResultStr) << "');";

			int PKID_PmtTrans_Auth = g_pPlutoConfig->threaded_mysql_query_withID(sql.str());
			if( PKID_PmtTrans_Auth==0 )
			{
				ErrorLog << "FAILED: " << sql; }
				m_cProcessOutcome = INTERNAL_ERROR;
				return true;
			}

			sql.str("");
			sql << "UPDATE PmtTrans SET Result=" << Result << " WHERE PKID_PmtTrans=" << TransNumber;
			g_pPlutoConfig->threaded_mysql_query(sql.str());


			VA_UpdateTransaction *pUP = new VA_UpdateTransaction(TransNumber,Result,ResultStr);
			m_listActions.push_back(pUP);
			m_cProcessOutcome = SUCCESSFULLY_PROCESSED;
		}
	}
#endif
	return true;
}
