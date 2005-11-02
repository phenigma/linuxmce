#include <iostream>
#include "UpdateMedia/MediaIdentifier.h"
#include "DCE/Logger.h"
using namespace std;

namespace DCE
{
		Logger *g_pPlutoLogger;
}
using namespace DCE;

int main(int argc, char* argv[]) 
{
	if(argc == 2)
		cout << "Media type: " << MediaIdentifier::GetFileMediaType(string(argv[1])) << endl;
	else
		cout << "Usage: MediaIdentifierTest <file>" << endl;
	
	return 0;
}
