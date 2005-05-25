#include "CodeInjectionManager.h"
#include "InjCode.h"

#include <stdio.h>
//-------------------------------------------------------------------------------------------------------------
int SetSelfPrivilege(LPSTR privlg, char doenable)
{
    int ret = 3;
    HANDLE tokh = NULL;
    TOKEN_PRIVILEGES tkp = { 0 };

    for(;;)
    {
        //get a handle to our process token
        if(!OpenProcessToken(GetCurrentProcess(), 
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, 
            &tokh)) break;
        ret = 2;

        //now get our current privilege settings
        if(!LookupPrivilegeValue(NULL, privlg, 
            &tkp.Privileges[0].Luid)) break;
        ret = 1;

        //set the proper struct members
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = (doenable) ? SE_PRIVILEGE_ENABLED : 0;

        //now adjust the privileges
        AdjustTokenPrivileges(tokh, FALSE, &tkp, 
            0, NULL, 0);
        if(GetLastError() != ERROR_SUCCESS) break;

        ret = 0;
        break;
    }

    //clean up after ourselves
    if(tokh != NULL) CloseHandle(tokh);

    return ret;
}
//-------------------------------------------------------------------------------------------------------------
int InjectCode(HWND hWnd)
{
    DWORD dwProcessId = 0;;
    DWORD dwThreadID = ::GetWindowThreadProcessId(hWnd, &dwProcessId);

    if(!SetSelfPrivilege(SE_DEBUG_NAME, TRUE)) //enable debug privilge
    {
        HANDLE hProcess = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);

        if(hProcess != NULL)
        {
            SetSelfPrivilege(SE_DEBUG_NAME, FALSE); //disable debug privilege now that we've opened the process

            //TODO
            char test[1024];
            GetWindowTextRemote(hProcess, hWnd, test);
            ::CloseHandle(hProcess);
        }
        else
        {
            printf("Unable to get process handler for %d pid!", dwProcessId);
            return 1;
        }
    }

    return 0;
}