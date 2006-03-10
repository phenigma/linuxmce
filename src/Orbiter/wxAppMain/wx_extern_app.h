//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_EXTERN_APP_H_
#define _WX_EXTERN_APP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_extern_app.h"
#endif

#include "wxthread_cmd.h"

extern bool g_USE_EXTERN_APP_ON_IDLE;
extern bool g_USE_EXTERN_APP_ON_TIMER;
extern bool g_USE_EXTERN_APP_ON_THREAD;

#ifdef USE_RELEASE_CODE
struct SDL_Event_Loop_Data;
#endif // USE_RELEASE_CODE

class ExternApp : public wxThread_Cmd
{
public:
    ExternApp(int argc, char *argv[]);
    ~ExternApp();

    void Run();

    bool EventProcess();
    bool Destroy();

protected:
    enum E_STATUS
    {
        E_UNINITIALIZED = 0,
        E_CREATE_SUCCESS,
        E_CREATE_ERROR,
        E_RUN_DONE,
    };
    E_STATUS v_eStatus;
    int argc;
    char **argv;

#ifdef USE_RELEASE_CODE
    SDL_Event_Loop_Data *v_pSDL_Event_Loop_Data;
#endif // USE_RELEASE_CODE

private:
    bool CreateObjects();
    bool Initialize();
};

bool ExternApp_Run_NoWx(int argc, char *argv[]);

#endif
// _WX_EXTERN_APP_H_
