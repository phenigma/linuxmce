//#include <unistd.h>
#include <math.h>

#include <iostream>
#include <strstream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

#include "DCE/Logger.h"
#include "SerializeClass/ShapesColors.h"

namespace DCE
{
    Logger *g_pPlutoLogger = NULL;
}

using namespace DCE; 

int main(int argc, char *argv[])
{
cout << argc << endl;
	if (argc!=3 || argc!=6)
	{
		cout << "convert_color [color as 32-bit Argb]|[r g b a]" << endl;
		exit(0);
	}
	
}

