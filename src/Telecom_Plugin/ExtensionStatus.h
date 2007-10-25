//
// C++ Interface: ExtensionStatus
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EXTENSIONSTATUS_H
#define EXTENSIONSTATUS_H

#include <string>

using namespace std;

/**
The class which will keep all the information related to a telecom extension.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/

namespace DCE
{
	class ExtensionStatus
	{
		public:
			
			/***/
			enum Availability { Unregistered=0, Registered, UnknownAvailability };
			
			/***/
			enum Activity { Ready=0, OnThePhone, Ringing, Dialing, UnknownActivity };
			
			/***/
			enum ExtensionStatusFormat { esfExtension = 0, esfCallerID, esfState, NUM_FIELDS};

			/***/
			static string Availability2String( Availability );
			/***/
			static string Activity2String( Activity );
			/***/
			static Availability String2Availability( string );
			/***/
			static Activity String2Activity( string );
		
			/***/
			ExtensionStatus();
			/***/
			~ExtensionStatus() {}
			
			/***/
			void SetAvailability(Availability av) { availability = av; }
			/***/
			Availability GetAvailability() const { return availability; }
			
			/***/
			void SetActivity(Activity ac) { activity = ac; }
			/***/
			Activity GetActivity() const { return activity; }
			
		private:
			
			Availability availability;
			Activity activity;
			
	};
}

#endif
