//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "RubySerialWrapper.h"

namespace DCE {

	RubySerialWrapper::RubySerialWrapper()
		: pdce_(NULL)	
	{
	}

	RubySerialWrapper::~RubySerialWrapper()
	{
	}

	void RubySerialWrapper::SendCommand(RubyCommandWrapper* pcmd) 
	{
		if(pdce_) {
			pdce_->SendCommand(pcmd);
		}
	}

	void RubySerialWrapper::DisableDevice( int device, bool bDisable ) 
	{
		if( pdce_ != NULL /*&& device_ != NULL*/ ) {

			// 791 = PK_Command for disable device
			RubyCommandWrapper *pcmd = new RubyCommandWrapper( device_.getDevId(), 0, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Set_Enable_Status_CONST );

			pcmd->setTemplate( DEVICETEMPLATE_General_Info_Plugin_CONST );

			pcmd->params_[COMMANDPARAMETER_PK_Device_CONST] = StringUtils::itos( device );

			if ( bDisable )
				pcmd->params_[COMMANDPARAMETER_Enable_CONST] = "0";
			else pcmd->params_[COMMANDPARAMETER_Enable_CONST] = "1";
			
			pdce_->SendCommand(pcmd);

			delete pcmd;
		}

	}

	void RubySerialWrapper::SetDeviceDataInDB( int device, int PK_DeviceData, string value )
	{
		if(pdce_) {

			RubyCommandWrapper *pcmd = new RubyCommandWrapper( device_.getDevId(), 
				DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_CHANGE, 0 );

			pcmd->params_[PK_DeviceData] = value;

			pdce_->SendCommand(pcmd);

			delete pcmd;

		}
	}

	string RubySerialWrapper::GetCurrentDeviceData( int PK_Device, int PK_DeviceData )
	{
		if(pdce_) {

			// TODO: make sure the "to" and "from" fields are filled corectlly
			// message type is irrelevant, from SendCommandReceiveString it's set this way anyway
			RubyCommandWrapper *pcmd = new RubyCommandWrapper( DEVICEID_DCEROUTER, 
				device_.getDevId(), PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_REQUEST, PK_DeviceData );

			string s_response = pdce_->SendCommandReceiveString(pcmd);

			delete pcmd;
			return s_response;

		}
		// Message *pMessage = new Message(m_dwPK_Device,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_DATAPARM_REQUEST, PK_DeviceData,0);
	}
};
