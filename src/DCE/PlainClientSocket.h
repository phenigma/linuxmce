/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef PlainClientSocket_H
#define PlainClientSocket_H

#include "ClientSocket.h"

class PlainClientSocket : public DCE::ClientSocket
{
public:
	PlainClientSocket(string IPAddress) :
		ClientSocket(1,IPAddress,"Plain Socket") {};
	bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );
};

#endif
