#ifndef __BDCommandProcessor_Symbian_Bluetooth_h
#define __BDCommandProcessor_Symbian_Bluetooth_h

#include "BDCommandProcessor_Symbian_Base.h"

class BDCommandProcessor_Symbian_Bluetooth:public BDCommandProcessor_Symbian_Base {
	public:
		BDCommandProcessor_Symbian_Bluetooth(const string& sMacAdress, MBluetoothListener* aListener):
		  BDCommandProcessor_Symbian_Base(sMacAdress, aListener) {}
		~BDCommandProcessor_Symbian_Bluetooth() {
			Close();
		}

		
		void Start();
};

#endif
