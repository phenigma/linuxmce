#include ".\prontoremoteaccess.h"

#include <string>
#include <iostream>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
static HWND g_hWnd = NULL;
//-------------------------------------------------------------------------------------------------------------
#define MAX_STRING 1024
//-------------------------------------------------------------------------------------------------------------
#define VERIFY_WINDOW() { \
    ::BringWindowToTop(g_hWnd); \
    ::SetForegroundWindow(g_hWnd); \
} 
//-------------------------------------------------------------------------------------------------------------
BOOL CALLBACK WindowsHandle(HWND hWnd, LPARAM lParam )
{
    static char pTitle[MAX_STRING];
    static char pClassName[MAX_STRING];
    ::GetWindowText(hWnd, pTitle, MAX_STRING);
    ::GetClassName(hWnd, pClassName, MAX_STRING);

    string sTitle(pTitle);
    if(string::npos != sTitle.find("Add IR Code", 0))
    {
        g_hWnd = hWnd;
        printf("Found the window:\nClassname: '%s'\nTitle: '%s'\n\n", pClassName, pTitle);    
        return FALSE;
    }

    return TRUE;
}
//-------------------------------------------------------------------------------------------------------------
ProntoRemoteAccess::ProntoRemoteAccess(void)
{
}
//-------------------------------------------------------------------------------------------------------------
ProntoRemoteAccess::~ProntoRemoteAccess(void)
{
}
//-------------------------------------------------------------------------------------------------------------
bool ProntoRemoteAccess::Attach()
{
    //finding pronto window
    ::EnumWindows(WindowsHandle, 0);

    if(!g_hWnd)
    {
        printf("IRCode window not found! Where is it?!\n");
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::SendKey(WORD wVirtualKey, DWORD dwDelay/* = 500*/)
{
    VERIFY_WINDOW();
    Sleep(dwDelay);

    static INPUT input;
    ::ZeroMemory(&input, sizeof(input));
    input.type           = INPUT_KEYBOARD;
    input.ki.wScan       = 0x50;
    input.ki.dwExtraInfo = 1;
    input.ki.wVk         = wVirtualKey;


    //down key
    input.ki.dwFlags = 0;
    ::SendInput(1, &input, sizeof(input));

    //up key
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    ::SendInput(1, &input, sizeof(input));
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::SendClick(int x, int y, DWORD dwDelay/* = 500*/) //not working yet
{
    Sleep(dwDelay);

    static INPUT input;
    ::ZeroMemory(&input, sizeof(input));
    input.type           = INPUT_MOUSE;
    input.mi.dx          = x * 65535 / 1280;
    input.mi.dy          = y * 65535 / 1024;
    input.mi.time        = 0;
    input.mi.dwExtraInfo = 0;
    input.mi.dwFlags     = MOUSEEVENTF_ABSOLUTE;

    //left down
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput(1, &input, sizeof(input));

    //left up
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    ::SendInput(1, &input, sizeof(input));
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::Reset()
{
    SendKey(VK_ESCAPE);
    SendKey(VK_SPACE);
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToDevice(int iDeviceNo)
{
    Reset();
    SendKey(VK_TAB);
    SendKey(VK_HOME);

    int iDeviceIterator = iDeviceNo;
    while(--iDeviceIterator)
        SendKey(VK_DOWN);

    printf(" - device: %d\n", iDeviceNo);
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToNextDevice()
{
    SendKey(VK_DOWN);
    printf(" - next device\n");
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToBrand(int iBrandNo) //relativ to device combobox!
{
    SendKey(VK_TAB);
    SendKey(VK_HOME);

    int iBrandIterator = iBrandNo;
    while(--iBrandIterator)
        SendKey(VK_DOWN);

    printf(" - brand: %d\n", iBrandNo);
}
//------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToNextBrand()
{
    SendKey(VK_DOWN);
    printf(" - next brand\n");
}
//------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToCodeSet(int iCodeSetNo) //relativ to brand combobox!
{
    SendKey(VK_TAB);
    SendKey(VK_HOME);

    int iCodeSetIterator = iCodeSetNo;
    while(--iCodeSetIterator)
        SendKey(VK_DOWN);

    printf(" - code set: %d\n", iCodeSetNo);
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToNextCodeSet()
{
    SendKey(VK_DOWN);
    printf(" - next code set\n");
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToFunction(int iFunctionNo) //relativ to code set combobox!
{
    SendKey(VK_TAB);
    SendKey(VK_HOME);

    int iFunctionIterator = iFunctionNo;
    while(--iFunctionIterator)
        SendKey(VK_DOWN);

    printf(" - function: %d\n", iFunctionNo);
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::GoToNextFunction()
{
    SendKey(VK_DOWN);
    printf(" - next function\n");
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::RestoreContext(int iDeviceNo, int iBrandNo, int iCodeSetNo, int iFunctionNo)
{
    printf("About to go to device %d, brand %d, code set %d, function %d\n", iDeviceNo, iBrandNo, iCodeSetNo, iFunctionNo);

    GoToDevice(iDeviceNo);
    GoToBrand(iBrandNo);
    GoToCodeSet(iCodeSetNo);
    GoToFunction(iFunctionNo);
}
//-------------------------------------------------------------------------------------------------------------
void ProntoRemoteAccess::LearnIR() //relativ to function list
{
    printf(" - learning IR...\n");    

    SendKey(VK_TAB);    
    SendKey(VK_TAB);
    SendKey(VK_TAB);
    SendKey(VK_TAB);
    SendKey(VK_SPACE);
}
//-------------------------------------------------------------------------------------------------------------