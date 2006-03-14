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
//--struct SDL_Event_Loop_Data;
class SDL_App_Object;
#endif // USE_RELEASE_CODE

class ExternApp : public wxThread_Cmd
{
public:
    ExternApp(int argc, char *argv[]);
    ~ExternApp();

    void Run();

    bool Create();
    bool EventProcess();
    int Destroy();

protected:
    int argc;
    char **argv;

#ifdef USE_RELEASE_CODE
    SDL_App_Object *v_pSDL_App_Object;
#endif // USE_RELEASE_CODE
};

int ExternApp_Run_NoWx(int argc, char *argv[]);

#endif
// _WX_EXTERN_APP_H_
