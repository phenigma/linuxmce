#include <iostream>
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/ShapesColors.h"

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{

	//	WSADATA wsaData;
	//	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif

	if( argc!=2 && argc!= 4 && argc!=5 )
	{
		cout << "Usage:" << endl;
		cout << "To convert from a 32 bit number to R,G,B,A values: ColorPicker [single 32 bit number]" << endl;
		cout << "To convert from R,G,B,A to a 32 bit number: ColorPicker R G B [A]" << endl;
		cout << "Opaque (255) is assumed if A is not specified" << endl;
		return 0;
	}

	if( argc==2 )
	{
		PlutoColor c( atoi(argv[1]) );
		cout << "R: " << c.R() << " G: " << c.G() << " B: " << c.B() << " A: " << c.A() << endl;
	}
	else
	{
		PlutoColor c( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]) );
		if( argc==5 )
			c.A( atoi(argv[4]) );
		else
			c.A(255);

		cout << "Value as signed int: " << c.AsARGB() << " unsigned: " << (unsigned long) c.AsARGB() << endl;
	}
}
