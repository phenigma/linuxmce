#pragma once

#include <windows.h> 
#include <winuser.h> 
#include <winnt.h> 
#include <winable.h>

class ProntoRemoteAccess
{
public:
    ProntoRemoteAccess(void);
    ~ProntoRemoteAccess(void);

    bool Attach();

    void SendKey(WORD wVirtualKey, DWORD dwDelay = 500);
    void SendClick(int x, int y, DWORD dwDelay = 500);
    void Reset();
    void GoToDevice(int iDeviceNo);
    void GoToNextDevice();
    void GoToBrand(int iBrandNo);
    void GoToNextBrand();
    void GoToCodeSet(int iCodeSetNo);
    void GoToNextCodeSet();
    void GoToFunction(int iFunctionNo);
    void GoToNextFunction();

    void RestoreContext(int iDeviceNo, int iBrandNo, int iCodeSetNo, int iFunctionNo);
    void LearnIR();
};
