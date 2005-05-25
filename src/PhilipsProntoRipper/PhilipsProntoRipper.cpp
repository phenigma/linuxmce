// PhilipsProntoRipper.cpp : Defines the entry point for the console application.
//

#include <windows.h> 
#include <winuser.h> 
#include <winnt.h> 
#include <winable.h>

#include "ProntoRemoteAccess.h"

#include <memory>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::auto_ptr<ProntoRemoteAccess>(psmartProntoRemoteAccess);
    if(psmartProntoRemoteAccess->Attach())
    {
        psmartProntoRemoteAccess->RestoreContext(2, 3, 1, 5);
        psmartProntoRemoteAccess->LearnIR();
    }

    return 0;
}
//-------------------------------------------------------------------------------------------------------------
