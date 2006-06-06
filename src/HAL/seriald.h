//
// C++ Interface: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SERIALD_H
#define SERIALD_H

#include <vector>

namespace DCE
{
	class HAL;
}

using namespace DCE;

/**
The PnP daemon for serial connection.
 
	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class SerialD
{
	public:
		
		SerialD();
	
		~SerialD();
	
		/***/
		static void* startUp(void * HAL);
		
		/***/
		static void shutDown();
		
	private:
		
		static DCE::HAL * halDevice;
		
		static std::vector<std::string> serialDevices;
		
		static bool running;
};

#endif
