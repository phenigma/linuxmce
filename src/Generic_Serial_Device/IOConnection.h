/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCEIOCONNECTION_H
#define DCEIOCONNECTION_H

namespace DCE {

/**
@author Igor Spac,,,
*/

class IOConnection {
public:
	virtual bool Open() = 0;
	virtual void Close() = 0;
	
	virtual int Send(const char* buff, unsigned int size) = 0;
	virtual int Recv(char* buff, unsigned int size, int timeout) = 0;

	virtual bool isOpened() = 0;
	virtual bool isDataAvailable(int timeout) = 0;

	virtual void Flush() = 0;
};

};

#endif
