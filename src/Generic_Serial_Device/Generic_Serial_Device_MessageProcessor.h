//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEGENERIC_SERIAL_DEVICE_MESSAGEPROCESSOR_H
#define DCEGENERIC_SERIAL_DEVICE_MESSAGEPROCESSOR_H

#include "MessageTranslation.h"
#include "DCE/Command_Impl.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class Generic_Serial_Device_MessageProcessor : public MessageProcessor {
public:
    Generic_Serial_Device_MessageProcessor();
    ~Generic_Serial_Device_MessageProcessor();

public:
	void setCommandImpl(Command_Impl* pcmdimpl) {
		pcmdimpl_ = pcmdimpl;
	}
	Command_Impl* getCommandImpl() {
		return pcmdimpl_;
	}
	
public:
	virtual bool Translate(Message* pmsg, MessageTimedQueue* pqueue);

	/*helper funcs*/
private:
	Command_Impl* FindTargetDevice(long devid);
	Command_Impl* FindTargetDevice(long devid, Command_Impl* pfromdev);
	
	
private:
	Command_Impl* pcmdimpl_;
};

};

#endif
