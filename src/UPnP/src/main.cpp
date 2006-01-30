/**
* The entry point in the application
*/

#include "main.h"
#include <string>

using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

int main()
{
	g_pPlutoLogger = new FileLogger("upnp.log");
	

}
