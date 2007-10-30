//
// C++ Interface: AssistedTransfer
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASSISTEDTRANSFER_H
#define ASSISTEDTRANSFER_H

#include "TelecomTask.h"

namespace DCE
{
	/**
	The telecom task which performs an assisted transfer.
	
		@author Eugen Constantinescu <eugen.c@plutohome.com>
	*/
	class AssistedTransfer : public TelecomTask
	{
		public:
			
			/***/
			AssistedTransfer(	const string & ext,
								const string & callid,
								const string & channelid,
								const string & mycallid );
			/***/
			virtual ~AssistedTransfer();
			
			/***/
			virtual bool ProcessJob(const string &);
			/***/
			virtual TelecomTask::TaskType RTTI() const { return TelecomTask::Assisted_Transfer; }
			
		private:
			
			string sExtDest;
			string sCallID_Dest;
			string sMyChannelID;
			string sMyCallID;
	};
};
#endif
