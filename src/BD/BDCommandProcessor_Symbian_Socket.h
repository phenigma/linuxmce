#ifndef __BDCommandProcessor_Symbian_Socket_h
#define __BDCommandProcessor_Symbian_Socket_h

#include "BDCommandProcessor_Symbian_Base.h"

class BDCommandProcessor_Symbian_Socket:public BDCommandProcessor_Symbian_Base{
	private:
		TUint32 iServerAddress;
	public:
		BDCommandProcessor_Symbian_Socket(const string& sMacAddressPhone, TUint32 sServerAddress):BDCommandProcessor_Symbian_Base(sMacAddressPhone), iServerAddress(sServerAddress) {}
		
		void Start();
		~BDCommandProcessor_Symbian_Socket() {
			Close();
		}
};

#endif
